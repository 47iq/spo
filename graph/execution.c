#include "execution.h"
#include "utils.h"
#include <string.h>

typedef struct FunCalls FunCalls;

struct FunCalls {
    Array *funCalls;
};

const int START_ARRAY_SIZE = 5000;
Array exceptions;
int currentExecutionId = 0;

ExecutionNode *executionNode(TreeNode *treeNode, ExecutionNode *nextNode,
                             ExecutionNode *breakNode, FunCalls *funCalls);

void addException(char *text) {
    char *exception = mallocString(text);
    addToList(&exceptions, exception);
}

TreeNode *findSourceNode(FilenameParseTree input) {
    TreeNode **inputNodes = input.tree->tree;
    int inputNodesSize = input.tree->size;
    TreeNode *sourceNode = inputNodes[inputNodesSize - 1];
    return sourceNode;
}

ExecutionNode *initExecutionNode(char *text) {
    ExecutionNode *node = malloc(sizeof(ExecutionNode));
    node->id = currentExecutionId++;
    node->desc = mallocString(text);
    node->defaultBranch = NULL;
    node->conditionalBranch = NULL;
    node->operationTree = NULL;
    node->printed = 0;
    return node;
}

TreeNode *mallocTreeNode(char *type, char *value, int nodeNumber) {
    TreeNode *node = malloc(sizeof(TreeNode));
    node->id = currentExecutionId++;
    if (type) {
        node->type = mallocString(type);
    } else {
        node->type = NULL;
    }
    if (value) {
        node->value = mallocString(value);
    } else {
        node->value = NULL;
    }
    node->children = malloc(sizeof(TreeNode *) * nodeNumber);
    node->childrenQty = nodeNumber;
    return node;
}

ExecutionNode *executionListStatementNode(TreeNode *treeNode,
                                          ExecutionNode *nextNode,
                                          ExecutionNode *breakNode,
                                          FunCalls *funCalls) {
    ExecutionNode *tmpNextNode = nextNode;
    if (treeNode->childrenQty == 2) {
        tmpNextNode = executionNode(treeNode->children[1], nextNode, breakNode, funCalls);
    }
    ExecutionNode *node = initExecutionNode("");
    node->defaultBranch =
            executionNode(treeNode->children[0], tmpNextNode, breakNode, funCalls);
    return node;
}

