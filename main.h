#ifndef SPO_ERROR_H
#define SPO_ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include "stdbool.h"
#include <string.h>
#include "parser.tab.h"
#include "graph/execution.h"
#include "graph/utils.h"
#include "listing/listing.h"

extern int yyparse();

extern FILE *yyin;

#endif //SPO_ERROR_H
