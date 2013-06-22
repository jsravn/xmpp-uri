xmpp-uri
========

Xmpp-uri is a pidgin plugin that adds support for clicking on XMPP URI links
([XEP-0147](http://xmpp.org/extensions/xep-0147.html)).

This allows you to click on links like `xmpp:romeo@montague.net?message` and
automatically open a chat window. Bots can generate such links for quickly
joining chatrooms or messaging other users.

Installing
==========

Pidgin and glib libraries must be installed, along with the usual build tools.

```bash
$ git clone https://github.com/jsravn/xmpp-uri.git
$ cd xmpp-uri && make install
```

Then enable the xmpp-uri plugin in Pidgin and you should be set.

Supported actions are described in the plugin menu.

Windows binary
==============

[Download](https://sourceforge.net/projects/xmppuri/files/) and put in your
`%APPDATA%/.purple/plugins` directory. Restart pidgin and enable it.

Building on Windows
===================

The plugin can also be cross compiled for Windows, but it's a bit more involved.

Make sure you can [build
pidgin](https://developer.pidgin.im/wiki/BuildingWinPidgin) - the same
environment is needed to build the plugin.

Then build by pointing to Makefile.mingw.

```bash
$ ... # build pidgin inside of ~/devel/pidgin-2.10.7
$ cd ~/devel
$ git clone https://github.com/jsravn/xmpp-uri.git
$ cd xmpp-uri && make -f Makefile.mingw
```

You will probably have to change HOST in the Makefile.mingw depending on how
your distribution installs mingw.
