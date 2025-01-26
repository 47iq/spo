#include <string.h>
#include "asm_gen.h"

void printExpression(TreeNode *treeNode, FILE* output);

void printOperationTreeNode(TreeNode *treeNode, FILE* output) {
    char* type = treeNode->type;
    if (!strcmp(type, "PLUS") || !strcmp(type, "MINUS") || !strcmp(type, "MUL")
            || !strcmp(type, "DIV") || !strcmp(type, "PERCENT") || !strcmp(type, "OR") || !strcmp(type, "AND")) {
        printExpression(treeNode, output);
    } else if (!strcmp(type, "CONST")) {
        // todo определиться как работать с константами
        // либо сторим их все в const_ram, либо команды для константных операндов
    } else if (!strcmp(type, "ASSIGN")) {
        printOperationTreeNode(treeNode->children[0], output);
        fprintf(output, "%s\n", "POP r0");
        // todo как определять тип, если нет typeRef
        fprintf(output, "%s\n", "ST r0 r1");
    } else if (!strcmp(type, "ARG")) {
        printOperationTreeNode(treeNode->children[0], output);
        fprintf(output, "%s\n", "POP r0");
        // todo находим адрес в фрейме либо создаем
        // todo тип определяем ифами по значению?
        fprintf(output, "%s\n", "ST r0 r1");
    } else if (!strcmp(type, "RET")) {
        // todo как возвращать значение из функции? всегда берем r0? резервируем ячейку в фрейме? передаем вершину стека?
    } else if (!strcmp(type, "CALL")) {
        // todo typeChecking для передаваемых аргументов?
    }
}

void printExecutionTreeNode(ExecutionNode* executionNode, FILE* output) {

}


void printExpression(TreeNode *treeNode, FILE* output) {
    char* type = treeNode->type;
    printOperationTreeNode(treeNode->children[0], output);
    printOperationTreeNode(treeNode->children[1], output);
    fprintf(output, "%s\n", "POP r1");
    fprintf(output, "%s\n", "POP r0");
    if (!strcmp(type, "PLUS")) {
        fprintf(output, "%s\n", "ADD r0 r1");
    } else if (!strcmp(type, "MINUS")) {
        fprintf(output, "%s\n", "SUB r0 r1");
    } else if (!strcmp(type, "MUL")) {
        fprintf(output, "%s\n", "MUL r0 r1");
    } else if (!strcmp(type, "DIV")) {
        fprintf(output, "%s\n", "DIV r0 r1");
    } else if (!strcmp(type, "PERCENT")) {
        fprintf(output, "%s\n", "REM r0 r1");
    } else if (!strcmp(type, "AND")) {
        fprintf(output, "%s\n", "AND r0 r1");
    } else if (!strcmp(type, "OR")) {
        fprintf(output, "%s\n", "OR r0 r1");
    }
}