#ifndef RMATCH_H
#define RMATCH_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

typedef struct rmatch_t {
        unsigned int valid;
        unsigned int start;
        unsigned int length;
        unsigned int position;
        
        unsigned int binary;
        long size;
    } rmatch_t;

char * rmatch_compile(char *regexp){
    char *optimized = (char *)malloc(strlen(regexp) + 1);
    char *optimized_ptr = optimized;
    char prev = 0;
    int in_range = 1;
    while(*regexp){
        if(*regexp == '[' && *(regexp + 2) == ']'){
            regexp++;
            *optimized_ptr++ = *regexp++;
            regexp++;
            continue;
        }

        if(*regexp == '[' && prev != '\\'){
            in_range = 1;
            *optimized_ptr = *regexp;
            regexp++;
            optimized_ptr++;
            continue;
        }
        if(*regexp == ']' && prev != '\\'){
            in_range = 0;
        }
        prev = *optimized_ptr;
        
        if(in_range && *(regexp) == '0' && *(regexp + 1) == '-' && *(regexp + 2) == '9'){
            regexp++;
            regexp++;
            *optimized_ptr++ = '\\';
            *optimized_ptr++ = 'd';
            regexp++;
            continue;
        } else if(in_range && *(regexp) == 'a' && *(regexp + 1) == '-' && *(regexp + 2) == 'z'){
            regexp++;
            regexp++;
            *optimized_ptr++ = '\\';
            *optimized_ptr++ = 'l';
            regexp++;
            continue;
        } else if(in_range && *(regexp) == 'A' && *(regexp + 1) == '-' && *(regexp + 2) == 'Z'){
            regexp++;
            regexp++;
            *optimized_ptr++ = '\\';
            *optimized_ptr++ = 'u';
            regexp++;
            continue;
        }
        
        *optimized_ptr = *regexp;
        optimized_ptr++;
        regexp++;
    }
    *optimized_ptr = '\0';
    return optimized;
}
char rmatch_optimize = 1;

