

typedef struct rmatch2_t {
    unsigned int valid;
    unsigned int start;
    unsigned int length;
    unsigned int position;
    
    unsigned int binary;
    long size;

} rmatch2_t;

rmatch2_t * rmatch2(char *regexp, char **txt)