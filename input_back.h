#ifndef INPUT_BACK_H_INCLUDED
#define INPUT_BACK_H_INCLUDED

#include "../diff_struct.h"

Tree_t *GetBackendBaseFromFile();
Node_t *ReadCurrNode(char **curr_symbol);

void SkipSpaces(char **curr_pos);

Node_t *CheckNodeTypeAndFillValue(char *curr_data);
Node_t *GetVariableNode(char **curr_symbol);
Node_t *GetFunctionNode(char **curr_symbol);

Function_t *CreateNewFunction(char *func_name, int func_num, int func_hash);

#endif