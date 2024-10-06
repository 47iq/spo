#include <stdio.h>
#include <string.h>
#include "error.h"
#include "parser.h"
#include "parser.tab.h"

void yyerror(const char *err) {
    char *buf = malloc(MAX_ERROR_SIZE * sizeof(char));
    sprintf(buf, "line %d: %s\n", yylineno, err);
    errors[errorsSize] = malloc(strlen(err));
    sprintf(errors[errorsSize], "%s", buf);
    free(buf);
    errorsSize++;
}