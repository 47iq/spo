#include "../parser.h"

typedef struct FilenameParseTree FilenameParseTree;
typedef struct ExecutionNode ExecutionNode;
typedef struct FunExecution FunExecution;
typedef struct Array Array;

struct ExecutionNode {
    char *text;
    ExecutionNode *defaultBranch;
    ExecutionNode *conditionalBranch;
    TreeNode *operationTree;
    int id;
    int printed;
};

struct Array {
    int size;
    int nextPosition;
    void **elements;
};

struct FunExecution {
    char *name;
    TreeNode *funCalls;
    ExecutionNode *nodes;
    char **errors;
    int errorsCount;
};

struct FilenameParseTree {
    char *filename;
    ResultTree *tree;
};

Array *executionGraph(FilenameParseTree *input, int size);

void printExecution(FunExecution *funExecution, FILE *outputFunCallFile, FILE *outputOperationTreesFile,
                    FILE *outputExecutionFile);
