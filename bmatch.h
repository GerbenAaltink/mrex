#ifndef BMATCH_H
#define BMATCH_H
#include <stdio.h>
#include <assert.h>
int bmatchhere(char *regexp, char *text);
int bmatchstar(int c, char *regexp, char *text);
int bmatch(char *regexp, char *text)
{
    if (regexp[0] == '^')
        return bmatchhere(regexp + 1, text) == 0 ? -1 : 0;
    int total_steps = 0;
    do
    {
        total_steps++;
        if (bmatchhere(regexp, text))
            return total_steps;
        
    } while (*text++ != '\0');
    return -1;
}
int bmatchhere(char *regexp, char *text)
{
    if (regexp[0] == '\0')
        return 1;
    if (regexp[1] == '*')
        return bmatchstar(regexp[0], regexp + 2, text);
    if (regexp[0] == '$' && regexp[1] == '\0')
        return *text == '\0';
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
        return bmatchhere(regexp + 1, text + 1);
    return 0;
}
int bmatchstar(int c, char *regexp, char *text)
{
    do
    {
        if (bmatchhere(regexp, text))
            return 1;
    } while (*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}
#endif