TreeNode *operationTreeNode(TreeNode *parsingTree, FunCalls *funCalls) {
    TreeNode *node = NULL;

    if (!strcmp(parsingTree->type, "braces")) {
        return operationTreeNode(parsingTree->children[0], funCalls);
    } else if (!strcmp(parsingTree->type, "slice")) {
        char sliceName[1024];
        sprintf(sliceName,
                "slice: %s",
                parsingTree->children[0]->value);
        if (parsingTree->childrenQty == 1) {
            node = mallocTreeNode(NULL, sliceName, 1);
            node->children[0] = operationTreeNode(parsingTree->children[0], funCalls);
        } else {
            Array argsArray = findListItemsUtil(parsingTree->children[1]);
            node = mallocTreeNode(NULL, sliceName, argsArray.nextPosition + 1);
            node->children[0] = operationTreeNode(parsingTree->children[0], funCalls);
            for (int i = 1; i <= argsArray.nextPosition; ++i) {
                node->children[i] = operationTreeNode(argsArray.elements[i - 1], funCalls);
            }
        }
    } else if (!strcmp(parsingTree->type, "block")) {
        return operationTreeNode(parsingTree->children[0], funCalls);
    } else if (!strcmp(parsingTree->type, "call")) {
        if (parsingTree->childrenQty == 1) {
            node = mallocTreeNode("CALL", NULL, 0);
        } else {
            Array argsArray = findListItemsUtil(parsingTree->children[1]->children[0]);
            node = mallocTreeNode("CALL", NULL, argsArray.nextPosition + 1);
            for (int i = 0; i < argsArray.nextPosition; ++i) {
                node->children[i + 1] = operationTreeNode(argsArray.elements[i], funCalls);
            }
        }
        char executionName[1024];
        sprintf(executionName, "%s", parsingTree->children[0]->value);
        node->children[0] = mallocTreeNode(NULL, executionName, 0);
        char funCallOperationIdNodeString[1024];
        sprintf(funCallOperationIdNodeString, "%d", node->id);
        TreeNode *funCallOperationIdNode = mallocTreeNode("operationTreeId", funCallOperationIdNodeString, 1);
        TreeNode *calledFunNameNode = mallocTreeNode("call", parsingTree->children[0]->value, 0);
        funCallOperationIdNode->children[0] = calledFunNameNode;
        addToList(funCalls->funCalls, funCallOperationIdNode);
    } else if (parsingTree->childrenQty == 3) {
        node = mallocTreeNode(parsingTree->type, parsingTree->value, parsingTree->childrenQty);
        node->children[0] = operationTreeNode(parsingTree->children[0], funCalls);
        node->children[1] = operationTreeNode(parsingTree->children[1], funCalls);
        node->children[2] = operationTreeNode(parsingTree->children[2], funCalls);
    } else if (parsingTree->childrenQty == 2) {
        node = mallocTreeNode(parsingTree->type, parsingTree->value, parsingTree->childrenQty);

        if (!strcmp(parsingTree->type, "ASSIGN")) {
            char valuePlace[1024];
            sprintf(valuePlace,
                    "%s",
                    parsingTree->children[0]->value);
            node->children[0] = mallocTreeNode(NULL, valuePlace, 0);
            node->children[1] = operationTreeNode(parsingTree->children[1], funCalls);
        } else {
            node->children[0] = operationTreeNode(parsingTree->children[0], funCalls);
            node->children[1] = operationTreeNode(parsingTree->children[1], funCalls);
        }
    } else if (parsingTree->childrenQty == 0) {
        if (!strcmp(parsingTree->type, "IDENTIFIER")) {
            node = mallocTreeNode("READ", NULL, 1);
            char valuePlace[1024];
            sprintf(valuePlace,
                    "%s",
                    parsingTree->value);
            node->children[0] = mallocTreeNode(NULL, valuePlace, 0);
        } else {
            node = mallocTreeNode("CONST", NULL, 2);
            char *typeByLiteral = "";
            if (!strcmp(parsingTree->type, "DEC")) {
                typeByLiteral = "int";
            } else if (!strcmp(parsingTree->type, "BIN")) {
                typeByLiteral = "int";
            } else if (!strcmp(parsingTree->type, "HEX")) {
                typeByLiteral = "int";
            } else if (!strcmp(parsingTree->type, "CHAR")) {
                typeByLiteral = "char";
            } else if (!strcmp(parsingTree->type, "STR")) {
                typeByLiteral = "str";
            } else if (!strcmp(parsingTree->type, "TRUE")) {
                typeByLiteral = "bool";
            } else if (!strcmp(parsingTree->type, "FALSE")) {
                typeByLiteral = "bool";
            }
            char constVal[1024];
            sprintf(constVal, "%s", typeByLiteral);
            node->children[0] = mallocTreeNode(NULL, typeByLiteral, 0);
            node->children[1] = mallocTreeNode(NULL, parsingTree->value, 0);
        }
    } else if (parsingTree->childrenQty == 1) {
        node = mallocTreeNode(parsingTree->type, parsingTree->value, parsingTree->childrenQty);
        node->children[0] = operationTreeNode(parsingTree->children[0], funCalls);
    }
    return node;
}

// декларация аргументов функции
ExecutionNode *functionArgsExecutionNode(TreeNode *functionSignatureNode, ExecutionNode *nextNode,
                                         ExecutionNode *breakNode, FunCalls *funCalls) {
    ExecutionNode *node = initExecutionNode("");
    node->defaultBranch = nextNode;
    if (functionSignatureNode->children > 0 &&
            functionSignatureNode->children[0] > 0 &&
        !strcmp(functionSignatureNode->children[0]->children[0]->type, "argListItems")) {
        Array args = findListItemsUtil(functionSignatureNode->children[0]->children[0]);
        ExecutionNode *parentNode = node;
        for (int i = 0; i < args.nextPosition; ++i) {
            TreeNode *argDef = args.elements[i];
            char argText[1024];
            sprintf(argText, "ARG %s %s", argDef->children[0]->value, argDef->children[1]->value);
            ExecutionNode *newDimNode = initExecutionNode(argText);
            newDimNode->operationTree = mallocTreeNode("ARG", NULL, 2);
            newDimNode->operationTree->children[0] = mallocTreeNode(NULL, argDef->children[1]->value, 0);
            newDimNode->operationTree->children[1] = mallocTreeNode(NULL, argDef->children[0]->value, 0);
            newDimNode->defaultBranch = parentNode->defaultBranch;
            parentNode->defaultBranch = newDimNode;
            parentNode = newDimNode;
        }
        parentNode->defaultBranch = nextNode;
    }
    return node;
}

