#ifndef ASSEMBLING_H_INCLUDED
#define ASSEMBLING_H_INCLUDED

#include <stdio.h>
#include "../diff_struct.h"

#define GET_FUNCTION_NAME(node) functions_array[node->value.function_index_in_array]->function_name
#define GET_FUNCTION_PARAMS_COUNT(node) functions_array[node->value.function_index_in_array]->params_count
#define GET_FUNCTION_VARIABLES_COUNT(node) functions_array[node->value.function_index_in_array]->variables_count
#define GET_FUNCTION_VARIABLE_TABLE(node) functions_array[node->value.function_index_in_array]->variables_table

#define GET_CURR_VARIABLE_NUM(node) curr_function_struct[node->value.variable_index_in_array]

void CreateAsmFile(Tree_t *tokens_graph);

void PrintAsmHeader(FILE *asm_file_ptr);
void PrintAsmEnd(FILE *asm_file_ptr);

void PrintAsmFunc(FILE *asm_file_ptr, Node_t *node);
void PrintAsmNode(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct);

void PrintFunctionCallAsm(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct);

void PrintInAsm(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct);
void PrintAsmEqualization(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct);
void PutVariableNumberInBX(FILE *asm_file_ptr, Node_t* node, Function_t *curr_function_struct);
void PrintReturnFromAsmFunc(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct);
void PrintOutAsm(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct);

void PrintPushAsmVariable(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct);

void PrintAsmOperation(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct);

void PrintAsmComparison(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct);

void PrintIfStructure(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct);
void PrintWhileStructure(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct);

#endif