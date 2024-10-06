#include "parser.h"
#include "tree.h"

const int MAX_TREE_SIZE = 5000;
const int MAX_ERROR_SIZE = 1000;

TreeNode **tree;
int treeSize;
char **errors;
int errorsSize;

ResultTree *parse(FILE *file) {
    tree = malloc(MAX_TREE_SIZE * sizeof(TreeNode*));
    errors = malloc(MAX_ERROR_SIZE * sizeof(char*));
    treeSize = 0;
    errorsSize = 0;

    yyin = file;
    yyparse();

    ResultTree *parseResult = malloc(sizeof(ResultTree));
    parseResult->tree = tree;
    parseResult->size = treeSize;
    parseResult->errors = errors;
    parseResult->errorsSize = errorsSize;
    return parseResult;
}

void freeMemory(ResultTree *parseResult) {
    for (int i = parseResult->size - 1; i >= 0; i--) {
        TreeNode *node = parseResult->tree[i];
        free(node->children);
        free(node->value);
        free(node);
    }
    free(parseResult->errors);
    free(parseResult);
}