include config.mk

SRC := $(shell find src/ -name '*.c')

CC=clang

all:	service yarn

service:
	@sed "s|##PREFIX##|$(PREFIX)|" svc/org.toqoz.yarn.service.in > svc/org.toqoz.yarn.service
	@sed "s|##PREFIX##|$(PREFIX)|" svc/yarn.systemd.service.in > svc/yarn.service

yarn:
	$(CC) ${CFLAGS}	${SRC} 	$(LDFLAGS) -o yarn
clean:
	rm -f yarn
	rm -f svc/org.toqoz.yarn.service
	rm -f svc/yarn.service

install-service: service
	mkdir -p ${DESTDIR}${PREFIX}/share/dbus-1/services/
	install -m644 svc/org.toqoz.yarn.service ${DESTDIR}${PREFIX}/share/dbus-1/services
	install -Dm644 svc/yarn.service ${DESTDIR}${PREFIX}/lib/systemd/user/yarn.service

install-yarn: yarn
	mkdir -p ${DESTDIR}${PREFIX}/bin
	install -m 755 yarn ${DESTDIR}${PREFIX}/bin/

install: install-service install-yarn

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/yarn
	rm -f ${DESTDIR}${PREFIX}/share/dbus-1/services/org.toqoz.yarn.service
	rm -f ${DESTDIR}${PREFIX}/lib/systemd/user/yarn.service
	#rm -rf ${DESTDIR}${PREFIX}share/yarn
