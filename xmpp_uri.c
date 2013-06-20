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

#include <assert.h>
#include <glib.h>
#include <string.h>

#include "account.h"
#include "conversation.h"
#include "core.h"
#include "debug.h"
#include "gtkplugin.h"
#include "gtkimhtml.h"
#include "plugin.h"
#include "version.h"

#define PLUGIN_ID           "gtk-jravn-xmppuri"
#define PLUGIN_NAME         "XMPP URI Handler"
#define PLUGIN_STATIC_NAME  "xmpp_uri"
#define PLUGIN_VERSION      "0.2-dev"
#define PLUGIN_SUMMARY      "Handler for inline XMPP URI links (XEP-0147)."
#define PLUGIN_DESCRIPTION  "This plugin adds support for clicking on XMPP "\
                            "URI links inside of conversation windows. "\
                            "The URI scheme is defined by XEP-0147. "\
                            "Links take the form of "\
                            "'xmpp:user@domain.com?query;param=value'. The "\
                            "supported queries are:\n\n"\
                            "  - ?message;body=hello%20world\n"
#define PLUGIN_AUTHOR       "James Ravn <james.ravn@gmail.com>"
#define PLUGIN_WEBSITE      "https://github.com/jsravn/xmpp-uri"

static PurplePlugin *plugin_instance = NULL;

// Methods

static gint match_proto(const void *acct, const void *proto) {
    const char *acct_proto = purple_account_get_protocol_name(acct);
    return g_strcmp0(acct_proto, proto);
}

/**
 * @Nullable if no active accounts
 */
static PurpleAccount *find_acct(GtkIMHtml *imhtml) {
    const char *proto = gtk_imhtml_get_protocol_name(imhtml);
    GList *accts = purple_accounts_get_all_active();
    if (!accts) {
        purple_debug_warning(PLUGIN_ID, "No active accounts found\n");
        return NULL;
    }

    PurpleAccount *acct = NULL;
    GList *acct_item = g_list_find_custom(accts, proto, match_proto); 
    if (!acct_item) {
        purple_debug_warning(PLUGIN_ID, "No account for %s found, using first"\
                "\n", proto);
        acct = g_list_first(accts)->data;
    } else {
        acct = acct_item->data;
    }

    g_list_free(accts);
    return acct;
}

/**
 * Creates a hash table of param to value pairs. The returned
 * hash table should be destroyed by the calling function.
 */
static GHashTable *find_params(const char *query) {
    GHashTable *params = g_hash_table_new_full(g_str_hash, g_str_equal,
           g_free, g_free); 

    const char *start_of_params = strchr(query, ';');
    if (!start_of_params) return params;

    char *params_str = strdup(start_of_params);
    char *saveptr1, *saveptr2;
    const char *start_of_param = strtok_r(params_str, ";", &saveptr1);

    while (start_of_param != NULL) {
        char *param_str = strdup(start_of_param);
        const char *key = strtok_r(param_str, "=", &saveptr2);
        const char *value = strtok_r(NULL, "=", &saveptr2);

        if (key && value) {
            g_hash_table_insert(params, strdup(key), strdup(value));
        }
        free(param_str);

        start_of_param = strtok_r(NULL, ";", &saveptr1);
    }

    free(params_str);

    return params;
}

static gboolean handle_message(PurpleAccount *acct, const char *jid,
        GHashTable *params) {
    const char *body = g_hash_table_lookup(params, "body");

    PurpleConversation *conv = purple_conversation_new(PURPLE_CONV_TYPE_IM,
        acct, jid);
    purple_conversation_present(conv);
    if (body) {
        purple_conv_send_confirm(conv, body);
    }

    return TRUE;
}

static void append_key_value(gpointer key, gpointer value, gpointer s) {
    g_string_append(s, "[");
    g_string_append(s, key);
    g_string_append(s, " -> ");
    g_string_append(s, value);
    g_string_append(s, "] ");
}

static char *to_string(GHashTable *params) {
    GString *s = g_string_new("");
    g_hash_table_foreach(params, append_key_value, s);
    return g_string_free(s, FALSE);
}

static void log_action(PurpleAccount *acct, const char *jid,
        const char *action, GHashTable *params) {
    if (!purple_debug_is_enabled) return;

    purple_debug_misc(PLUGIN_ID, "Account: %s\n",
            purple_account_get_username(acct));
    purple_debug_misc(PLUGIN_ID, "Jid: %s\n", jid);
    purple_debug_misc(PLUGIN_ID, "Action: %s\n", action);
    char *params_str = to_string(params);
    purple_debug_misc(PLUGIN_ID, "Parameters: %s\n", params_str);
    free(params_str);
}

static gboolean handle_action(PurpleAccount *acct, const char *jid,
        const char *action, GHashTable *params) {
    log_action(acct, jid, action, params);

    if (g_str_equal(action, "?message")) {
        return handle_message(acct, jid, params);
    } else {
        purple_debug_warning(PLUGIN_ID, "Unrecognized action, doing nothing");
        return FALSE;
    }
}

static gboolean handle_uri(GtkIMHtml *imhtml, GtkIMHtmlLink *link) {
    const char *url = gtk_imhtml_link_get_url(link);
    purple_debug_misc(PLUGIN_ID, "XMPP URI clicked: '%s'\n", url);

    if (!g_str_has_prefix(url, "xmpp:")) {
        purple_debug_error(PLUGIN_ID, "bug: unexpected protocol");
        return FALSE;
    }

    char *query = strchr(url, '?');
    if (!query) {
        purple_debug_warning(PLUGIN_ID, "No query, ignoring. Did you forget a "
                "'?message' ?");
        return FALSE;
    }

    PurpleAccount *acct = find_acct(imhtml);
    if (!acct) {
        return FALSE;
    }

    const char *start_of_jid = url + strlen("xmpp:");
    char *jid = strndup(start_of_jid, query - start_of_jid);

    const char *end_of_action = strchr(query, ';');
    if (!end_of_action) end_of_action = strchr(query, '\0');
    char *action = strndup(query, end_of_action - query);

    GHashTable *params = find_params(query);

    gboolean r = handle_action(acct, jid, action, params);

    free(jid);
    free(action);
    g_hash_table_destroy(params);
    return r;
}

static gboolean plugin_load(PurplePlugin *plugin) {
	gtk_imhtml_class_register_protocol("xmpp:", handle_uri, NULL);

    purple_debug_info(PLUGIN_ID, "Loaded\n");
    return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin) {
	gtk_imhtml_class_register_protocol("xmpp:", NULL, NULL);

    purple_debug_info(PLUGIN_ID, "Unloaded\n");
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
    plugin_unload,                  /* unload               */
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
    plugin_instance = plugin;
}

PURPLE_INIT_PLUGIN(PLUGIN_STATIC_NAME, init_plugin, info)