int rmatchhere(rmatch_t * rm,unsigned char *regexp, unsigned char *text);
inline int rmatchgreedy(rmatch_t * rm, int c, unsigned char *regexp, unsigned char *text, int required);
rmatch_t *rmatch(char *regexp, char **txt)
{
    char * compiled = rmatch_optimize ? rmatch_compile(regexp) : NULL;
    if(compiled)
        regexp = compiled;

    static rmatch_t rm;
    rm.start = 0;
    rm.length = 0;
    rm.valid = 0;
    rm.binary = 0;
    rm.position = 0;
    rm.size = 0;
    unsigned char *text = (unsigned char*)*txt;
    static unsigned int result[3];
    //result[0] = 0;
    memset(result, 0, sizeof(result));
    if (!*text)
        return &rm;
    if (regexp[0] == '^')
    {
        if ((result[2] = rmatchhere(&rm, (unsigned char *)(regexp + 1), text)))
        {
            result[2]--;
            result[1] = 0;
            result[0] = result[2] > 0;
            *txt += result[2];
            rm.start = 0;
            rm.valid = result[0] ? 1 : 0;
            rm.position = result[0] ? result[1] + result[2] : 0;
            rm.length = result[2];
        }
        if(compiled)
            free(compiled);
        return &rm;
    }
    unsigned int steps_total = -1;
    do
    {
        steps_total++;
        if (!(result[2] = rmatchhere(&rm, (unsigned char *)regexp, text)))
            continue;
        result[2]--;
        result[0] = result[2] > 0;
        result[1] = steps_total;
        *txt += result[1] + result[2];
        rm.valid = result[0] ? 1 : 0;
        rm.position = result[0] ? result[1] + result[2] : 0;
        rm.length = result[0] ? result[2] : 0;
        rm.start = result[0] ? result[1] : 0;
        if(compiled)
            free(compiled);
        return &rm;
    
    } while (*text++ != '\0');
        if(compiled)
        free(compiled);
    return &rm;
}

 
rmatch_t * rmatchb(unsigned char *regexp, unsigned char **txt, long size)
{
   
    static rmatch_t rm;
    rm.start = 0;
    rm.length = 0;
    rm.valid = 0;
    rm.binary = 0;
    rm.position = 0;
    rm.size = size;
    unsigned char *text = *txt;
    static unsigned int result[3];
    //result[0] = 0;
    memset(result, 0, sizeof(result));
    if (!*text)
        return &rm;
    if (regexp[0] == '^')
    {
        if ((result[2] = rmatchhere(&rm,regexp + 1, text)))
        {
            result[2]--;
            result[1] = 0;
            result[0] = result[2] > 0;
            *txt += result[2];
            rm.valid = result[0] ? 1 : 0;
        }
        return &rm;
    }
    unsigned int steps_total = -1;
    do
    {
        steps_total++;
        if (!(result[2] = rmatchhere(&rm,regexp, text)))
            continue;
        result[2]--;
        result[0] = result[2] > 0;
        result[1] = steps_total;
        *txt += result[1] + result[2];
        return &rm;
    
    } while (*text++ || rm.position != rm.size);
    return &rm;
}
int rmatchhere(rmatch_t * rm,unsigned char *regexp, unsigned char *text)
{
    
    unsigned int res = 0;
    rm->position += 1;

    if(rm->binary && rm->position == rm->size - 1)
    {
        return 1;
    }
    if (regexp[0] == '\0')
        return 1;
    if (regexp[0] == '\\' && regexp[1] == 'd'){
        if(isdigit(*text))
            return rmatchhere(rm, regexp + 2, text + 1) + 1 ;
        else 
            return 0;
    }
    if (regexp[0] == '\\' && regexp[1] == 'w'){
        if(isalpha(*text))
            return rmatchhere(rm, regexp + 2, text + 1) + 1;
        else 
            return 0;
    }
    if (regexp[0] == '\\' && regexp[1] == 'l'){
        if(isalpha(*text) && islower(*text))
            return rmatchhere(rm, regexp + 2, text + 1) + 1;
        else 
            return 0;
    }
    if (regexp[0] == '\\' && regexp[1] == 'u'){
        if(isalpha(*text) && !islower(*text))
            return rmatchhere(rm, regexp + 2, text + 1) + 1;
        else 
            return 0;
    }
    if (regexp[1] == '*')
        if ((res = rmatchgreedy(rm, regexp[0], regexp + 2, text, 0)))
        {
            rm->position = res - 1;
            return res - 1;
        }
    if (regexp[1] == '+')
        if ((res = rmatchgreedy(rm, regexp[0], regexp + 2, text, 1)))
            return res - 1;
    if (regexp[0] == '[')
    {
        regexp++;
        int block_true = 0;
        do 
        {
            if (!block_true && *regexp == *text)
                block_true++;
            regexp++;
        } while((*regexp != ']'));
        if (block_true)
            if((res = rmatchhere(rm, regexp +1 , text + 1))){
                rm->position = res + 1;
                return res + 1;
            }
        return 0;
    }
    if (regexp[0] == '$' && regexp[1] == '\0')
        return *text == '\0';
    //if (regexp[0] == '\\')
      //  return *text == '.' && rmatchhere(rm, regexp + 2, text + 1) + 1;
    if ((*text != '\0' && !rm->binary) && (regexp[0] == '.' || regexp[0] == *text))
        if ((res = rmatchhere(rm, regexp + 1, text + 1)))
        {
           rm->position = res + 1;
            return res + 1;
        }    
    return 0;
}
int rmatchgreedy(rmatch_t * rm, int c, unsigned char *regexp, unsigned char *text, int required)
{
    int rmatch_success = 0;
    int res = 0;
    do
    {
        rmatch_success++;
        if (!(res = rmatchhere(rm,regexp, text)))
             continue;
        res = rmatch_success + res;
        if (!required)
        {
            rm->position = res;
            return res;
        }
        else if (required && res > 1){
            rm->position = res;
            return res;
        }
    } while (*text != '\0' && ((*text++ == c) || c == '.'));
    return 0;
}

char * rmatch_replace(char *regexpr, char ** txt, char *replacement){
    char * txt_ptr = *txt;
    rmatch_t * match_info = rmatch(regexpr, &txt_ptr);
    if(!match_info->valid){
        return NULL;
    }
    char * txt_end = txt_ptr;
    if(*txt_ptr){
        char tail[strlen(txt_ptr) + 1];
        strcpy(tail,txt_end);
        char * txt_start = txt_ptr - match_info->length; // test
        *txt_start = 0;
        strcat(txt_start,replacement);
        strcat(txt_start,tail);
    }else{
        strcat(txt_ptr,replacement);
    }
    char * txt_original = *txt;
    *txt = txt_ptr;
    return txt_original;
}
char *rmatch_extract(char *regexp, char **txt)
{
    rmatch_t * result = rmatch(regexp, txt);
    if (result->valid)
    {
        char *extracted = *txt - result->length;
        char *str = (char *)malloc(result->length + 1);
        strncpy(str, extracted, result->length);
        str[result->length] = 0;
        return str;
    }
    return NULL;
}

