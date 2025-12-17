#ifndef FONT_OUTPUT_H_INCLUDED
#define FONT_OUTPUT_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#include "../diff_struct.h"

#define GET_OP_STANDARD_NAME(node) node->value.operation_type_value.operation_back_standard_name
#define GET_SERV_STANDARD_NAME(node) node->value.service_type_value.service_back_standard_name
#define GET_COMPARISON_STANDARD_NAME(node) node->value.comparison_operator_type_value.comparison_back_standard_name
#define GET_CONTROL_STANDARD_NAME(node) node->value.control_structure_type_value.control_back_standard_name
#define GET_FUNCTION_NAME(node) functions_array[node->value.function_index_in_array]->function_name

void PrintFrontInOutputFile(Tree_t *tree);
void PrintToken(Node_t *node, FILE *output_file_ptr);

#endif