#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "lib/tree/diff_dump.h"
#include "../diff_struct.h"
#include "lib/onegin/buffer.h"
#include "input_back.h"
#include "math.h"

Variable_t *variables_array = (Variable_t *) calloc(1, sizeof(Variable_t) * 1);
unsigned int variables_count = 0;

Function_t **functions_array = (Function_t **) calloc(1, sizeof(Function_t*) * 1);
unsigned int curr_function_num = -1;

unsigned int functions_count = 0;

const char *const DEFAULT_INPUT_FILE_NAME = "lang_front_output.txt";
const int MAX_COMMAND_SIZE = 100;

#define SYNTAX_ERROR { fprintf(stderr, "SYNTAX ERROR from " __FILE__ ":%d:0 at %s\n\n", __LINE__, __FUNCTION__); assert(NULL); }


Tree_t *GetBackendBaseFromFile() {

    size_t buffer_size = 0;
    char *buffer = GetBuffer(DEFAULT_INPUT_FILE_NAME, &buffer_size);

    if (buffer == NULL) {
        printf("Input file error\n");
        return NULL;
    }

ON_DEBUG(buffer_begin = buffer);

    Tree_t *tree = (Tree_t *) calloc(1, sizeof(Tree_t));
    tree->root = ReadCurrNode(&buffer);

    return tree;
}

void SkipSpaces(char **curr_pos) {

    assert(curr_pos);

    while (isspace(**curr_pos) && **curr_pos != '\0')
        (*curr_pos)++;

ON_DEBUG(Print_Curr_Input_Information(*curr_pos, "Dump after skipping spaces"));

}

Node_t *ReadCurrNode(char **curr_symbol) {

    assert(curr_symbol);
    SkipSpaces(curr_symbol);

ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "Dump in the begin of recursion"));

    if (**curr_symbol == '(') {

ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "Dump in the begin of reading new node"));

        Node_t *new_node = (Node_t *) calloc(1, sizeof(Node_t));
        (*curr_symbol)++;

        size_t data_len = 0;
        sscanf(*curr_symbol, " \"%*[^\"]\"%n", &data_len);

        char *curr_data = (char *) calloc(data_len, sizeof(char));
        sscanf(*curr_symbol, " \"%[^\"]", curr_data);
ON_DEBUG(fprintf(stderr, "curr data is %s\n", curr_data));
ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "Dump ufter skipping ("));        
        new_node = CheckNodeTypeAndFillValue(curr_data);

        (*curr_symbol) += data_len;

ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "Tree dump after creating new node without sons"));
ON_DEBUG(TreeDumpCreateGraphFile(new_node));
ON_DEBUG(TreeDumpCreateGraphFile(new_node));
        new_node->left = ReadCurrNode(curr_symbol);
ON_DEBUG(TreeDumpCreateGraphFile(new_node));
        new_node->right = ReadCurrNode(curr_symbol);
ON_DEBUG(TreeDumpCreateGraphFile(new_node));
        SkipSpaces(curr_symbol);

        if (**curr_symbol != ')') {
            printf("Read from input file error: SYNTAX ERROR\n");
            return NULL;
        }
        (*curr_symbol)++;

ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "Dump after reading new node"));
ON_DEBUG(TreeDumpCreateGraphFile(new_node));

        return new_node;
    } else {
ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "Dump before nil"));

        if (strncmp(*curr_symbol, "nil", strlen("nil")) == 0)
            *curr_symbol += strlen("nil");
        else
            printf("Read from input file error: SYNTAX ERROR\n");

ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "Dump after nil"));

        return NULL;
    }
    return NULL;
}

