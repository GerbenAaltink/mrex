#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int rmatchhere(char *regexp, char *text);
int rmatchstar(int c, char *regexp, char *text);
int * rmatch(char *regexp, char **txt)
{
    char *text = *txt;
    static int result[3];
    memset(result, 0, sizeof(result));
    if (!*text)
        return result;
    if (regexp[0] == '^')
    {
        if ((result[2] = rmatchhere(regexp + 1, text)))
        {
            result[2]--;
            result[1] = 0;
            result[0] = result[2] > 0;
            *txt += result[2];
        }
        return result;
    }
    int steps_total = -1;
    int steps_false = 0;
    do
    {
        steps_total++;
        if ((result[2] = rmatchhere(regexp, text)))
        {
            result[2]--;
            result[0] = result[2] > 0;
            result[1] = steps_total;
            printf("%d : %d %d\n", steps_total, steps_false, result[2]);
            *txt += result[1] + result[2];
            return result;
        }else{
            steps_false++;
        }
    } while (*text++ != '\0');
    return result; 
}
int rmatchhere(char *regexp, char *text)
{
    int res = 0;
    if (regexp[0] == '\0')
        return 1;
    if (regexp[1] == '*')
        if((res  = rmatchstar(regexp[0], regexp + 2, text)))
            return res - 1;
    if (regexp[0] == '$' && regexp[1] == '\0')
        return *text == '\0';
    if (regexp[0] == '\\')
          return *text == '.' && rmatchhere(regexp + 2, text + 1) + 1;
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
        if ((res = rmatchhere(regexp + 1, text + 1)))
            return res + 1;
    return 0;
}
int rmatchstar(int c, char *regexp, char *text)
{
    int rmatch_success = -1;
    int res = 0;
    do
    {
        rmatch_success++;
        if (res = rmatchhere(regexp, text))
            return rmatch_success + res + 1 ;
    } while (*text != '\0' && ((*text++ == c) || c == '.'));
    return 0;
}
char * rextract(char *regexp, char **txt){
    int * result = rmatch(regexp, txt);
    if(result[0] && result[2]){
        char * extracted = *txt - result[2];
        char * str = (char *)malloc(result[2] + 1);
        str[0] = 0;
        strncpy(str,extracted,result[2]);
        
        str[result[2]] = 0;
        return str;
        
    }else{
        printf("%d : %d : %d\n", result[0],result[1],result[2]);
    }
    return NULL;
}
void retoor()
{
    char *text = "The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.d";
    char *regexp = "^.*The.*fuck.*lazy.*dog.";
    int *result = rmatch(regexp, &text);
    while (result[0])
    {
        printf("%d %d %d <%s>\n", result[0], result[1], result[2], text);
        result = rmatch(regexp, &text);
    }
}