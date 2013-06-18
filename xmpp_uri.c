/*
 * Copyright 2013 James Ravn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define PURPLE_PLUGINS

#include <glib.h>

#include "debug.h"
#include "gtkplugin.h"
#include "notify.h"
#include "plugin.h"
#include "version.h"

#define PLUGIN_ID           "gtk-jravn-xmppuri"
#define PLUGIN_NAME         "XMPP URI Handler"
#define PLUGIN_STATIC_NAME  "xmpp_uri"
#define PLUGIN_VERSION      "0.1"
#define PLUGIN_SUMMARY      "Handler for XMPP URI links (XEP-1047)."
#define PLUGIN_DESCRIPTION  "This plugin adds support for XMPP URI links as "\
                            "specified by XEP-1047. These links take the "\
                            "form of 'xmpp:user@domain.com?query'. The "\
                            "supported queries are:\n\n"\
                            "  - ?message - opens a message window to the "\
                                           "user\n"
#define PLUGIN_AUTHOR       "James Ravn <james.ravn@gmail.com>"
#define PLUGIN_WEBSITE      "https://github.com/jsravn/xmpp-uri"

static gboolean
plugin_load(PurplePlugin *plugin) {
    purple_debug_info(PLUGIN_ID, "Loaded\n");
    return TRUE;
}


static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,            /* Magic                */
    PURPLE_MAJOR_VERSION,           /* Purple Major Version */
    PURPLE_MINOR_VERSION,           /* Purple Minor Version */
    PURPLE_PLUGIN_STANDARD,         /* plugin type          */
    PIDGIN_PLUGIN_TYPE,             /* ui requirement       */
    0,                              /* flags                */
    NULL,                           /* dependencies         */
    PURPLE_PRIORITY_DEFAULT,        /* priority             */

    PLUGIN_ID,                      /* plugin id            */
    PLUGIN_NAME,                    /* name                 */
    PLUGIN_VERSION,                 /* version              */
    PLUGIN_SUMMARY,                 /* summary              */
    PLUGIN_DESCRIPTION,             /* description          */
    PLUGIN_AUTHOR,                  /* author               */
    PLUGIN_WEBSITE,                 /* website              */

    plugin_load,                    /* load                 */
    NULL,                           /* unload               */
    NULL,                           /* destroy              */

    NULL,                           /* ui_info              */
    NULL,                           /* extra_info           */
    NULL,                           /* prefs_info           */
    NULL,                           /* actions              */

    /* padding */
    NULL,
    NULL,
    NULL,
    NULL
};

static void
init_plugin(PurplePlugin *plugin)
{
}

PURPLE_INIT_PLUGIN(PLUGIN_STATIC_NAME, init_plugin, info)
