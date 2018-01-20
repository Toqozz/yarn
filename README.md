# yarn

yarn is a light notification daemon that includes some quality-of-life features.  yarn is built for users who may have and/or want to read multiple notifications at the same time; think chat programs (pidgin et al.).

![yarn example](http://i.imgur.com/2fMdMJw.gif)

I've tried to include as many options to customize the appearance as possible.  If you have further ideas, create a new issue and tag it as suggestion/improvement!

----------

## features

- Highly customizable.
	- Scroll speed, color, corner rounding, shadows, font, margins, text offsets, message dimensions and offsets.
- (Lightweight).
	- By default, yarn is less efficient on average than notification daemons that draw a single time, because it repaints often to achieve a scrolling effect.  The effect is negligible on any processor from the past decade.
	- You can disable scrolling to achieve on-par performance.
- Multiple notifications at once...
	- The main motivation behind this project is because almost no notification daemons (none??) show multiple notifications at one time.

## drawbacks

yarn draws on a single X Window.  This allows yarn to be much more flexible with its appearance and behaviour (and faster), but has a couple of drawbacks:

- Shadows are handled by yarn, not the compositor.
- Transparency (between notifications and such) is handled by yarn.

What this means is that you'll have to configure your compositor to ignore yarn for shadow purposes (well you don't have to, but it may look nicer).  This is pretty easy with compton, simply add `shadow-exclude = ["n:e:yarn"];` to your config.

## install

### arch linux
yarn will be in the AUR soon.

### other / building from source
Dependencies (with their common package names):

- dbus (`dbus-1`)
- glib 2 (`glib2`)
- cairo (`cairo`)
- pango (`pango`)
- libconfig (`libconfig`)

Install steps:

1. Clone the repository with `git clone https://github.com/Toqozz/yarn.git`.
2. Step into the directory: `cd ./yarn`.
3. Compile and install: `sudo make all install`

Service files will be installed to `/usr/local/share/dbus-1/services/org.toqoz.yarn.service` and `/usr/local/lib/systemd/user/yarn.service`.  
yarn itself will be installed to `/usr/local/bin/yarn`.

From here, you can then use `systemctl --user enable yarn` and `systemctl --user start yarn` to enable / start yarn.


To uninstall, just run `sudo make uninstall` and these files will be removed.

## config

Please copy the example [config](https://github.com/Toqozz/yarn/blob/master/config) to `~/.config/yarn/config`.  Currently sane defaults dont exist, so yarn won't work without this (sorry!).

The example config is heavily commented, so each option should make sense.

## *TODO*

- Sane config defaults.
- Implement option to start scrolling from a specified point in the message.
- Optimize draw loop when scrolling is turned off.
- Scroll from a different side.
- Images from `notify-send` (album art, application icon, etc).
- ~~Bounce back and scroll through the whole message.~~
