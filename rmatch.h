#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
int rmatchhere(char *regexp, char *text);
int rmatchgreedy(int c, char *regexp, char *text, int required);
int *rmatch(char *regexp, char **txt)
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
            // printf("%d : %d %d\n", steps_total, steps_false, result[2]);
            *txt += result[1] + result[2];
            return result;
        }
        else
        {
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
        if ((res = rmatchgreedy(regexp[0], regexp + 2, text, 0)))
            return res - 1;
    if (regexp[1] == '+')
        if ((res = rmatchgreedy(regexp[0], regexp + 2, text, 1)))
            return res - 1;
    if (regexp[0] == '[')
    {
        regexp++;
        int block_true = 0;
        while (*regexp != ']')
        {
            if (!block_true && *regexp == *text)
                block_true = 1;
            regexp++;
        }
        if (block_true)
            return rmatchhere(regexp + 1, text + 1) + 1;
        return 0;
    }
    if (regexp[0] == '$' && regexp[1] == '\0')
        return *text == '\0';
    if (regexp[0] == '\\')
        return *text == '.' && rmatchhere(regexp + 2, text + 1) + 1;
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
        if ((res = rmatchhere(regexp + 1, text + 1)))
            return res + 1;
    return 0;
}
int rmatchgreedy(int c, char *regexp, char *text, int required)
{
    int rmatch_success = -1;
    int res = 0;
    do
    {
        rmatch_success++;
        if ((res = rmatchhere(regexp, text)))
        {
            res = rmatch_success + res;
            if (!required)
            {
                return res + 1;
            }
            else if (required && res > 1)
            {
                return res + 1;
            }
        }
    } while (*text != '\0' && ((*text++ == c) || c == '.'));
    return 0;
}
char *rmatch_extract(char *regexp, char **txt)
{
    int *result = rmatch(regexp, txt);
    if (result[0])
    {
        char *extracted = *txt - result[2];
        char *str = (char *)malloc(result[2] + 1);
        str[0] = 0;
        strncpy(str, extracted, result[2]);
        str[result[2]] = 0;
        return str;
    }
    return NULL;
}

void rmatch_test(char *expr, char *text, char **expected)
{
    char *res;
    printf("test: <%s> \"%s\"\n", expr, text);
    int expected_index = 0;
    while (*text && expected[expected_index] != NULL)
    {
        char *text_original = text;
        res = rmatch_extract(expr, &text);
        if ((!res && !(res == expected[expected_index])) || strncmp(res, expected[expected_index], strlen(expected[expected_index])))
        {
            printf("\e[31merror:\n");
            printf(" expected: \"%s\"\n", expected[expected_index]);
            printf(" got: \"%s\"\e[0m\n", res ? res : "[no match]");
            assert(0);
        }
        else
        {
            printf("\e[32mcorrect:\e[0m <%s> \"%s\"\n", expr, text_original);
        }
        expected_index++;
    }
    if (*text && expected[expected_index] != NULL)
    {
        printf("\e[31merror:\n");
        printf(" expected: %s\n", "empty string");
        printf(" got: \"%s\"\e[0m\n", text);
        assert(!*text);
    }
    assert(!expected[expected_index]);
}

void example_one()
{
    char *str = "testtest";
    char *expr = "t.*t";
    // Two times it equals to "test"
    assert(!strcmp(rmatch_extract(expr, &str), "test"));
    assert(!strcmp(rmatch_extract(expr, &str), "test"));
    // Third time it equals to NULL
    assert(!rmatch_extract(expr, &str));
}

void example_two()
{
    char *str = "testtest";
    char *expr = "t.*t";
    char *result = NULL;
    // Loops until NULL
    while ((result = rmatch_extract(expr, &str)))
    {
        printf("Found: \"%s\"\n", result);
    }
    // Be sure last result is invalid
    assert(!result);
}

void rmatch_tests()
{
    // Examples
    printf("Testing example one.\n");
    example_one();
    printf("Testing example two.\n");
    example_two();
    printf("Testing extracting.\n");
    // Star
    printf("Testing star.\n");
    rmatch_test(".*H.*ry P.*rS.*la", "Harry PotterSim       SalaHarry PotterSimSalaHarry PotterSimSalaHarry PotterSimSala",
                (char *[]){
                    "Harry PotterSim       Sala",
                    "Harry PotterSimSala",
                    "Harry PotterSimSala",
                    "Harry PotterSimSala",
                    NULL});
    char *text_fox = "The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.";
    rmatch_test("The q.*ick b.*n f.*x j.*s over the lazy dog.$", text_fox, (char *[]){"The quick brown fox jumps over the lazy dog.", NULL});
    char *text_fox2 = "The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.";
    rmatch_test("T.*e q.*k b.*n f.*x j.*s o.*r t.*e l.*y d.*g.", text_fox2, (char *[]){"The quick brown fox jumps over the lazy dog.", "The quick brown fox jumps over the lazy dog.", "The quick brown fox jumps over the lazy dog.", "The quick brown fox jumps over the lazy dog.", "The quick brown fox jumps over the lazy dog.", NULL});
    // Block
    printf("Testing block.\n");
    char *text_fox3 = "The quick brown fox jumps over the lazy dog.";
    rmatch_test("T.*e q.*k b.*n f.*x j.*s o.*r t.*e l[oa][az].*y d[ao]g.", text_fox3, (char *[]){"The quick brown fox jumps over the lazy dog.", NULL});
    /// Plus
    printf("Testing plus.\n");
    char *text_fox4 = "The quick brown fooox jumps over the lazy dog.";
    rmatch_test("T.*e q.*k b.*n fo+x j.*s o.*r t.*e l[oa][az].*y d[ao]g.", text_fox4, (char *[]){"The quick brown fooox jumps over the lazy dog.", NULL});
    rmatch_test("T.*e q.*k b.*n f.+x j.*s o.*r t.*e l[oa][az].*y d[ao]g.", text_fox4, (char *[]){"The quick brown fooox jumps over the lazy dog.", NULL});
    rmatch_test("T.*e q.*k b.*n f+.*x j.*s o.*r t.*e l[oa][az].*y d[ao]g.", text_fox4, (char *[]){"The quick brown fooox jumps over the lazy dog.", NULL});
    rmatch_test("T.*e q.*k b.*n fo+x j.*s o.*r t.*e l[oa][az].*y d[ao]g.", text_fox4, (char *[]){"The quick brown fooox jumps over the lazy dog.", NULL});
    rmatch_test("T.*e q.*k b.*n f+x j.*s o.*r t.*e l[oa][az].*y d[ao]g.", text_fox4, (char *[]){NULL});
}