char *expressionNodeToString(TreeNode *treeNode) {
    if (treeNode->childrenQty == 0) {
        return mallocString(treeNode->value);
    } else if (treeNode->childrenQty == 1) {
        char *childStr = expressionNodeToString(treeNode->children[0]);
        char text[1024];
        sprintf(text,
                "%s⟨%s⟩",
                treeNode->type, childStr);
        return mallocString(text);
    } else {
        char *childLeftStr = expressionNodeToString(treeNode->children[0]);
        char *childRightStr = expressionNodeToString(treeNode->children[1]);
        char text[1024];
        sprintf(text,
                "%s %s %s",
                childLeftStr, treeNode->type, childRightStr);
        return mallocString(text);
    }
}

ExecutionNode *executionExpressionNode(TreeNode *treeNode, ExecutionNode *nextNode, FunCalls *funCalls) {
    ExecutionNode *node = initExecutionNode(expressionNodeToString(treeNode));
    node->operationTree = operationTreeNode(treeNode, funCalls);
    node->defaultBranch = nextNode;
    return node;
}

ExecutionNode *executionElseNode(TreeNode *treeNode, ExecutionNode *nextNode,
                                 ExecutionNode *breakNode, FunCalls *funCalls) {
    ExecutionNode *node = initExecutionNode("");
    if (treeNode->childrenQty == 1) {
        node->defaultBranch = executionNode(treeNode->children[0],
                                         nextNode, breakNode, funCalls);
    } else {
        node->defaultBranch = nextNode;
    }
    return node;
}

ExecutionNode *executionIfNode(TreeNode *treeNode, ExecutionNode *nextNode,
                               ExecutionNode *breakNode, FunCalls *funCalls) {
    ExecutionNode *node = initExecutionNode("");
    TreeNode *elseTreeNode = NULL;
    TreeNode *ifStatements = NULL;
    if (treeNode->childrenQty == 3) {
        ifStatements = treeNode->children[1];
        elseTreeNode = treeNode->children[2];
    } else if (treeNode->childrenQty == 2 &&
               !strcmp(treeNode->children[1]->type, "else")) {
        elseTreeNode = treeNode->children[1];
    } else if (treeNode->childrenQty == 2) {
        ifStatements = treeNode->children[1];
    }

    ExecutionNode *conditionNextNode = NULL;
    ExecutionNode *conditionConditionallyNode = NULL;
    if (elseTreeNode != NULL) {
        ExecutionNode *elseNode =
                executionElseNode(elseTreeNode, nextNode, breakNode, funCalls);
        conditionNextNode = elseNode;
    } else {
        conditionNextNode = nextNode;
    }
    if (ifStatements != NULL) {
        ExecutionNode *statementsNode =
                executionNode(ifStatements, nextNode, breakNode, funCalls);
        conditionConditionallyNode = statementsNode;
    }
    ExecutionNode *conditionNode = executionExpressionNode(treeNode->children[0], conditionNextNode, funCalls);
    node->defaultBranch = conditionNode;
    conditionNode->conditionalBranch = conditionConditionallyNode;
    return node;
}

ExecutionNode *executionLoopNode(TreeNode *treeNode, ExecutionNode *nextNode,
                                 ExecutionNode *breakNode, FunCalls *funCalls) {
    ExecutionNode *node = initExecutionNode("");
    ExecutionNode *statementNode = NULL;
    if (treeNode->childrenQty == 2) {
        statementNode = executionNode(treeNode->children[1], node, nextNode, funCalls);
    } else {
        statementNode = initExecutionNode("");
        statementNode->defaultBranch = node;
    }
    ExecutionNode *conditionNode = executionExpressionNode(treeNode->children[0], nextNode, funCalls);
    node->defaultBranch = conditionNode;
    conditionNode->conditionalBranch = statementNode;
    return node;
}

ExecutionNode *executionRepeatNode(TreeNode *treeNode, ExecutionNode *nextNode, FunCalls *funCalls) {
    ExecutionNode *node = initExecutionNode("");
    ExecutionNode *conditionNode = executionExpressionNode(treeNode->children[0], nextNode, funCalls);
    conditionNode->conditionalBranch = node;
    ExecutionNode *statementNode = NULL;
    if (treeNode->childrenQty == 2) {
        statementNode = executionNode(treeNode->children[1], conditionNode, nextNode, funCalls);
    } else {
        statementNode = initExecutionNode("");
        statementNode->defaultBranch = conditionNode;
    }
    node->defaultBranch = statementNode;
    return node;
}

