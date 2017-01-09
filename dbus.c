#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <gio/gio.h>
#include <dbus/dbus.h>
#include <assert.h>

#include "dbus.h"
#include "datatypes.h"
#include "yarn.h"
//#include "config.h"

// mainly from dunst, demistifyed a lot of dbus in c for me.

// in dunst, used for emiting the NotificationClosed signal later.
GDBusConnection *conn;

// xml data to be parsed and turned into node.
static GDBusNodeInfo *introspection_data = NULL;
static const char *introspection_xml =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<node name=\"/org/freedesktop/Notifications\">"
    "    <interface name=\"org.freedesktop.Notifications\">"

    "        <method name=\"GetCapabilities\">"
    "            <arg direction=\"out\" name=\"capabilities\"    type=\"as\"/>"
    "        </method>"

    "        <method name=\"Notify\">"
    "            <arg direction=\"in\"  name=\"app_name\"        type=\"s\"/>"
    "            <arg direction=\"in\"  name=\"replaces_id\"     type=\"u\"/>"
    "            <arg direction=\"in\"  name=\"app_icon\"        type=\"s\"/>"
    "            <arg direction=\"in\"  name=\"summary\"         type=\"s\"/>"
    "            <arg direction=\"in\"  name=\"body\"            type=\"s\"/>"
    "            <arg direction=\"in\"  name=\"actions\"         type=\"as\"/>"
    "            <arg direction=\"in\"  name=\"hints\"           type=\"a{sv}\"/>"
    "            <arg direction=\"in\"  name=\"expire_timeout\"  type=\"i\"/>"
    "            <arg direction=\"out\" name=\"id\"              type=\"u\"/>"
    "        </method>"

    "        <method name=\"CloseNotification\">"
    "            <arg direction=\"in\"  name=\"id\"              type=\"u\"/>"
    "        </method>"

    "        <method name=\"GetServerInformation\">"
    "            <arg direction=\"out\" name=\"name\"            type=\"s\"/>"
    "            <arg direction=\"out\" name=\"vendor\"          type=\"s\"/>"
    "            <arg direction=\"out\" name=\"version\"         type=\"s\"/>"
    "            <arg direction=\"out\" name=\"spec_version\"    type=\"s\"/>"
    "        </method>"

    "        <signal name=\"NotificationClosed\">"
    "            <arg name=\"id\"         type=\"u\"/>"
    "            <arg name=\"reason\"     type=\"u\"/>"
    "        </signal>"

    "        <signal name=\"ActionInvoked\">"
    "            <arg name=\"id\"         type=\"u\"/>"
    "            <arg name=\"action_key\" type=\"s\"/>"
    "        </signal>"
    "   </interface>"
    "</node>";

// forward declaration for functions
static void
onGetCapabilities(GDBusConnection *connection,
                  const gchar *sender,
                  const GVariant *parameters,
                  GDBusMethodInvocation *invocation);
static void
onNotify(GDBusConnection *connection,
         const gchar *sender,
         GVariant *parameters,
         GDBusMethodInvocation *invocation);

static void
onCloseNotification(GDBusConnection *connection,
                    const gchar *sender,
                    GVariant *parameters,
                    GDBusMethodInvocation *invocation);
static void
onGetServerInformation(GDBusConnection *connection,
                       const gchar *sender,
                       const GVariant *parameters,
                       GDBusMethodInvocation *invocation);





// when a method is sent, check what it wants to do and call the appropriate function.
void
handle_method_call(GDBusConnection *connection,
                   const gchar *sender,
                   const gchar *object_path,
                   const gchar *interface_name,
                   const gchar *method_name,
                   GVariant *parameters,
                   GDBusMethodInvocation *invocation, gpointer user_data)
{
    if (g_strcmp0(method_name, "GetCapabilities") == 0) {
        onGetCapabilities(connection, sender, parameters, invocation);
    } else if (g_strcmp0(method_name, "Notify") == 0) {
        onNotify(connection, sender, parameters, invocation);
    } else if (g_strcmp0(method_name, "CloseNotification") == 0) {
        onCloseNotification(connection, sender, parameters, invocation);
    } else if (g_strcmp0(method_name, "GetServerInformation") == 0) {
        onGetServerInformation(connection, sender, parameters, invocation);
    } else {
        printf("WARNING: sender: %s; unknown method_name: %s\n", sender, method_name);
    }
}

// the appropriate functions.
static void
onGetCapabilities(GDBusConnection *connection,
                  const gchar *sender,
                  const GVariant *parameters,
                  GDBusMethodInvocation *invocation)
{
    GVariantBuilder *builder;
    GVariant *value;

    builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
    g_variant_builder_add(builder, "s", "actions");
    g_variant_builder_add(builder, "s", "body");
    g_variant_builder_add(builder, "s", "body-markup");
    value = g_variant_new("(as)", builder);
    g_variant_builder_unref(builder);
    g_dbus_method_invocation_return_value(invocation, value);

    g_dbus_connection_flush(connection, NULL, NULL, NULL);
    g_variant_unref(value);
}

