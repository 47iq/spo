#include "main.h"
#include "parser.h"
#include "graph/execution.h"
#include <stdio.h>
#include <dirent.h>
#include <libgen.h>

int main(int argc, char *argv[]) {
    if (argc > 2) {
        DIR *outputDir = opendir(argv[1]);
        if (!outputDir) {
            printf("cannot open output dir: %s\n", argv[1]);
            return 1;
        }
        FILE *inputFiles;
        for (int i = 2; i < argc; ++i) {
            inputFiles = fopen(argv[i], "r");
            if (!inputFiles) {
                printf("cannot open input file: %s\n", argv[i]);
                return 1;
            }
        }
        for (int i = 0; i < argc - 2; ++i) {
            char *filename = basename(argv[i + 2]);
            FILE *inputFile = &inputFiles[i];
            char outputParseTreeFileName[1024];
            sprintf(outputParseTreeFileName, "%s/%s-parse-tree.txt", argv[1], basename(filename));
            FILE *outputParseTreeFile = fopen(outputParseTreeFileName, "w");
            ResultTree *result = parse(inputFile);
            printTree(result->tree, result->size, outputParseTreeFile);
            Array *executionRes =
                    executionGraph(&(FilenameParseTree) {filename, result}, 1);
            for (int j = 0; j < result->errorsSize; ++j) {
                fprintf(stderr, "%s", result->errors[j]);
            }
            for (int j = 0; j < executionRes->nextPosition; ++j) {
                ExecutionInfo *funExecution = executionRes->elements[j];
                for (int k = 0; k < funExecution->errorsCount; ++k) {
                    fprintf(stderr, "%s", funExecution->errors[k]);
                }
            }
            char outputFunCallFileName[1024];
            sprintf(outputFunCallFileName, "%s/%s.ext-fun-call.txt", argv[1], basename(filename));
            FILE *outputFunCallFile = fopen(outputFunCallFileName, "w");
            char outputOperationTreesFileName[1024];
            sprintf(outputOperationTreesFileName, "%s/%s.ext-operation-tree.txt", argv[1],
                    basename(filename));
            FILE *outputOperationTreesFile = fopen(outputOperationTreesFileName, "w");
            char outputExecutionFileName[1024];
            sprintf(outputExecutionFileName, "%s/%s.ext", argv[1],
                    basename(filename));
            FILE *outputExecutionFile = fopen(outputExecutionFileName, "w");

            fprintf(outputOperationTreesFile, "flowchart TB\n");
            fprintf(outputExecutionFile, "flowchart TB\n");
            fprintf(outputFunCallFile, "flowchart TB\n");

            for (int j = 0; j < executionRes->nextPosition; ++j) {
                ExecutionInfo *funExecution = executionRes->elements[j];
                printExecution(funExecution, outputFunCallFile, outputOperationTreesFile, outputExecutionFile);
            }

            fprintf(outputOperationTreesFile, "\n");
            fprintf(outputExecutionFile, "\n");
            fprintf(outputFunCallFile, "\n");
            fclose(outputFunCallFile);
            fclose(outputOperationTreesFile);
            fclose(outputExecutionFile);
            fclose(outputParseTreeFile);
            freeMemory(result);
        }

        closedir(outputDir);
        for (int i = 0; i < argc - 2; ++i) {
            fclose(&inputFiles[i]);
        }
    } else {
        printf("Use parameter: with output dir and input files");
    }
    return 0;
}