Node_t *CheckNodeTypeAndFillValue(char *curr_data) {

    assert(curr_data);

    if (strncmp(curr_data, "OP", strlen("OP")) == 0) {
        curr_data += strlen("OP");
        SkipSpaces(&curr_data);

        for (int i = 0; i < OPERATIONS_COUNT; i++) 
            if (strcmp(curr_data, OPERATIONS_ARRAY[i].operation_back_standard_name) == 0)
                return CreateNewOperationNode(NULL, NULL, OPERATIONS_ARRAY[i]);

        SYNTAX_ERROR;
    }

ON_DEBUG(fprintf(stderr, "Not OP\n"));

    if (strncmp(curr_data, "VAR", strlen("VAR")) == 0) {
        curr_data += strlen("VAR");
        SkipSpaces(&curr_data);

        return GetVariableNode(&curr_data);
    }

ON_DEBUG(fprintf(stderr, "Not VAR\n"));

    if (strncmp(curr_data, "NUM", strlen("NUM")) == 0) {
        curr_data += strlen("NUM");
        SkipSpaces(&curr_data);

        double curr_num = 0;
        sscanf(curr_data, "%lf", &curr_num);

        return CreateNewNumNode(NULL, NULL, curr_num);
    }

ON_DEBUG(fprintf(stderr, "Not NUM\n"));

    if (strncmp(curr_data, "SERV", strlen("SERV")) == 0) {
        curr_data += strlen("SERV");
        SkipSpaces(&curr_data);

        for (int i = 0; i < SERVICE_STRUCTURES_COUNT; i++) 
            if (strcmp(curr_data, SERVICE_STRUCTURES_ARRAY[i].service_back_standard_name) == 0)

                return CreateNewServiceNode(NULL, NULL, SERVICE_STRUCTURES_ARRAY[i]);

        SYNTAX_ERROR;
    }

ON_DEBUG(fprintf(stderr, "Not SERV\n"));

    if (strncmp(curr_data, "COMP", strlen("COMP")) == 0) {
        curr_data += strlen("COMP");
        SkipSpaces(&curr_data);
        
        for (int i = 0; i < COMPARISON_OPERATORS_COUNT; i++) 
            if (strcmp(curr_data, COMPARISON_OPERATORS_ARRAY[i].comparison_back_standard_name) == 0)

                return CreateNewComparisonOperatorNode(NULL, NULL, COMPARISON_OPERATORS_ARRAY[i]);

        SYNTAX_ERROR;
    }
    
ON_DEBUG(fprintf(stderr, "Not COMP\n"));

    if (strncmp(curr_data, "CONTROL", strlen("CONTROL")) == 0) {
        curr_data += strlen("CONTROL");
        SkipSpaces(&curr_data);

        for (int i = 0; i < CONTROL_STRUCTURES_COUNT; i++)
            if (strcmp(curr_data, CONTROL_STRUCTURES_ARRAY[i].control_back_standard_name) == 0) 

                return CreateNewControlNode(NULL, NULL, CONTROL_STRUCTURES_ARRAY[i]);

        SYNTAX_ERROR;
    }

    if (strncmp(curr_data, ";", strlen(";")) == 0)
        return CreateEmptyNode();

    if (strncmp(curr_data, "CALL", strlen("CALL")) == 0)
        return CreateFuncCallNode();

    if (strncmp(curr_data, "NEWFUNC", strlen("NEWFUNC")) == 0)
        return CreateNewFuncNode();

    if (strncmp(curr_data, "FUNC", strlen("FUNC")) == 0) {
        curr_data += strlen("VAR");
        SkipSpaces(&curr_data);

        return GetFunctionNode(&curr_data);
    }

    SYNTAX_ERROR;
}

Node_t *GetVariableNode(char **curr_symbol) {
    
    assert(curr_symbol);

    size_t first_data_len = 0;
    sscanf(*curr_symbol, " %*[A-Za-z]%n", &first_data_len);

ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "GET VARIABLE"));
    
    char *data = (char *) calloc(first_data_len, sizeof(char));
    
    if (!sscanf(*curr_symbol, " %[A-Za-z]", data)) {
        SYNTAX_ERROR;
        return NULL;
    } 

    size_t second_data_len = 0;
    sscanf((*curr_symbol) + first_data_len, "%*[A-Za-z0-9]%n", &second_data_len);

    if (second_data_len != 0) {
        data = (char *) realloc(data, sizeof(char) * (first_data_len + second_data_len));
        sscanf((*curr_symbol) + first_data_len, "%[A-Za-z0-9]", data + first_data_len);
    }

    int variable_hash = CountStringHashDJB2(data);
    for (int i = 0; i < variables_count; i++)
        if (variable_hash == variables_array[i].variable_hash)
            if (strcmp(data, variables_array[i].variable_name) == 0) {
                Node_t *new_node = CreateNewVariableNode(NULL, NULL, i);

                (*curr_symbol) += first_data_len + second_data_len;
                return new_node;
            }

    variables_count++;
    variables_array = (Variable_t *) realloc(variables_array, sizeof(Variable_t) * variables_count);
    variables_array[variables_count - 1] = {data, variables_count - 1, variable_hash, 0};      

    return CreateNewVariableNode(NULL, NULL, variables_count - 1);
}

Node_t *GetFunctionNode(char **curr_symbol) {

    assert(curr_symbol);

    size_t first_data_len = 0;
    sscanf(*curr_symbol, " %*[A-Za-z]%n", &first_data_len);

ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "GET FUNCTION"));
    
    char *data = (char *) calloc(first_data_len, sizeof(char));
    
    if (!sscanf(*curr_symbol, " %[A-Za-z]", data)) {
        SYNTAX_ERROR;
        return NULL;
    } 

    size_t second_data_len = 0;
    sscanf((*curr_symbol) + first_data_len, "%*[A-Za-z0-9]%n", &second_data_len);

    if (second_data_len != 0) {
        data = (char *) realloc(data, sizeof(char) * (first_data_len + second_data_len));
        sscanf((*curr_symbol) + first_data_len, "%[A-Za-z0-9]", data + first_data_len);
    }

    int function_hash = CountStringHashDJB2(data);
    for (int i = 0; i < functions_count; i++)
        if (function_hash == functions_array[i]->function_hash)
            if (strcmp(data, functions_array[i]->function_name) == 0) 
                return CreateFunctionNode(NULL, NULL, i);

    functions_count++;
    functions_array = (Function_t **) realloc(functions_array, sizeof(Function_t*) * functions_count);
    functions_array[functions_count - 1] = CreateNewFunction(data, functions_count - 1, function_hash);

    (*curr_symbol) += first_data_len;
    Node_t *new_node = CreateNewVariableNode(NULL, NULL, variables_count - 1);
    
    CreateFunctionNode(NULL, NULL, functions_count - 1);
}

Function_t *CreateNewFunction(char *func_name, int func_num, int func_hash) {

    Function_t *new_function = (Function_t *) calloc(1, sizeof(Function_t));

    new_function->function_name = func_name;
    new_function->function_num = func_num;
    new_function->variables_table = (Variable_t *) calloc(1, sizeof(Variable_t));
    new_function->function_hash = func_hash;

    return new_function;
}