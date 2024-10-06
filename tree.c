#include "tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


TreeNode *createNode(char *type, TreeNode **childrenInfo, int childrenQty, char *value) {
    TreeNode *node = malloc(sizeof(TreeNode));
    node->type = type;
    char *buf = malloc(1024 * sizeof(char));
    strcpy(buf, value);
    node->value = buf;
    TreeNode **children = malloc(sizeof(TreeNode*) * childrenQty);
    for (int i = 0; i < childrenQty; i++) {
        children[i] = childrenInfo[i];
    }
    node->children = children;
    node->childrenQty = childrenQty;
    tree[treeSize] = node;
    treeSize++;
    return node;
}

void printNodeVal(TreeNode *node, FILE *output_file) {
    fprintf(output_file,"node%d([Type: %s", node->id, node->type);
    if (strlen(node->value) > 0) {
        fprintf(output_file,", Value: %s", node->value);
    }
    fprintf(output_file,"])");
}

void printChildren(TreeNode *parent, FILE *output_file) {
    for (int i = 0; i < parent->childrenQty; ++i) {
        TreeNode *child = parent->children[i];
        if (child) {
            printNodeVal(parent, output_file);
            fprintf(output_file," --> ");
            printNodeVal(child, output_file);
            fprintf(output_file,"\n");
            printChildren(child, output_file);
        }
    }
}

void printTree(TreeNode **treeNodes, int size, FILE *output_file) {
    for (int i = 0; i < size; ++i) {
        tree[i]->id = i;
    }
    fprintf(output_file, "flowchart TB\n");
    printChildren(tree[size - 1], output_file);
    fprintf(output_file,"\n");
}