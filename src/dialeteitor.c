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
 * Planos futuros incluem o Caipirator (sugestão do Frank).
 */


#include <regex.h>
#include <string.h>

#define PURPLE_PLUGINS
#define PLUGIN_ID "core-dialeteitor"

#include "cmds.h"
#include "conversation.h"
#include "plugin.h"
#include "util.h"
#include "version.h"

PurplePlugin *plugin_handle = NULL;

static PurpleCmdId port_cmd;
static PurpleCmdId gripado_cmd;
static PurpleCmdId miguxo_cmd;

static int port_cmd_id    = 1;
static int gripado_cmd_id = 1 << 1;
static int miguxo_cmd_id  = 1 << 2;

typedef struct {
    const gchar* re;
    const gchar* subtext;
} regexp;

static const regexp portunhol[] = {
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

static const regexp gripado[] = {
    {"m([aeiouáéíóúâêôãõAEIOUÁÉÍÓÚÂÊÔÃÕ])", "b\\1"},
    {"M([aeiouáéíóúâêôãõAEIOUÁÉÍÓÚÂÊÔÃÕ])", "B\\1"},
    {"n([aeiouáéíóúâêôãõAEIOUÁÉÍÓÚÂÊÔÃÕ])", "d\\1"},
    {"N([aeiouáéíóúâêôãõAEIOUÁÉÍÓÚÂÊÔÃÕ])", "D\\1"},
    {"m([pbPB])", "n\\1"},
    {"M([pbPB])", "N\\1"},
    {"v", "b"},
    {"V", "B"},
    {"p", "b"},
    {"P", "B"},
    {"t", "d"},
    {"T", "D"},
    {"[sç]", "z"},
    {"[SÇ]", "Z"},
    {"qu", "gu"},
    {"QU", "GU"},
    {"ch", "j"},
    {"lh", "dj"},
    {"\\.\\.\\.", "... A-A-A-Atchô!"},
    {NULL, NULL}
};

/* miguxo level 3! */
static const regexp miguxo[] = {
    // Numeros
    {"\\buma?\\b", "1"},
    {"\\b(dois|duas)\\b", "2"},
    {"\\btrês\\b", "3"},
    {"\\bquatro\\b", "3"},
    {"\\bcinco\\b", "5"},
    {"\\bseis\\b", "6"},
    {"\\bsete\\b", "7"},
    {"\\boito\\b", "8"},
    {"\\bnove\\b", "9"},
    {"\\bdez\\b", "10"},
    {"\\bonze\\b", "11"},
    {"\\bdoze\\b", "12"},
    {"\\btreze\\b", "13"},
    {"\\b(c|qu)atorze\\b", "14"},
    {"\\bquinze\\b", "15"},
    {"\\bdezesseis\\b", "16"},
    {"\\bdezessete\\b", "17"},
    {"\\bdezoito\\b", "18"},
    {"\\bdezenove\\b", "19"},

    // Repeticoes
    {"\\b([0-9]+) vez(es)?\\b", "\\1x"},

    // Horas, minutos
    {"\\b(\\d+) horas?\\b", "\\1h"},
    {"\\b(\\d+) minutos?\\b", "\\1min"},

    // Dias da semana
    {"\\bsegunda-feira\\b", "2a"},
    {"\\bterça-feira\\b", "3a"},
    {"\\bquarta-feira\\b", "4a"},
    {"\\bquinta-feira\\b", "5a"},
    {"\\bsexta-feira\\b", "6a"},

    // Símbolos
    {"\\baté mais\\b", "t+"},
    {"\\bdemais\\b", "d+"},
    {"\\bmais ou menos\\b", "+-"},
    {"\\bmais\\b", "+"},
    {"\\bmenos\\b", "-"},
    {"\\bmei[oa]\\b", "1/2"},

    // Abreviações simples
    {"\\bpor\\s?qu[eê]", "pq"},
    {"\\bhoje\\b", "hj"},
    {"\\btambém\\b", "tb"},
    {"\\bbeleza\\b", "blz"},
    {"\\bfirmeza\\b", "fmz"},
    {"\\bquando\\b", "qdo"},
    {"\\bquant([ao])(s?)\\b", "qu\\1\\2"},
    {"\\bmuit([ao])(s?)\\b", "mt\\1\\2"},
    {"\\bbeij(o|ão)\\b", "bj\\1\\2"},
    {"\\bbeijos\\b", "bjs"},

    // Abreviações avançadas
    {"\\bmesm[ao](s?)\\b", "msm\\1"},
    {"\\bdepois\\b", "dpois"},
    {"\\bquem\\b", "qm"},
    {"\\bcomigo\\b", "cmg"},
    {"\\bcadê\\b", "kd"},
    {"\\bqualuqer\\b", "qq"},
    {"\\bfalow\\b", "flw"},
    {"\\bvaleu\\b", "vlw"},
    {"\\btchau\\b", "xau"},

    {"\\bque\\b", "ki"},
    {"\\b(adoro você|te adoro)", "adoluxê"},
    {"\\bamo vocês\\b", "amodolu vocês"},
    {"\\b(amo você|te amo)", "te amodolu"},
    {"\\bvocê", "vuxê"},

    // Glossário
    {"\\btecl(e|ar|ou|amos)\\b", "tc"},
    {"\\binternet\\b", "net"},
    {"\\be-?mail(s?)\\b", "meio$1"},
    {"\\b(grana|dinheiro)\\b", "$$$$$$"},

    {"\\badicion[\\wáí]+", "add"},
    {"\\bamig([ao]s?)\\b", "migux\\1"},
    {"\\blind([ao]s?)\\b", "lindux\\1"},
    {"\\bfof([ao]s?)\\b", "fofux\\1"},
    {"\\bdormir\\b", "mimir"},
    {"\\bnome(s?)\\b", "nominho\\1"},
    {"\\besposa\\b", "marida"},
    {"\\b(de novo|novamente)\\b", "dinovo"},

    // tou, tava, tar
    {"\\best(ar|ou|ava|ive|aria|ão)\\b", "t\\1"},
    {"\\bestá([^\\wáéíóúàâêôãõüç]|$)", "tah\\1"},

    // para
    {"\\bpara ([ao]s?)\\b", "pr\\1"},
    {"\\bpara([^\\wáéíóúàâêôãõüç]|$)", "pra\\1"},

    // Simplifiq: irmos -> ir, acabou -> cabou
    {"([aei]r)mos\\b", "\\1"},
    {"\\bacab", "cab"},

    // entaum, naum
    {"ão\\b", "aum"},

    //andando -> andano, comendo -> comeno
    {"(\\w[aei])ndo\\b", "\\1no"},

    // tada$ -> tadeenha, foto -> foteenha, gata -> gateenha
    {"(\\w[crt]ad)([ao])\\b", "\\1eenh\\2"},
    {"foto(s?)\\b", "foteenha\\1"},
    {"(\\w)tinha([ao])\\b", "\\1teenh\\2"},

    //No orkut é mais fófi terminar em i
    {"\\bse\\b", "si"},
    {"\\bde\b", "di"},
    {"\\bte\b", "ti"},

    // CH, SH e QU não existem
    {"ch", "x"},
    {"sh", "x"},
    {"qh", "k"},

    // e -> i (alguns casos)
    {"(\\w(ss|[cdgtv]))e(s?)m?\\b", "\\1i\\3"},
    {"\\bseg", "sig"},
    {"\\bdes([^s])", "dis\\1"},

    // ei -> i (alguns casos) deixa -> dexa
    {"eix", "ex"},

    // o -> u (alguns casos)
    {"\\bbonit", "bunit"},

    // e sozinho -> i
    {"\\be\\b", "i"},

    // inglês -> ingleix
    {"ês\\b", "eix"},

    // atrás -> atraix
    {"(\\w)(ás|az)\\b", "\\1aix"},

    // Acento no final eh moh uoh
    {"á([^\\wáéíóúàâêôãõüç]|$)", "ah\\1"},
    {"é([^\\wáéíóúàâêôãõüç]|$)", "eh\\1"},
    {"í([^\\wáéíóúàâêôãõüç]|$)", "ih\\1"},
    {"ó([^\\wáéíóúàâêôãõüç]|$)", "oh\\1"},
    {"ú([^\\wáéíóúàâêôãõüç]|$)", "uh\\1"},

    // Acentuação? Nunca.
    {"[áàâãä]", "a"},
    {"[éèêë]", "e"},
    {"[íìîï]", "i"},
    {"[óòôõö]", "o"},
    {"[úùûü]", "u"},
    {"[ç]", "ss"},

    // l$ -> u
    {"(\\w[a-z])l\\b", "\\1u"},

    // amo -> amu, todo -> todu (plural também)
    {"o(s?)\\b", "u\\1"},
    {"\\b(\\d+)u\\b", "\\1o"}, // fix 1u > 1o (primeiro)

    // ou -> o (se for parte de palavra)
    {"(\\w)ou\\b", "\\1ow"},
    {"\\bou(\\w)", "o\\1"},
    {"(\\w)ou(\\w)", "\\1o\\2"},

    // ^c -> k (exceções: certo, cidade, c)
    {"\\bc([^ei\\W])", "k\\1"},

    // andar -> andah, comer -> come, sentir -> senti
    {"ar\\b", "ah"},
    {"er\\b", "e"},
    {"ir\\b", "i"},

    // eira$ -> era (sonzera, ladera)
    {"eirar\\b", "era"},

    // sa$ -> za, casa -> caza
    {"([^s\\W])sa\\b", "\\1za"},

    // O abominável X no fim das palavras no plural
    {"([^\\Ws]s\\b)", "\\1x"},

    // O abominável H no fim de certas palavras
    {"(\\w)a\\b)", "\\1ah"},

    // Risada
    {"\\b(he|ha|hi|ho|hua|rs){2,}h?\\b", "kkkkkkkkkkk"},
    {"[8:][-o]?[(D)]", "kkkkkkkkkkk"},

    // Somente um ponto final é muito pouco
    {"\\.", "......"},

    // Pra que vírgula? Pontos são mais legais... vários...
    {", ", "..."},
    {",(\\n|$)", "...\\1"},

    // Sejamos enfáticos!!!
    {"!", "!!!!!"},
    {"\\?", "?!?!"},

    // E uns ajustes finais pra ficar ainda mais fofuxu
    {"x", "xXx"},
    {"ss", "XX"},

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

static GString* camelize(GString* input)
{
    GString* retstr = g_string_new("");
    input = g_string_ascii_down(input);
    int pos = 0;
    srand(time(NULL));
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
    gchar rp[24]; /* TODO: Esse tamanho eh aceitavel? */
    gchar *accstr = NULL;
    regex_t regex;

    accstr = g_strjoinv(" ", ++args);
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
        switch ((int)(*data)) {
            case port_cmd_id:
                message = tradutor(args, &portunhol[0], REG_ICASE);
                break;
            case gripado_cmd_id:
                message = tradutor(args, &gripado[0], 0);
                break;
            case miguxo_cmd_id:
                message = tradutor(args, &miguxo[0], REG_ICASE);
                break;
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
                               portunhol_help, &port_cmd_id);

    gripado_cmd = purple_cmd_register("gripado", "s", PURPLE_CMD_P_PLUGIN,
                               flags, NULL, PURPLE_CMD_FUNC(cmd_func),
                               gripe_help, &gripado_cmd_id);

    miguxo_cmd = purple_cmd_register("miguxo", "s", PURPLE_CMD_P_PLUGIN,
                               flags, NULL, PURPLE_CMD_FUNC(cmd_func),
                               miguxo_help, &miguxo_cmd_id);
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
