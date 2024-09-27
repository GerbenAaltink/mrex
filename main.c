#include <stdio.h>
#include "mrex.h"
int matchhere(char *regexp, char *text);
int matchstar(int c, char *regexp, char *text);
int match(char *regexp, char *text)
{
    if (regexp[0] == '^')
        return matchhere(regexp + 1, text);
    do
    {
        if (matchhere(regexp, text))
            return 1;
    } while (*text++ != '\0');
    return 0;
}
int matchhere(char *regexp, char *text)
{
    if (regexp[0] == '\0')
        return 1;
    if (regexp[1] == '*')
        return matchstar(regexp[0], regexp + 2, text);
    if (regexp[0] == '$' && regexp[1] == '\0')
        return *text == '\0';
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
        return matchhere(regexp + 1, text + 1);
    return 0;
}
int matchstar(int c, char *regexp, char *text)
{
    do
    {
        if (matchhere(regexp, text))
            return 1;
    } while (*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}

rtest(char *expr, char *text){
    char * res;
    printf("<%s> \"%s\"\n",expr,text);
    while(1){
     res = rextract(expr,&text);
        if(!res)
            break;
    printf("Extracted: %s\n",res);
    }
}
int main()
{
    rtest(".*H.*ry P.*rS.*la", "Harry PotterSim       SalaHarry PotterSimSalaHarry PotterSimSalaHarry PotterSimSala");
    rtest(".*H.*ry P.*rS.*la", "Harry PotterSimSalaHarry PotterSimSalaHarry PotterSimSalaHarry");

    rtest(".*H.*ry P.*rS.*la", "Harry PotterSimSalaHarry PotterSimSala Potter");

    exit(0);
    char *text = "The quick brown foxxx jumps over the lazy dog.";
    char *text_original = text;
    char *regexp = "fox* jumps ov";
    int result = match(regexp, text);
    printf("%d", result);
    int *rresult = rmatch(regexp, &text);
    printf("%d:%d:%d", rresult[0], rresult[1], rresult[2]);
    text = text_original;
    char *sresult = rextract(regexp, &text);
    printf("%s", sresult);
    *sresult = rextract(regexp, &text);
    printf("%s", sresult);
}