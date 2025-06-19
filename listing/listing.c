#include "listing.h"

ValuePlaceAssociation *addValuePlace(Array *valuePlaceAssociations, char *valuePlaceName, char *valuePlaceType);

void initListingParseNode(ExecutionNode *executionNode) {
    ListingNode *node = malloc(sizeof(ListingNode));
    node->node = executionNode;
    node->label = NULL;
    node->checked = 1;
    executionNode->listingNode = node;
}

void tryPlaceLabel(ExecutionNode *executionNode, int *labelCounter, bool necessaryLabeled) {
    if (executionNode == NULL) {
        return;
    }
    bool existListing = false;
    if (executionNode->listingNode != NULL) {
        existListing = true;
    } else {
        initListingParseNode(executionNode);
    }
    if (existListing || necessaryLabeled) {
        char *existingLabel = executionNode->listingNode->label;
        if (existingLabel == NULL) {
            ++(*labelCounter);
            char *label = malloc(sizeof(char) * 10);
            sprintf(label, "label%d", *labelCounter);
            executionNode->listingNode->label = label;
        }
    }
    if (!existListing) {
        tryPlaceLabel(executionNode->defaultBranch, labelCounter, false);
        tryPlaceLabel(executionNode->conditionalBranch, labelCounter, true);
    }
}

void placeLabels(Array *funExecutions) {
    int labelCounter = 0;
    for (int i = 0; i < funExecutions->nextPosition; ++i) {
        ExecutionInfo *funExecution = funExecutions->elements[i];
        tryPlaceLabel(funExecution->nodes, &labelCounter, false);
    }
}

ValuePlaceAssociation *findValuePlace(Array *valuePlaceAssociations, char *name) {
    for (int i = 0; i < valuePlaceAssociations->nextPosition; ++i) {
        ValuePlaceAssociation *valuePlaceAssociation = valuePlaceAssociations->elements[i];
        if (!strcmp(valuePlaceAssociation->name, name)) {
            return valuePlaceAssociation;
        }
    }
    return NULL;
}

void printException(char *message) {
    printf("EXCEPTION: %s", message);
}

void fprintlnWithArg(char *message, char *arg, FILE *file) {
    fprintf(file, "%s %s\n", message, arg);
}

void fprintln(char *message, FILE *file) {
    fprintf(file, "%s\n", message);
}

ValuePlaceAssociation *addArgumentPlace(Array *valuePlaceAssociations, char *argName, char *argType) {
    ValuePlaceAssociation *findRes = findValuePlace(valuePlaceAssociations, argName);
    if (findRes != NULL) {
        char exceptionMessage[1000];
        sprintf(exceptionMessage, "duplicate argument name %s", findRes->name);
        printException(exceptionMessage);
        return findRes;
    } else {
        for (int i = 0; i < valuePlaceAssociations->nextPosition; ++i) {
            ValuePlaceAssociation *previousArg = valuePlaceAssociations->elements[i];
            previousArg->shiftPosition -= 2;
        }
        ValuePlaceAssociation *newArgAssociation = malloc(sizeof(ValuePlaceAssociation));
        newArgAssociation->name = argName;
        newArgAssociation->type = argType;
        newArgAssociation->shiftPosition = -2;
        addToList(valuePlaceAssociations, newArgAssociation);
        return newArgAssociation;
    }
}

ValuePlaceAssociation *addValuePlace(Array *valuePlaceAssociations, char *valuePlaceName, char *valuePlaceType) {
    ValuePlaceAssociation *findRes = findValuePlace(valuePlaceAssociations, valuePlaceName);
    if (findRes != NULL) {
        char exceptionMessage[1000];
        sprintf(exceptionMessage, "duplicate value place name %s", findRes->name);
        printException(exceptionMessage);
        return findRes;
    } else {
        ValuePlaceAssociation *newAssociation = malloc(sizeof(ValuePlaceAssociation));
        newAssociation->name = valuePlaceName;
        newAssociation->type = valuePlaceType;
        ValuePlaceAssociation *lastElement = NULL;
        if (valuePlaceAssociations->nextPosition > 0) {
            lastElement = valuePlaceAssociations->elements[valuePlaceAssociations->nextPosition - 1];
        }
//        0 - адрес возврата, 1-2 возвращаемое значение
        int newElementShift = 3;
        if (lastElement != NULL && lastElement->shiftPosition > 0) {
            newElementShift = lastElement->shiftPosition + 2;
        }
        newAssociation->shiftPosition = newElementShift;
        addToList(valuePlaceAssociations, newAssociation);
        return newAssociation;
    }
}

