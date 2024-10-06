#include <stdio.h>
#include "tree.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    if (argc != 1) {
        FILE *input = fopen(argv[1], "r");
        FILE *output = fopen(argv[2], "w");
        if (!input) {
            printf("cannot open files file: %s\n", argv[1]);
            return 1;
        }
        if (!output) {
            printf("cannot open output file: %s\n", argv[2]);
            return 1;
        }
        ResultTree *result = parse(input);
        for(int i = 0; i < result->errorsSize; i++)
            fprintf(stderr, "%s", result->errors[i]);
        printTree(result->tree, result->size, output);
        fclose(input);
        fclose(output);
        freeMemory(result);
    } else {
        printf("args format: [files-file-path] [output-file-path]");
    }
    return 0;
}