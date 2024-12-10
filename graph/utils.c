#include <string.h>
#include "execution.h"
#include "utils.h"

char *mallocString(char *text) {
    char *pointer = malloc(sizeof(char) * 1024);
    sprintf(pointer, "%s", text);
    return pointer;
}

void addToList(Array *currentArray, void *element) {
    void **nodes;
    if (currentArray->size != currentArray->nextPosition) {
        nodes = currentArray->elements;
    } else {
        nodes = malloc(sizeof(void *) * 2 * currentArray->size);
        for (int i = 0; i < currentArray->size; ++i) {
            nodes[i] = currentArray->elements[i];
        }
        free(currentArray->elements);
    }
    nodes[currentArray->nextPosition] = element;
    currentArray->nextPosition += 1;
}

Array findListItemsUtil(TreeNode *treeNode) {
    TreeNode **nodes = malloc(sizeof(TreeNode *) * 10);
    Array items = {10, 0, (void **) nodes};

    TreeNode *currentListNode = treeNode;
    do {
        if (currentListNode->childrenQty == 0) {
            currentListNode = NULL;
        } else if (currentListNode->childrenQty == 1) {
            addToList(&items, currentListNode->children[0]);
            currentListNode = NULL;
        } else if (currentListNode->childrenQty == 2) {
            addToList(&items, currentListNode->children[0]);
            currentListNode = currentListNode->children[1];
        }
    } while (currentListNode != NULL);
    return items;
}

