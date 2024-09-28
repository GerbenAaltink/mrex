![workflow](https://github.com/GerbenAaltink/mrex/actions/workflows/make-mrex-single-platform.yml/badge.svg)

# Rmatch

## Summary
Super minimal regex validator.


## Supported functions
Support for:
 - extract matched data
 - . Does work in combination with greedy matching
 - ^
 - \+ greedy
 - \* greedy
 - [abc] (or). Does NOT work in combination with greedy matching

## Minimal usage matching
`rmatch(char * expr, char ** str)` is the main function. It returns int[3].
If you have a `char *` as str argument, don't forget to add the `&`.
The return value contains:
```
result[0] // 1 if validated. Else 0.
result[1] // match start.
result[2] // match length.
```

## Minimal usage extracting
`rmatch_extract(char * expr, char * str)` is the main function. It returns char * or NULL.
If you have a `char *` as str argument, don't forget to add the `&`.
You can use it in a loop since it uses the pointer of the input string:
```
char * str = "testtest";
char * expr = "t.*t";
// Two times it equals to "test"
assert(!strcmp(rmatch_extract(expr, &str), "test"));
assert(!strcmp(rmatch_extract(expr, &str), "test"));
// Third time it equals to NULL
assert(!rmatch_extract(expr, &str));
```
Other example:
```
char * str = "testtest";
char * expr = "t.*t";
char * result = NULL;
// Loops until NULL
while((result = rmatch_extract(expr, &str), "test")){
    printf("Found: \"%s\"\n", result);
}
// Be sure last result is invalid
assert(!result);
```

## Tests / examples
For executing tests in this repository, just run `make`.
You find test source [here](rmatch.c#tests).