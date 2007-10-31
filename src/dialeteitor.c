/*
 * Dialeteitor
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

/* Plugin para você poder falar em certos dialetos do português.
 *
 * A rigor, esse plugin é apenas uma adaptação de scripts disponíveis na
 * internet. Estava querendo aprender plugins pro Pidgin, e já aproveitei
 * pra entender melhor ERs também.
 *
 * Começou com o Portunholator, script Python disponível em
 * http://www.portunhol.art.br/wiki/Python e escrito por Roberto
 * de Almeida, mas como o Pidgin ainda não tem loader para plugins em Python
 * tive que reescrever em C.
 *
 * Com a adição do engripeitor e do miguxeitor, cabe dizer que são adaptações
 * dos sites
 *     http://www.aurelio.net/web/engripeitor.html e
 *     http://www.aurelio.net/web/miguxeitor.html
 * já que eu só peguei as ER definidas no código fonte (em javascript) e
 * adaptei (inclusive mantive os comentários, para me achar caso hajam
 * atualizações lá). Todos os créditos, portanto, para Aurélio Jargas pelo
 * trabalho pesado =D
 * (Comprem o livro de expressões regulares dele, é muito bom!)
 *
 * Planos futuros incluem o Caipireitor (sugestão do Frank).
 */

#include <stdlib.h>
#include <regex.h>
#include <string.h>

#define PURPLE_PLUGINS
#define PLUGIN_ID "core-dialeteitor"

#include "cmds.h"
#include "conversation.h"
#include "plugin.h"
#include "util.h"
#include "version.h"

#include "data.h"

PurplePlugin *plugin_handle = NULL;

static PurpleCmdId port_cmd;
static PurpleCmdId gripado_cmd;
static PurpleCmdId miguxo_cmd;

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

static GString* camelize(GString* input)
{
    GString* retstr = g_string_new("");
    int pos = 0;
    srand(time(NULL));
    input = g_string_ascii_down(input);
    for (pos = 0; pos <= input->len; pos++) {
        if (input->str[pos] <= 122 && input->str[pos] >= 97) {
            if (rand()%2) // capitaliza
                g_string_append_c(retstr, input->str[pos] - 32);
            else // deixa minuscula
                g_string_append_c(retstr, input->str[pos]);
        }
        else
            g_string_append_c(retstr, input->str[pos]);
    }
    return retstr;
}

static GString* tradutor(gchar **args, const regexp *re, int flags)
{
    GString *msgret = NULL;
    gchar rp[24]; /* FIXME: Esse tamanho eh aceitavel? */
    gchar *accstr = NULL;
    regex_t regex;

    accstr = g_strjoinv(" ", args);
    while (re->re != NULL) {
        regcomp(&regex, re->re, flags|REG_EXTENDED);
        rreplace(accstr, strlen(accstr) + 1, &regex, strcpy(rp, re->subtext));
        regfree (&regex);
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
    GString *message = NULL;
    msgstr = g_string_new("");

    if (*args != NULL) {
        if (!strcmp(data, "portunhol")) {
            message = tradutor(args, &portunhol[0], REG_ICASE);
        }
        else if (!strcmp(data, "gripado")) {
            message = tradutor(args, &gripado[0], 0);
        }
        else if (!strcmp(data, "miguxo")) {
            message = camelize(tradutor(args, &miguxo[0], REG_ICASE));
        }
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
    const gchar *portunhol_help;
    const gchar *gripado_help;
    const gchar *miguxo_help;

    PurpleCmdFlag flags = PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT;

    portunhol_help = ("portunhol <mensagem>: envia a <mensagem> "
                      "traduzida para portunhol.\nCréditos para "
                      "Roberto de Almeida (http://dealmeida.net)");

    gripado_help = ("gripado <mensagem>: envia a <mensagem> "
                    "traduzida para engripado\nCréditos para "
                    "Aurélio Jargas (http://aurelio.net)");

    miguxo_help = ("miguxo <mensagem>: envia a <mensagem> "
                   "traduzida para miguxês\nCréditos para "
                   "Aurélio Jargas (http://aurelio.net)");

    port_cmd = purple_cmd_register("portunhol", "s", PURPLE_CMD_P_PLUGIN,
                               flags, NULL, PURPLE_CMD_FUNC(cmd_func),
                               portunhol_help, "portunhol");

    gripado_cmd = purple_cmd_register("gripado", "s", PURPLE_CMD_P_PLUGIN,
                               flags, NULL, PURPLE_CMD_FUNC(cmd_func),
                               gripado_help, "gripado");

    miguxo_cmd = purple_cmd_register("miguxo", "s", PURPLE_CMD_P_PLUGIN,
                               flags, NULL, PURPLE_CMD_FUNC(cmd_func),
                               miguxo_help, "miguxo");
    return TRUE;
}

static gboolean
plugin_unload(PurplePlugin *plugin)
{
    purple_cmd_unregister(port_cmd);
    purple_cmd_unregister(gripado_cmd);
    purple_cmd_unregister(miguxo_cmd);

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
     "Dialeteitor",
     "1.0",
     "Tradutor para dialetos diversos",
     "Registra os comandos /portunhol, /gripado e /miguxo para traduzir "
     "sua mensagem. Para mais informações, digite /help <nome-do-comando> .",
     "Luiz Irber <luiz.irber@gmail.com>",
     "http://www.comp.ufscar.br/~luizcarlos/projects/dialeteitor/",
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

PURPLE_INIT_PLUGIN(dialeteitor, init_plugin, info)
