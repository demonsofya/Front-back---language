#ifndef MIDDLE_END_H_INCLUDED
#define MIDDLE_END_H_INCLUDED

#include "../diff_struct.h"

void PutVariablesPosesInNametableInTree(Tree_t *tokens_graph);
void PrintFunctionVariablesPosesInNametable(Node_t *node);
void PutVariableNodePosInNametable(Node_t *node, Function_t *curr_function_struct);

#endif