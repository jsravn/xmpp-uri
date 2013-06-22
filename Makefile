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

CC = gcc
CFLAGS = -O2 -Wall -g --std=gnu99

ifeq ($(PREFIX),)
	LIB_INSTALL_DIR = $(HOME)/.purple/plugins
else
	LIB_INSTALL_DIR = $(PREFIX)/lib/pidgin
endif

CFLAGS += $(shell pkg-config --cflags pidgin gtk+-2.0)
LIBS := $(shell pkg-config --libs pidgin gtk+-2.0)

.PHONY: all install clean

all: xmpp_uri.so

install: all
	mkdir -p $(LIB_INSTALL_DIR)
	cp xmpp_uri.so $(LIB_INSTALL_DIR)

xmpp_uri.so: xmpp_uri.o
	$(CC) -shared $(CFLAGS) $< -o $@ $(LIBS)

xmpp_uri.o: xmpp_uri.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

clean:
	rm -rf *.o *.so
