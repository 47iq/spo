#include "../parser.h"

typedef struct FilenameParseTree FilenameParseTree;
typedef struct ExecutionNode ExecutionNode;
typedef struct ExecutionInfo ExecutionInfo;
typedef struct Array Array;
typedef struct ListingNode ListingNode;

struct ExecutionNode {
    char *desc;
    TreeNode *operationTree;
    ExecutionNode *defaultBranch;
    ExecutionNode *conditionalBranch;
    int id;
    int printed;
    ListingNode *listingNode;
};

struct ExecutionInfo {
    char *name;
    TreeNode *funCalls;
    ExecutionNode *nodes;
    char *filename;
    TreeNode *funcSignature;
    char **errors;
    int errorsCount;
};

struct FilenameParseTree {
    char *filename;
    ResultTree *tree;
};

struct Array {
    int size;
    int nextPosition;
    void **elements;
};

Array *executionGraph(FilenameParseTree *input, int size);

void printExecution(ExecutionInfo *funExecution, FILE *outputFunCallFile, FILE *outputOperationTreesFile,
                    FILE *outputExecutionFile);
