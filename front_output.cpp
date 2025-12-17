#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "lib/tree/diff_dump.h"
#include "../diff_struct.h"
#include "get_tokens.h"
#include "create_tokens_tree.h"
#include "semantical_analize.h"
#include "front_output.h"

const char *DEFAULT_OUTPUT_FRONT_FILE_NAME = "lang_front_output.txt";

#define SYNTAX_ERROR { fprintf(stderr, "SYNTAX ERROR from " __FILE__ ":%d:0 at %s\n\n", __LINE__, __FUNCTION__); assert(NULL); }

void PrintToken(Node_t *node, FILE *output_file_ptr) {

    assert(node);

    fprintf(output_file_ptr, " ( ");

    switch (node->type) {
        case OperationType:
            fprintf(output_file_ptr, "\"OP %s\"", GET_OP_STANDARD_NAME(node));
            break;

        case VariableType:
            fprintf(output_file_ptr, "\"VAR %s\"", GET_VARIABLE_NAME(node));
            break;

        case NumberType:
            fprintf(output_file_ptr, "\"NUM %0.lf\"", GET_NODE_NUM_VALUE(node));
            break;

        case ServiceType:
            fprintf(output_file_ptr, "\"SERV %s\"", GET_SERV_STANDARD_NAME(node));
            break;

        case ComparisonOperatorType:
            fprintf(output_file_ptr, "\"COMP %s\"", GET_COMPARISON_STANDARD_NAME(node));
            break;

        case ControlStructureType:
            fprintf(output_file_ptr, "\"CONTROL %s\"", GET_CONTROL_STANDARD_NAME(node));
            break;

        case EmptyOperatorType:
            fprintf(output_file_ptr, "\";\"");
            break;

        case FunctionCallType:
            fprintf(output_file_ptr, "\"CALL\"");
            break;

        case FunctionType:
            fprintf(output_file_ptr, "\"FUNC %s\"", GET_FUNCTION_NAME(node));
            break;

        case NewFuncType:
            fprintf(output_file_ptr, "\"NEWFUNC\"");
            break;

        default:
            SYNTAX_ERROR;
    }

    if (node->left != NULL)
        PrintToken(node->left, output_file_ptr);
    else
        fprintf(output_file_ptr, " nil ");

    if (node->right != NULL)
        PrintToken(node->right, output_file_ptr);
    else
        fprintf(output_file_ptr, " nil ");

    fprintf(output_file_ptr, " ) ");
}

void PrintFrontInOutputFile(Tree_t *tree) {

    assert(tree);
    FILE *output_file_ptr = fopen(DEFAULT_OUTPUT_FRONT_FILE_NAME, "w");

    PrintToken(tree->root, output_file_ptr);

    fclose(output_file_ptr);
}