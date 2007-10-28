/*
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

/* Este é apenas um programa teste, mais fácil debugar aqui do que dentro do
 * Pidgin. Possivelmente deixará de ser atualizado com o tempo, mas é bom para
 * aprender como usar expressões regulares em C, e principalmente pela função
 * rreplace, que inexplicavelmente não é fornecida na biblioteca padrão de C e
 * no padrão POSIX...
 * A rigor, esse programa é apenas uma tradução para C do script Python
 * disponível em http://www.portunhol.art.br/wiki/Python , mas como o Pidgin
 * ainda não tem loader para plugins em Python tive que reescrever em C.
 */

#include <glib.h>
#include <glib/gprintf.h>
#include <regex.h>
#include <string.h>

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



int rreplace (char *buf, int size, regex_t *re, char *rp)
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

static GString* tradutor(gchar **args, const regexp *re)
{
    GString *msgret = NULL;
    char rp[FILENAME_MAX];
    gchar accstr[FILENAME_MAX];
    strcpy(accstr, g_strjoinv(" ", ++args));
    regex_t regex;
    while (re->re != NULL) {
        regcomp(&regex, re->re, REG_ICASE|REG_EXTENDED);
        rreplace(accstr, FILENAME_MAX, &regex, strcpy(rp, re->subtext));
        regfree (&regex);
        re++;
    }
    msgret = g_string_new("");
    g_string_append_printf(msgret, "%s", accstr);
    return msgret;
}

int main(int *argc, char** argv) {
  const regexp *re = &portunhol[0];
  g_printf("portunhol: %s\n", tradutor(argv, re)->str);
  re = &gripado[0];
  g_printf("gripado: %s\n", tradutor(argv, re)->str);
  return 0;
}

