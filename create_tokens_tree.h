#ifndef READIN_TOKENS_H_INCLUDED
#define READIN_TOKENS_H_INCLUDED

#include "lib/tree/diff_struct.h"

#define GET_CURR_VARIABLES_TABLE functions_array[curr_function_num]->variables_table
#define GET_CURR_VARIABLES_COUNT functions_array[curr_function_num]->variables_count
#define GET_CURR_PARAMS_COUNT    functions_array[curr_function_num]->params_count

Tree_t *GetTokensGraph();

Function_t *CreateNewFunction(Node_t *node);
void CreateNewFunctionParam(Node_t **tokens_array, int *curr_token_num);
Node_t *GetFunctionParams(Node_t **tokens_array, int *curr_token_num);

Node_t *GetFunctionNode(Node_t **tokens_array, int *curr_token_num);
Node_t *GetFunctionBody(Node_t **tokens_array, int *curr_token_num);
Node_t *GetFunctionElement(Node_t **tokens_array, int *curr_token_num);

Node_t *GetIfOperator(Node_t **tokens_array, int *curr_token_num);
Node_t *GetWhileCycle(Node_t **tokens_array, int *curr_token_num);
Node_t *GetOperator(Node_t **tokens_array, int *curr_token_num);
Node_t *GetUserFunction(Node_t **tokens_array, int *curr_token_num);
Node_t *GetNewVariable(Node_t **tokens_array, int *curr_token_num);
Node_t *GetIn(Node_t **tokens_array, int *curr_token_num);
Node_t *GetReturn(Node_t **tokens_array, int *curr_token_num);
Node_t *GetPrint(Node_t **tokens_array, int *curr_token_num);

Node_t *GetLogicalOperator(Node_t **tokens_array, int *curr_token_num);
Node_t *GetComparisonOperator(Node_t **tokens_array, int *curr_token_num);
Node_t *GetSubstractionOrAddictionOperation(Node_t **tokens_array, int *curr_token_num);
Node_t *GetMultiplicationOrDivisionOperation(Node_t **tokens_array, int *curr_token_num);
Node_t *GetPowOperation(Node_t **tokens_array, int *curr_token_num);
Node_t *GetBracket(Node_t **tokens_array, int *curr_token_num);

#endif