ExecutionNode *executionBreakNode(TreeNode *treeNode, ExecutionNode *nextNode,
                                  ExecutionNode *breakNode) {
    ExecutionNode *node = initExecutionNode("");
    if (breakNode == NULL) {
        char exceptionText[1024];
        sprintf(exceptionText,
                "Exception: BREAK outside loop for tree node id --> %d",
                treeNode[0].id);
        addException(exceptionText);
        ExecutionNode *exceptionNode = initExecutionNode(exceptionText);
        node->defaultBranch = exceptionNode;
        exceptionNode->defaultBranch = nextNode;
    } else {
        node->defaultBranch = breakNode;
    }
    return node;
}

ExecutionNode *executionBlockNode(TreeNode *treeNode, ExecutionNode *nextNode,
                                  ExecutionNode *breakNode, FunCalls *funCalls) {
    return executionNode(treeNode->children[0],
                         nextNode, breakNode, funCalls);
}

ExecutionNode *executionNode(TreeNode *treeNode, ExecutionNode *nextNode,
                             ExecutionNode *breakNode, FunCalls *funCalls) {
    if (!strcmp(treeNode[0].type, "statementList")) {
        return executionListStatementNode(treeNode, nextNode, breakNode, funCalls);
    } else if (!strcmp(treeNode[0].type, "if")) {
        return executionIfNode(treeNode, nextNode, breakNode, funCalls);
    } else if (!strcmp(treeNode[0].type, "loop")) {
        return executionLoopNode(treeNode, nextNode, breakNode, funCalls);
    } else if (!strcmp(treeNode[0].type, "repeat")) {
        return executionRepeatNode(treeNode, nextNode, funCalls);
    } else if (!strcmp(treeNode[0].type, "break")) {
        return executionBreakNode(treeNode, nextNode, breakNode);
    } else if (!strcmp(treeNode[0].type, "block")) {
        return executionBlockNode(treeNode, nextNode, breakNode, funCalls);
    } else {
        return executionExpressionNode(treeNode, nextNode, funCalls);
    }
}

ExecutionNode *initGraph(TreeNode *sourceItem, FunCalls *funCalls) {
    ExecutionNode *startNode = initExecutionNode("START");
    ExecutionNode *endNode = initExecutionNode("END");
    ExecutionNode *listStatementNode = endNode;
    TreeNode *funcDef = sourceItem;
    if (funcDef->childrenQty == 2) {
         listStatementNode = executionNode(funcDef->children[1], endNode, NULL, funCalls);
    }
    ExecutionNode *functionArgs =
            functionArgsExecutionNode(funcDef->children[0], listStatementNode, NULL, funCalls);
    startNode->defaultBranch = functionArgs;
    return startNode;
}

void initExceptions() {
    void **nodes = malloc(sizeof(char *) * START_ARRAY_SIZE);
    exceptions = (Array) {START_ARRAY_SIZE, 0, nodes};
}

Array *executionGraph(FilenameParseTree *input, int size) {
    void **resultNodes = malloc(sizeof(ExecutionInfo *) * START_ARRAY_SIZE);
    Array *result = malloc(sizeof(Array));
    result->size = START_ARRAY_SIZE;
    result->nextPosition = 0;
    result->elements = resultNodes;

    for (int i = 0; i < size; ++i) {
        initExceptions();
        TreeNode *sourceNode = findSourceNode(input[i]);
        Array sourceItems;
        if (sourceNode->childrenQty != 0) {
            TreeNode *sourceItemsList = sourceNode->children[0];
            sourceItems = findListItemsUtil(sourceItemsList);
        } else {
            sourceItems = (Array) {0, 0, NULL};
        }
        for (int j = 0; j < sourceItems.nextPosition; ++j) {
            ExecutionInfo *currentFunExecution = malloc(sizeof(ExecutionInfo));
            currentFunExecution->filename = input[i].filename;
            TreeNode **currentSourceItemElements =
                    ((TreeNode **) sourceItems.elements);
            currentFunExecution->name =
                    currentSourceItemElements[j]->children[0]->value;
            currentFunExecution->funcSignature =
                    currentSourceItemElements[j]->children[0];

            void **nodes = malloc(sizeof(TreeNode *) * START_ARRAY_SIZE);
            Array funs = (Array) {START_ARRAY_SIZE, 0, nodes};
            FunCalls funCalls = (FunCalls) {&funs};
            currentFunExecution->nodes = initGraph(currentSourceItemElements[j], &funCalls);
            TreeNode *funCallsRoot = mallocTreeNode("function", currentFunExecution->name,
                                                    funCalls.funCalls->nextPosition);
            for (int k = 0; k < funCalls.funCalls->nextPosition; ++k) {
                funCallsRoot->children[k] = funCalls.funCalls->elements[k];
            }
            currentFunExecution->funCalls = funCallsRoot;
            currentFunExecution->errorsCount = exceptions.nextPosition;
            currentFunExecution->errors = (char **) exceptions.elements;
            addToList(result, currentFunExecution);
        }
    }
    return result;
}