unsigned int rmatch_count(char * expr, char * text){
    unsigned occurence_count = 0;
    while(1){
        rmatch_t * match_info = rmatch(expr, &text);
        if(!match_info->valid)
            break;
        occurence_count++;
    }
    return occurence_count;
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

void rmatch_example_one()
{
    char *str = "testtest";
    char *expr = "t.*t";
    // Two times it equals to "test"
    assert(!strcmp(rmatch_extract(expr, &str), "test"));
    assert(!strcmp(rmatch_extract(expr, &str), "test"));
    // Third time it equals to NULL
    assert(!rmatch_extract(expr, &str));
}

void rmatch_example_two()
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
void rmatch_tests_compile(){
    printf("<%s>\n",rmatch_compile("a*bb*.*[a][b][0-9a-zA-ZA-B1-2a-b]"));
    assert(!strcmp("a*bb*.*ab[\\d\\l\\uA-B1-2a-b]", rmatch_compile("a*bb*.*[a][b][0-9a-zA-ZA-B1-2a-b]")));
}
void rmatch_tests_slash(){
    rmatch_t * result ;
    char * text = "TeS3";
    char * text_ptr;
    char optimized_original = rmatch_optimize;
    
    // Test without optimizer
    rmatch_optimize = 0;
    text_ptr = text;
    result = rmatch("\\u\\l\\w\\d",&text_ptr);
    assert(result->valid);
    assert(result->length == 4);
    assert(result->start == 0);
    assert(result->position == 4);

    // Test with optimizer
    rmatch_optimize = 1;
    text_ptr = text;
    result = rmatch("\\u\\l\\w\\d",&text_ptr);
    assert(result->valid);
    assert(result->length == 4);
    assert(result->start == 0);
    assert(result->position == 4);

    // Restore original optimizer value
    rmatch_optimize = optimized_original;
    
}
void rmatch_tests_replace(){
    char str[1024]; 
    strcpy(str, "testxtest");
    char * ptr = str;
    char * result = rmatch_replace("x", &ptr, "y");
    printf("%s\n",result);
    assert(!strcmp(result, "testytest"));
    assert(!strcmp(str, "testytest"));
    assert(!strcmp(ptr,"test"));
    result = rmatch_replace("es",&ptr,"zz");
    assert(!strcmp(result,"tzzt"));
    assert(!strcmp(str, "testytzzt"));
    assert(!strcmp(ptr,"t"));
    result = rmatch_replace("zz",&result,"dd");
    assert(!strcmp(result,"tddt"));
    assert(!strcmp(str, "testytddt"));
    assert(!strcmp(ptr,"t"));
    result = rmatch_replace("q",&ptr,"p");
    assert(result == NULL); // not found
    assert(!strcmp(ptr,"t")); // still the same
    assert(!strcmp(str, "testytddt")); // still the same
    ptr = str;
    result = rmatch_replace("dd",&ptr,"woop");
    assert(!strcmp(result,"testytwoopt"));
    printf("%s\n",ptr);
    assert(!strcmp(ptr,"opt"));
    assert(!strcmp(str, "testytwoopt"));
   // result = rmatch_replace("es",&str)
}

void rmatch_tests()
{
    // Examples
    printf("Testing example one.\n");
    rmatch_example_one();
    printf("Testing example two.\n");
    rmatch_example_two();
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
    rmatch_test("T.*e q.*k b.*n f.*x j.*s o.*r t.*e.*l.*[obcdefghia][zabcdefg].*y d[ao]g.", text_fox3, (char *[]){"The quick brown fox jumps over the lazy dog.", NULL});
    /// Plus
    printf("Testing plus.\n");
    char *text_fox4 = "The quick brown fooox jumps over the lazy dog.";
    rmatch_test("T.*e q.*k b.*n fo+x j.*s o.*r t.*e l[oa][az].*y d[ao]g.", text_fox4, (char *[]){"The quick brown fooox jumps over the lazy dog.", NULL});
    rmatch_test("T.*e q.*k b.*n f.+x j.*s o.*r t.*e l[oa][az].*y d[ao]g.", text_fox4, (char *[]){"The quick brown fooox jumps over the lazy dog.", NULL});
    rmatch_test("T.*e q.*k b.*n f+.*x j.*s o.*r t.*e l[oa][az].*y d[ao]g.", text_fox4, (char *[]){"The quick brown fooox jumps over the lazy dog.", NULL});
    rmatch_test("T.*e q.*k b.*n fo+x j.*s o.*r t.*e l[oa][az].*y d[ao]g.", text_fox4, (char *[]){"The quick brown fooox jumps over the lazy dog.", NULL});
    rmatch_test("T.*e q.*k b.*n f+x j.*s o.*r t.*e l[oa][az].*y d[ao]g.", text_fox4, (char *[]){NULL});
    rmatch_tests_replace();
    rmatch_tests_compile();
    rmatch_tests_slash();
}
#endif