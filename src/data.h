#ifndef DATA_H
#define DATA_H

typedef struct {
    const char* re;
    const char* subtext;
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

static const regexp alemon[] = {
    {"b", "p"},
    {"v", "f"},
    {"g", "c"},
    {"t", "d"},
    {"ch", "j"},
    {"s\\b", ""},
    {NULL, NULL}
};

#endif /* DATA_H */