void printNode(TreeNode *node, FILE *outputFile) {
    if (node == NULL) {
        return;
    }
    int childrenNumber = node->childrenQty;
    for (int i = 0; i < childrenNumber; ++i) {
        printNode(node->children[i], outputFile);
        fprintf(outputFile, "node%d", node->id);
        fprintf(outputFile, "([");
        int typeExists = 0;
        if (node->type != NULL && strlen(node->type) > 0) {
            fprintf(outputFile, "Type: %s", node->type);
            typeExists = 1;
        }
        if (node->value != NULL && strlen(node->value) > 0) {
            if (typeExists) {
                fprintf(outputFile, ", ", node->value);
            }
            fprintf(outputFile, "Value: %s", node->value);
        }
        fprintf(outputFile, "])");

        TreeNode *childNode = node->children[i];
        fprintf(outputFile, " --> ");
        fprintf(outputFile, "node%d", childNode->id);
        fprintf(outputFile, "([");
        typeExists = 0;
        if (childNode->type != NULL && strlen(childNode->type) > 0) {
            fprintf(outputFile, "Type: %s", childNode->type);
            typeExists = 1;
        }
        if (childNode->value != NULL && strlen(childNode->value) > 0) {
            if (typeExists) {
                fprintf(outputFile, ", ");
            }
            fprintf(outputFile, "Value: %s", childNode->value);
        }
        fprintf(outputFile, "])");
        fprintf(outputFile, "\n");
    }
}

void printExecutionNode(ExecutionNode *father, ExecutionNode *child, FILE *outputFile, char *relationName) {
    fprintf(outputFile, "node%d", father->id);
    fprintf(outputFile, "([");
    fprintf(outputFile, "Text: %s", father->desc);
    fprintf(outputFile, "])");
    fprintf(outputFile, " --%s--> ", relationName);
    fprintf(outputFile, "node%d", child->id);
    fprintf(outputFile, "([");
    fprintf(outputFile, "Text: %s", child->desc);
    fprintf(outputFile, "])");
    fprintf(outputFile, "\n");
}

void printExecutionGraphNodeToFile(ExecutionNode *executionNode, FILE *outputOperationTreesFile,
                                   FILE *outputExecutionFile) {
    if (executionNode->printed) {
        return;
    } else {
        executionNode->printed = 1;
    }

    if (executionNode->operationTree) {
        char linkedExecutionNodeId[1024];
        sprintf(linkedExecutionNodeId, "%d", executionNode->id);
        TreeNode *linkedExecutionNode = mallocTreeNode("linked execution node id", linkedExecutionNodeId, 1);
        linkedExecutionNode->children[0] = executionNode->operationTree;
        printNode(linkedExecutionNode, outputOperationTreesFile);
    }

    ExecutionNode *defaultBranch = executionNode->defaultBranch;
    if (defaultBranch) {
        printExecutionGraphNodeToFile(defaultBranch, outputOperationTreesFile, outputExecutionFile);

        printExecutionNode(executionNode, defaultBranch, outputExecutionFile, "defaultBranch");
    }

    ExecutionNode *conditionalBranch = executionNode->conditionalBranch;
    if (conditionalBranch) {
        printExecutionGraphNodeToFile(conditionalBranch, outputOperationTreesFile, outputExecutionFile);

        printExecutionNode(executionNode, conditionalBranch, outputExecutionFile, "conditionalBranch");
    }
}

void printExecution(ExecutionInfo *funExecution, FILE *outputFunCallFile, FILE *outputOperationTreesFile,
                    FILE *outputExecutionFile) {
    printNode(funExecution->funCalls, outputFunCallFile);
    printExecutionGraphNodeToFile(funExecution->nodes, outputOperationTreesFile, outputExecutionFile);
}
