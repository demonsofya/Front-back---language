#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../diff_struct.h"
#include "lib/tree/diff_dump.h"
#include "semantical_analize.h"

#define SYNTAX_ERROR { fprintf(stderr, "SYNTAX ERROR from " __FILE__ ":%d:0 at %s\n\n", __LINE__, __FUNCTION__); assert(NULL); }

void PutVariablesPosesInNametableInTree(Tree_t *tokens_graph) {

    assert(tokens_graph);

    PrintFunctionVariablesPosesInNametable(tokens_graph->root);
}

void PrintFunctionVariablesPosesInNametable(Node_t *node) {

    if (node->type == NewFuncType) {
        PrintFunctionVariablesPosesInNametable(node->left);
        PrintFunctionVariablesPosesInNametable(node->right);

        return;
    }

    PutVariableNodePosInNametable(node->left, functions_array[node->value.function_index_in_array]);
    if (functions_array[node->value.function_index_in_array]->params_count > 0)
        PutVariableNodePosInNametable(node->right, functions_array[node->value.function_index_in_array]);
}

void PutVariableNodePosInNametable(Node_t *node, Function_t *curr_function_struct) {

    assert(node);

    if (node->type != VariableType) {
        if (node->left != NULL)
            PutVariableNodePosInNametable(node->left, curr_function_struct);

        if (node->right != NULL)
            PutVariableNodePosInNametable(node->right, curr_function_struct);
        
        return;
    }

    for (int i = 0; i < curr_function_struct->variables_count; i++)
        if (curr_function_struct->variables_table[i].variable_hash == GET_VARIABLE_HASH(node))
            if (strcmp(curr_function_struct->variables_table[i].variable_name, GET_VARIABLE_NAME(node)) == 0) {
                node->value.variable_index_in_array = i;
                return;
            }

    for (int i = 0; i < functions_count; i++)
        if (GET_VARIABLE_HASH(node) == functions_array[i]->function_hash)
            if (strcmp(GET_VARIABLE_NAME(node), functions_array[i]->function_name) == 0) {
                node->value.function_index_in_array = i;
                node->type = FunctionType;

                return;
            }

    SYNTAX_ERROR;
}



