#ifndef SPO_NODE_H
#define SPO_NODE_H

#include <sys/types.h>
#include <bits/types/FILE.h>
#include "parser.h"

typedef struct TreeNode TreeNode;
typedef struct TreeNodeChildrenInfo TreeNodeChildrenInfo;

struct TreeNode {
    char *type;
    TreeNode **children;
    long childrenQty;
    char *value;
    int id;
};

void printTree(TreeNode **treeNodes, int size, FILE *output_file);

TreeNode *createNode(char *type, TreeNode **childrenInfo, int childrenQty, char *value);

#endif //SPO_NODE_H