static void
onNotify(GDBusConnection *connection,
         const gchar *sender,
         GVariant *parameters,
         GDBusMethodInvocation *invocation)
{
    gchar *app_name = NULL;
    guint replaces_id = 0;
    gchar *app_icon = NULL;
    gchar *summary = NULL;
    gchar *body = NULL;
    gint  expire_timeout = -1;

    {
        GVariantIter *iter = g_variant_iter_new(parameters);
        GVariant *content;
        int i = 0;
        while ((content = g_variant_iter_next_value(iter)))
        {
            switch(i) 
            {
                case 0:
                    if (g_variant_is_of_type(content, G_VARIANT_TYPE_STRING))
                        app_name = g_variant_dup_string(content, NULL);
                    break;
                case 1:
                    if (g_variant_is_of_type(content, G_VARIANT_TYPE_UINT32))
                        replaces_id = g_variant_get_uint32(content);
                    break;
                case 2:
                    if (g_variant_is_of_type(content, G_VARIANT_TYPE_STRING))
                        app_icon = g_variant_dup_string(content, NULL);
                    break;
                case 3:
                    if (g_variant_is_of_type(content, G_VARIANT_TYPE_STRING))
                        summary = g_variant_dup_string(content, NULL);
                    break;
                case 4:
                    if (g_variant_is_of_type(content, G_VARIANT_TYPE_STRING))
                        body = g_variant_dup_string(content, NULL);
                    break;
                case 5:
                    break;
                case 6:
                    break;
                case 7:
                    if (g_variant_is_of_type(content, G_VARIANT_TYPE_INT32))
                        expire_timeout = g_variant_get_int32(content);
                    break;
            }
        g_variant_unref(content);
        i++;
        }
    g_variant_iter_free(iter);
    }

    Notification *n = malloc(sizeof(Notification));
    assert (n != NULL);

    n->app_name = app_name;
    n->replaces_id = replaces_id;
    n->app_icon = app_icon;
    n->summary = summary;
    n->body = body;
    n->expire_timeout = expire_timeout;

    int id = 1;
    GVariant *reply = g_variant_new("(u)", id);
    g_dbus_method_invocation_return_value(invocation, reply);
    g_dbus_connection_flush(connection, NULL, NULL, NULL);

    g_print("About to print the summary from yarn.c\n");

    prepare(n);
}

static void
onCloseNotification(GDBusConnection *connection,
                    const gchar *sender,
                    GVariant *parameters,
                    GDBusMethodInvocation *invocation)
{
    guint32 id;
    g_variant_get(parameters, "(u)", &id);
    //notification_close_by_id(id, 3);
    g_dbus_method_invocation_return_value(invocation, NULL);
    g_dbus_connection_flush(connection, NULL, NULL, NULL);
}

static void
onGetServerInformation(GDBusConnection *connection,
                       const gchar *sender,
                       const GVariant *parameters,
                       GDBusMethodInvocation *invocation)
{
    GVariant *value;

    value = g_variant_new("(ssss)", "dunst", "knopwob", "12", "1.2");
    g_dbus_method_invocation_return_value(invocation, value);

    g_dbus_connection_flush(connection, NULL, NULL, NULL);
}

// ------------ default bus items ------------
// export objects onto the bus here.
static const GDBusInterfaceVTable interface_vtable = { handle_method_call };

static void
on_bus_acquired (GDBusConnection *connection,
                 const gchar *name,
                 gpointer user_data)
{
    //guint registration_id;
    //registration_id = 
    g_dbus_connection_register_object(connection,
                                      "/org/freedesktop/Notifications",
                                      introspection_data->interfaces[0],
                                      &interface_vtable,
                                      NULL, NULL, NULL);
}

static void
on_name_acquired (GDBusConnection *connection,
                  const gchar *name,
                  gpointer user_data)
{
    g_print ("Acquired the name %s on the session bus\n", name);
}

static void
on_name_lost (GDBusConnection *connection,
              const gchar *name,
              gpointer user_data)
{
    g_print ("Lost the name %s on the session bus\n", name);
}
// ------------ default bus items ------------



int
//main (int argc, char *argv[])
yarn_init(void)
{
    //guint owner_id;

    // parse the xml data and turn it into a node.
    introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, NULL);

    g_bus_own_name(G_BUS_TYPE_SESSION,
                   "org.freedesktop.Notifications",
                   G_BUS_NAME_OWNER_FLAGS_NONE,
                   on_bus_acquired,
                   on_name_acquired, on_name_lost, NULL, NULL);

    // loop and wait for events.
    GMainLoop *loop;

    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);

  return 0;
}

/* kill notifications here
void
dbus_tear_down(int owner_id)
{
    g_bus_unown_name(owner_id);
}
*/