void tryPrintOperationTreeNode(TreeNode *operationTree, FILE *listingFile, Array *valuePlaceAssociations,
                               int *argumentNumber) {
    char *operationType = operationTree->type;
    if (!strcmp(operationType, "ARG")) {
        (*argumentNumber)++;
        addArgumentPlace(
                valuePlaceAssociations,
                operationTree->children[1]->value,
                operationTree->children[0]->value
        );
    } else if (!strcmp(operationType, "AS")) {
        addValuePlace(
                valuePlaceAssociations,
                operationTree->children[1]->value,
                operationTree->children[0]->value
        );
    } else if (!strcmp(operationType, "CONST")) {
        if (!strcmp(operationTree->children[0]->value, "int")) {
            fprintlnWithArg("LD R0,", operationTree->children[1]->value, listingFile);
            fprintln("PUSH R0", listingFile);
        } else if (!strcmp(operationTree->children[0]->value, "char")) {
            char value[1];
            sprintf(value, "%d", (int) operationTree->children[1]->value[0]);
            fprintlnWithArg("LD R0,", value, listingFile);
            fprintln("PUSH R0", listingFile);
        } else if (!strcmp(operationTree->children[0]->value, "bool")) {
            if (!strcmp(operationTree->children[1]->value, "true")) {
                fprintlnWithArg("LD R0,", "1", listingFile);
                fprintln("PUSH R0", listingFile);
            } else {
                fprintlnWithArg("LD R0,", "0", listingFile);
                fprintln("PUSH R0", listingFile);
            }
        } else {
            fprintln("EXCEPTION", listingFile);
        }
    } else if (!strcmp(operationType, "ASSIGN")) {
        tryPrintOperationTreeNode(operationTree->children[1], listingFile, valuePlaceAssociations, argumentNumber);
        ValuePlaceAssociation *valuePlace = findValuePlace(valuePlaceAssociations, operationTree->children[0]->value);
        if (valuePlace == NULL) {
            valuePlace = addValuePlace(valuePlaceAssociations, operationTree->children[0]->value, operationTree->children[1]->type);
        }
        char valuePlaceShift[1000];
        sprintf(valuePlaceShift, "%d", valuePlace->shiftPosition);
        fprintln("POP R0", listingFile);
        fprintlnWithArg("ST_BP R0,", valuePlaceShift, listingFile);
    } else if (!strcmp(operationType, "READ")) {
        ValuePlaceAssociation *valuePlace = findValuePlace(valuePlaceAssociations, operationTree->children[0]->value);
        if (valuePlace == NULL) {
            char exceptionMessage[1000];
            sprintf(exceptionMessage, "value place not found by name %s", operationTree->children[0]->value);
            printException(exceptionMessage);
            return;
        }
        char valuePlaceShift[1000];
        sprintf(valuePlaceShift, "%d", valuePlace->shiftPosition);
        fprintlnWithArg("LD_BP R0,", valuePlaceShift, listingFile);
        fprintln("PUSH R0", listingFile);
    } else if (!strcmp(operationType, "EQITY")) {
        tryPrintOperationTreeNode(operationTree->children[0], listingFile, valuePlaceAssociations, argumentNumber);
        tryPrintOperationTreeNode(operationTree->children[1], listingFile, valuePlaceAssociations, argumentNumber);
        fprintln("POP R1", listingFile);
        fprintln("POP R0", listingFile);
        fprintln("EQ R0, R1", listingFile);
        fprintln("PUSH R0", listingFile);
    } else if (!strcmp(operationType, "NEQ")) {
        tryPrintOperationTreeNode(operationTree->children[0], listingFile, valuePlaceAssociations, argumentNumber);
        tryPrintOperationTreeNode(operationTree->children[1], listingFile, valuePlaceAssociations, argumentNumber);
        fprintln("POP R1", listingFile);
        fprintln("POP R0", listingFile);
        fprintln("NEQ R0, R1", listingFile);
        fprintln("PUSH R0", listingFile);
    } else if (!strcmp(operationType, "PLUS")) {
        tryPrintOperationTreeNode(operationTree->children[0], listingFile, valuePlaceAssociations, argumentNumber);
        tryPrintOperationTreeNode(operationTree->children[1], listingFile, valuePlaceAssociations, argumentNumber);
        fprintln("POP R1", listingFile);
        fprintln("POP R0", listingFile);
        fprintln("ADD R0, R1", listingFile);
        fprintln("PUSH R0", listingFile);
    } else if (!strcmp(operationType, "MINUS")) {
        tryPrintOperationTreeNode(operationTree->children[0], listingFile, valuePlaceAssociations, argumentNumber);
        tryPrintOperationTreeNode(operationTree->children[1], listingFile, valuePlaceAssociations, argumentNumber);
        fprintln("POP R1", listingFile);
        fprintln("POP R0", listingFile);
        fprintln("SUB R0, R1", listingFile);
        fprintln("PUSH R0", listingFile);
    } else if (!strcmp(operationType, "MUL")) {
        tryPrintOperationTreeNode(operationTree->children[0], listingFile, valuePlaceAssociations, argumentNumber);
        tryPrintOperationTreeNode(operationTree->children[1], listingFile, valuePlaceAssociations, argumentNumber);
        fprintln("POP R1", listingFile);
        fprintln("POP R0", listingFile);
        fprintln("MUL R0, R1", listingFile);
        fprintln("PUSH R0", listingFile);
    } else if (!strcmp(operationType, "DIV")) {
        tryPrintOperationTreeNode(operationTree->children[0], listingFile, valuePlaceAssociations, argumentNumber);
        tryPrintOperationTreeNode(operationTree->children[1], listingFile, valuePlaceAssociations, argumentNumber);
        fprintln("POP R1", listingFile);
        fprintln("POP R0", listingFile);
        fprintln("DIV R0, R1", listingFile);
        fprintln("PUSH R0", listingFile);
    } else if (!strcmp(operationType, "PERCENT")) {
        tryPrintOperationTreeNode(operationTree->children[0], listingFile, valuePlaceAssociations, argumentNumber);
        tryPrintOperationTreeNode(operationTree->children[1], listingFile, valuePlaceAssociations, argumentNumber);
        fprintln("POP R1", listingFile);
        fprintln("POP R0", listingFile);
        fprintln("REM R0, R1", listingFile);
        fprintln("PUSH R0", listingFile);
    } else if (!strcmp(operationType, "CALL")) {
        if (!strcmp(operationTree->children[0]->value, "stdin")) {
            fprintln("LD_IN R0", listingFile);
            fprintln("PUSH R0", listingFile);
        } else if (!strcmp(operationTree->children[0]->value, "stdout")) {
            tryPrintOperationTreeNode(
                    operationTree->children[1],
                    listingFile,
                    valuePlaceAssociations,
                    argumentNumber
            );
            fprintln("POP R0", listingFile);
            fprintln("ST_OUT R0", listingFile);
        } else {
            for (int i = 1; i < operationTree->childrenQty; ++i) {
                tryPrintOperationTreeNode(operationTree->children[i], listingFile, valuePlaceAssociations,
                                          argumentNumber);
            }
            fprintlnWithArg("CALL", operationTree->children[0]->value, listingFile);
        }
    } else {
        fprintln("EXCEPTION", listingFile);
    }
}

