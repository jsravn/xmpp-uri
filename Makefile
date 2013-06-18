#
# xmpp-uri Makefile
#
# Copyright 2013 James Ravn
#
# Heavily inspired and copied from pidgin-latex's Makefile.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

CC := gcc
LIBTOOL := libtool

ifeq ($(PREFIX),)
	LIB_INSTALL_DIR = $(HOME)/.purple/plugins
else
	LIB_INSTALL_DIR = $(PREFIX)/lib/pidgin
endif

PIDGIN_CFLAGS = $(shell pkg-config pidgin --cflags)
GTK_CFLAGS = $(shell pkg-config gtk+-2.0 --cflags)
PIDGIN_LIBS = $(shell pkg-config pidgin --libs)
GTK_LIBS = $(shell pkg-config gtk+-2.0 --libs)
PIDGIN_LIBDIR = $(shell pkg-config --variable=libdir pidgin)/pidgin

all: xmpp_uri.so

install: all
	mkdir -p $(LIB_INSTALL_DIR)
	cp xmpp_uri.so $(LIB_INSTALL_DIR)

xmpp_uri.so: xmpp_uri.o
	$(CC) -shared $(CFLAGS) $< -o $@ $(PIDGIN_LIBS) $(GTK_LIBS) -Wl,--export-dynamic -Wl,-soname

xmpp_uri.o: xmpp_uri.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@ $(PIDGIN_CFLAGS) $(GTK_CFLAGS) -DHAVE_CONFIG_H

clean:
	rm -rf *.o *.c~ *.h~ *.so *.la .libs
