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
 *
 * A rigor, esse programa é apenas uma tradução para C do script Python
 * disponível em http://www.portunhol.art.br/wiki/Python e escrito por Roberto
 * de Almeida, mas como o Pidgin ainda não tem loader para plugins em Python
 * tive que reescrever em C.
 *
 * Com a adição do engripator e do miguxeitor, cabe dizer que são adaptações
 * dos sites
 *     http://www.aurelio.net/web/engripeitor.html e
 *     http://www.aurelio.net/web/miguxeitor.html
 * já que eu só peguei as ER definidas no código fonte e adaptei (inclusive
 * mantive os comentários, para me achar caso hajam atualizações). Todos os
 * créditos, portanto, para Aurélio Jargas pelo trabalho pesado =D
 * (Comprem o livro de expressões regulares dele, é muito bom!)
 */

#include <glib.h>
#include <glib/gprintf.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>

#include "data.h"

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

int main(int *argc, char** argv) {
  const regexp *re = &portunhol[0];
  g_printf("portunhol: %s\n", tradutor(argv, re, REG_ICASE)->str);
  re = &gripado[0];
  g_printf("gripado: %s\n", tradutor(argv, re, 0)->str);
  re = &miguxo[0];
  g_printf("miguxo: %s\n", camelize(tradutor(argv, re, REG_ICASE))->str);
  re = &alemon[0];
  g_printf("alemon: %s\n", tradutor(argv, re, REG_ICASE)->str);
  return 0;
}

