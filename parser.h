#ifndef SPO_PARSE_H
#define SPO_PARSE_H

#include <stdlib.h>
#include <stdio.h>
#include "tree.h"

typedef struct ChildNodes ChildNodes;
typedef struct TreeNode TreeNode;
typedef struct ChildNodes ChildNodes;
typedef struct ResultTree ResultTree;

extern const int MAX_TREE_SIZE;
extern const int MAX_ERROR_SIZE;

extern TreeNode **tree;
extern int treeSize;
extern char** errors;
extern int errorsSize;

extern int yyparse();

extern FILE *yyin;

struct ResultTree {
    int size;
    TreeNode **tree;
    char** errors;
    int errorsSize;
};

ResultTree* parse(FILE *file);

void freeMemory(ResultTree *parseResult);

#endif //SPO_PARSE_H
