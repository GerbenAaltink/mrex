#include "rmatch.h"
#include "bmatch.h"

/*
* Rmatcher with offset support
* Returns -1 if not found
* Returns offset if found
*/
extern int matchr(char * expr, char * str){
	rmatch_t *result = rmatch(expr,str);
	return result->valid ? result->start : -1;
}
/*
* Original beautiful code matcher with offset support
* Returns -1 if not found
* Returns offset if found
*/
extern int matchb(char * expr, char * str){
	return rbmatch(expr,str);
}
