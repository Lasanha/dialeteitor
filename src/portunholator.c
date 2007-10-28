/*
 * Portunholator
 * Copyright 2007 Luiz Irber <luiz.irber@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#include <regex.h>
#include <string.h>

#define PURPLE_PLUGINS
#define PLUGIN_ID "core-portunholator"

#include "cmds.h"
#include "conversation.h"
#include "plugin.h"
#include "util.h"
#include "version.h"

PurplePlugin *plugin_handle = NULL;

static PurpleCmdId port;

typedef struct {
    const gchar* re;
    const gchar* subtext;
} regexp;

static const regexp regexps[] = {
    /* palavras */
    {"\\beu\\b", "jo"},
    {"\\bumas\\b", "pero"},
    {"\\buma\\b", "una"},
    {"\\bum\\b", "uno"},
    {"\\b(minha|meu)\\b", "mi"},
    {"\\b(s|t)(ua|eu)\\b", "\\1u"},
    {"\\b(tu|você)\\b", "usted"},
    {"\\bdo\\b", "del"},
    {"\\bem\\b", "en"},
    {"\\bomo\\b", "bueno"},
    {"\\b(a|o)(s?)\\b", "l\\1\\2"},
    {"\\bé\\b", "es"},
    {"\\bcara\\b", "cabrón"},
    {"\\bhoje\\b", "hoy"},

    /* pedacos */
    {"\\b(\\w{2,}?)ção\\b", "\\1ción"},
    {"\\b(\\w{2,}?)ções\\b", "\\1ciónes"},
    {"\\b(\\w{3,}?)ão\\b", "\\1ión"},
    {"\\b(\\w{3,}?)ões\\b", "\\1iónes"},
    {"\\b(\\w*)inh(a|o)\\b", "\\1it\\2"},
    {"\\b(\\w)o(\\w{2,3})\\b", "\\1ue\\2"},
    {"\\b(\\w)e(\\w{2,3})\\b", "\\1ie\\2"},
    {"\\b(\\w{3,}?)dade\\b", "\\1dad"},
    {NULL, NULL}
};

static int rreplace (char *buf, int size, regex_t *re, gchar *rp)
{
    char *pos;
    int sub, so, n;
    regmatch_t pmatch [10]; /* regoff_t is int so size is int */

    if (regexec (re, buf, 10, pmatch, 0)) return 0;
    for (pos = rp; *pos; pos++)
        if (*pos == '\\' && *(pos + 1) > '0' && *(pos + 1) <= '9') {
            so = pmatch [*(pos + 1) - 48].rm_so;
            n = pmatch [*(pos + 1) - 48].rm_eo - so;
            if (so < 0 || strlen (rp) + n - 1 > size) return 1;
            memmove (pos + n, pos + 2, strlen (pos) - 1);
            memmove (pos, buf + so, n);
            pos = pos + n - 2;
        }
    sub = pmatch [1].rm_so; /* no repeated replace when sub >= 0 */
    for (pos = buf; !regexec (re, pos, 1, pmatch, 0); ) {
        n = pmatch [0].rm_eo - pmatch [0].rm_so;
        pos += pmatch [0].rm_so;
        if (strlen (buf) - n + strlen (rp) + 1 > size) return 1;
        memmove (pos + strlen (rp), pos + n, strlen (pos) - n + 1);
        memmove (pos, rp, strlen (rp));
        pos += strlen (rp);
        if (sub >= 0) break;
    }
    return 0;
}

static GString* portunhol(gchar **args)
{
    GString *msgret = NULL;
    regex_t regex;
    gchar accstr[FILENAME_MAX], rp[FILENAME_MAX];
    regexp *re = &regexps[0];
    strcpy(accstr, *args);
    while (re->re != NULL) {
        regcomp(&regex, re->re, REG_ICASE|REG_EXTENDED);
        rreplace(accstr, FILENAME_MAX, &regex, strcpy(rp, re->subtext));
        regfree(&regex);
        re++;
    }
    msgret = g_string_new("");
    g_string_append_printf(msgret, "%s", accstr);
    return msgret;
}

static PurpleCmdRet
cmd_func(PurpleConversation *conv, const gchar *cmd, gchar **args,
         gchar *error, void *data)
{
    GString *msgstr = NULL;
    msgstr = g_string_new("");

    if (*args != NULL) {
        GString *message = portunhol(args);
        g_string_append(msgstr, message->str);
        g_string_free(message, TRUE);
    }

    switch(purple_conversation_get_type(conv)) {
        case PURPLE_CONV_TYPE_IM:
            purple_conv_im_send(PURPLE_CONV_IM(conv), msgstr->str);
            break;
        case PURPLE_CONV_TYPE_CHAT:
            purple_conv_chat_send(PURPLE_CONV_CHAT(conv), msgstr->str);
            break;
        default:
            g_string_free(msgstr, TRUE);
            return PURPLE_CMD_RET_FAILED;
    }

    g_string_free(msgstr, TRUE);

    return PURPLE_CMD_RET_OK;
}

static gboolean
plugin_load(PurplePlugin *plugin)
{
    const gchar *portunholator_help;

    PurpleCmdFlag flags = PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT;

    portunholator_help = ("portunhol <mensagem>: envia a <mensagem> "
                          "traduzida para portunhol");

    port = purple_cmd_register("portunhol", "s", PURPLE_CMD_P_PLUGIN,
                               flags, NULL, PURPLE_CMD_FUNC(cmd_func),
                               portunholator_help, NULL);

    return TRUE;
}

static gboolean
plugin_unload(PurplePlugin *plugin)
{
    purple_cmd_unregister(port);

    return TRUE;
}

static PurplePluginInfo info =
{
     PURPLE_PLUGIN_MAGIC,
     PURPLE_MAJOR_VERSION,
     PURPLE_MINOR_VERSION,
     PURPLE_PLUGIN_STANDARD,
     NULL,
     0,
     NULL,
     PURPLE_PRIORITY_DEFAULT,
     PLUGIN_ID,
     "Portunholator",
     "1.0",
     "Hable en Portunhol muy facilmente!",
     "Registra el comando /portunhol para traduzir las mensagens "
     "para portunhol!",
     "Luiz Irber <luiz.irber@gmail.com>",
     "http://www.portunhol.art.br/wiki/P%C3%A1gina_principal",
     plugin_load,
     plugin_unload,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
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

PURPLE_INIT_PLUGIN(portunholator, init_plugin, info)