void tryPrintNode(ExecutionNode *executionNode, FILE *listingFile, Array *valuePlaceAssociations, int *argumentNumber) {
    if (executionNode == NULL) {
        return;
    }
    if (executionNode->listingNode->label != NULL) {
        char label[1000];
        sprintf(label, "%s:", executionNode->listingNode->label);
        fprintln(label, listingFile);
    }
    executionNode->listingNode->checked++;
    if (executionNode->operationTree != NULL) {
        tryPrintOperationTreeNode(executionNode->operationTree, listingFile, valuePlaceAssociations, argumentNumber);
        if (executionNode->conditionalBranch != NULL) {
            fprintln("POP R0", listingFile);
            fprintlnWithArg("JNE R0,", executionNode->conditionalBranch->listingNode->label, listingFile);
        }
    }
    if (executionNode->defaultBranch == NULL) {
        char argNumberString[10];
        sprintf(argNumberString, "%d", *argumentNumber);
        fprintln("ST_BP R0, 1", listingFile);
        fprintlnWithArg("RET", argNumberString, listingFile);
        return;
    }
    if (executionNode->defaultBranch->listingNode->checked > 1) {
        fprintlnWithArg("JUMP", executionNode->defaultBranch->listingNode->label, listingFile);
    } else {
        tryPrintNode(executionNode->defaultBranch, listingFile, valuePlaceAssociations, argumentNumber);
    }
    if (executionNode->conditionalBranch != NULL && executionNode->conditionalBranch->listingNode->checked > 1) {
        return;
    } else {
        tryPrintNode(executionNode->conditionalBranch, listingFile, valuePlaceAssociations, argumentNumber);
    }
}

void printListing(Array *funExecutions, FILE *listingFile) {
    fprintln("[section ram]", listingFile);
    fprintln("LOAD_HP end", listingFile);
    fprintln("CALL main", listingFile);
    //fprintln("POP R0", listingFile);
    //fprintln("HLT", listingFile);
    for (int i = 0; i < funExecutions->nextPosition; ++i) {
        int argumentNumber = 0;
        Array *valuePlaceAssociationsArray = malloc(sizeof(Array));
        valuePlaceAssociationsArray->size = 100;
        valuePlaceAssociationsArray->nextPosition = 0;
        valuePlaceAssociationsArray->elements = malloc(sizeof(ValuePlaceAssociation) * 100);
        ExecutionInfo *funExecution = funExecutions->elements[i];
        char funLabel[1000];
        sprintf(funLabel, "%s:", funExecution->name);
        fprintln(funLabel, listingFile);
        tryPrintNode(funExecution->nodes, listingFile, valuePlaceAssociationsArray, &argumentNumber);
        free(valuePlaceAssociationsArray);
    }
    fprintln("end:", listingFile);
}