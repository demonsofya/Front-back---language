#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../diff_struct.h"
#include "lib/onegin/buffer.h"
#include "lib/tree/diff_dump.h"
#include "get_tokens.h"

#define SYNTAX_ERROR SyntaxError(__FILE__, __FUNCTION__, __LINE__)

Node_t **GetTokensArray(char **curr_symbol, int *tokens_count) {

    assert(curr_symbol);
    assert(tokens_count);

    int curr_node_num = 0;
    Node_t **tokens_array = (Node_t **) calloc(curr_node_num + 1, sizeof(Node_t *));

    while (**curr_symbol != '\0') {
        SkipSpaces(curr_symbol);
ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "GETTING NEW LEXEME"));

        if (**curr_symbol == '\0')
            return tokens_array;

        if (isdigit(**curr_symbol) || (isdigit(*(*curr_symbol + 1)) && (**curr_symbol) == '-')) {
            tokens_array = (Node_t **) realloc(tokens_array, (curr_node_num + 1) * sizeof(Node_t *));
            tokens_array[curr_node_num++] = GetNumberToken(curr_symbol);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[curr_node_num - 1]));
            continue;
        }

        if ('А' <= **curr_symbol && **curr_symbol <= 'Я') {
            tokens_array = (Node_t **) realloc(tokens_array, (curr_node_num + 1) * sizeof(Node_t *));
            tokens_array[curr_node_num++] = GetOperationToken(curr_symbol);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[curr_node_num - 1]));
            continue;
        }

        if (isalpha(**curr_symbol)) {
            tokens_array = (Node_t **) realloc(tokens_array, (curr_node_num + 1) * sizeof(Node_t *));
            tokens_array[curr_node_num++] = GetVariableToken(curr_symbol);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[curr_node_num - 1]));
            continue;
        }

        if (**curr_symbol == '(') {
            tokens_array = (Node_t **) realloc(tokens_array, (curr_node_num + 1) * sizeof(Node_t *));
            tokens_array[curr_node_num++] = CreateNewServiceNode(NULL, NULL, SERVICE_STRUCTURES_ARRAY[OpenBracketStructure]);
            
            (*curr_symbol)++;
ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[curr_node_num - 1]));
            continue;
        }

        if (**curr_symbol == ')') {
            tokens_array = (Node_t **) realloc(tokens_array, (curr_node_num + 1) * sizeof(Node_t *));
            tokens_array[curr_node_num++] = CreateNewServiceNode(NULL, NULL, SERVICE_STRUCTURES_ARRAY[CloseBracketStructure]);
            
            (*curr_symbol)++;
ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[curr_node_num - 1]));
            continue;
        }

        if (**curr_symbol == ',') {
            tokens_array = (Node_t **) realloc(tokens_array, (curr_node_num + 1) * sizeof(Node_t *));
            tokens_array[curr_node_num++] = CreateNewServiceNode(NULL, NULL, SERVICE_STRUCTURES_ARRAY[ComaStructure]);
            
            (*curr_symbol)++;
ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[curr_node_num - 1]));
            continue;
        }

        SYNTAX_ERROR;
        return tokens_array;
    }

    return tokens_array;
}

Node_t *GetNumberToken(char **curr_symbol) {
    
    assert(curr_symbol);
    
    double value = 0;
    int data_len = 0;

    sscanf(*curr_symbol, " %lf%n", &value, &data_len);
    (*curr_symbol) += data_len;

    return CreateNewNumNode(NULL, NULL, value);
}

Node_t *GetVariableToken(char **curr_symbol) {
    
    assert(curr_symbol);

    size_t first_data_len = 0;
    sscanf(*curr_symbol, " %*[A-Za-z]%n", &first_data_len);

ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "GET VARIABLE"));

    char *data = GetWord(curr_symbol, first_data_len);

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

    (*curr_symbol) += first_data_len;
    Node_t *new_node = CreateNewVariableNode(NULL, NULL, variables_count - 1);
    return new_node;
}

Node_t *GetOperationToken(char **curr_symbol) {

    assert(curr_symbol);

ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "GET OPERATION"));

    char *curr_word = GetRussianWord(curr_symbol);
    assert(curr_word);
    int curr_hash = CountStringHashDJB2(curr_word);

    for (int i = 0; i < OPERATIONS_COUNT; i++)
        if (curr_hash == OPERATIONS_ARRAY[i].operation_hash)
            if (strcmp(curr_word, OPERATIONS_ARRAY[i].operation_name) == 0) 
                return CreateNewOperationNode(NULL, NULL, OPERATIONS_ARRAY[i]);
            
    for (int i = 0; i < COMPARISON_OPERATORS_COUNT; i++)
        if (curr_hash == COMPARISON_OPERATORS_ARRAY[i].comparison_operator_hash)
            if (strcmp(curr_word, COMPARISON_OPERATORS_ARRAY[i].comparison_operator_name) == 0)
                return CreateNewComparisonOperatorNode(NULL, NULL, COMPARISON_OPERATORS_ARRAY[i]);
            

    for (int i = 0; i < SERVICE_STRUCTURES_COUNT; i++)
        if (curr_hash == SERVICE_STRUCTURES_ARRAY[i].service_structure_hash)
            if (strcmp(curr_word, SERVICE_STRUCTURES_ARRAY[i].service_structure_name) == 0)
                return CreateNewServiceNode(NULL, NULL, SERVICE_STRUCTURES_ARRAY[i]);

    for (int i = 0; i < CONTROL_STRUCTURES_COUNT; i++) 
        if (curr_hash == CONTROL_STRUCTURES_ARRAY[i].control_structure_hash) 
            if (strcmp(curr_word, CONTROL_STRUCTURES_ARRAY[i].control_structure_name) == 0)
                return CreateNewControlNode(NULL, NULL, CONTROL_STRUCTURES_ARRAY[i]);

    for (int i = 0; i < LOGICAL_OPERATORS_COUNT; i++) 
        if (curr_hash == LOGICAL_OPERATORS_ARRAY[i].logical_operator_hash) 
            if (strcmp(curr_word, LOGICAL_OPERATORS_ARRAY[i].logical_operator_name) == 0)
                return CreateNewLogicalOperatorNode(NULL, NULL, LOGICAL_OPERATORS_ARRAY[i]);

    variables_count++;
    variables_array = (Variable_t *) realloc(variables_array, sizeof(Variable_t) * variables_count);
    variables_array[variables_count - 1] = {curr_word, variables_count - 1, curr_hash, 0};      

    return CreateNewVariableNode(NULL, NULL, variables_count - 1);

}

char *GetRussianWord(char **data) {

    assert(data);

ON_DEBUG(Print_Curr_Input_Information(*data, "GET RUSSIAN WORD"));

    int data_len = 0;

ON_DEBUG(fprintf(stderr, "Сейчас буква %d, A это %d, Я это %d\n", (*data)[data_len], 'А', 'Я'));

    SkipSpaces(data);
    while('А' <= (*data)[data_len] && (*data)[data_len] <= 'Я') 
        data_len++;

ON_DEBUG(Print_Curr_Input_Information(&(*data)[data_len], "AFTER GETTING RUSSIAN WORD"));
ON_DEBUG(fprintf(stderr, "Word len is %d\n", data_len));

    char *word = (char *) calloc(data_len, sizeof(char));
    assert(word);

    char format[MAX_WORD_FORMAT_SIZE] = "";
    snprintf(format, MAX_WORD_FORMAT_SIZE, "%%%ds", data_len);
    sscanf(*data, format, word);
    //sscanf_s(data, "%s", word, *data_len);
    (*data) += data_len;

ON_DEBUG(fprintf(stderr, "Word is %s\n", word));
    return word;
}

char *GetWord(char **curr_symbol, size_t data_len) {

    assert(curr_symbol);

    char *data_string = (char *) calloc(data_len, sizeof(char));

ON_DEBUG(Print_Curr_Input_Information(*curr_symbol, "GET WORD"));
    
    if (!sscanf(*curr_symbol, " %[A-Za-z]", data_string)) {
        SYNTAX_ERROR;
        return NULL;
    } 
    
ON_DEBUG(fprintf(stderr, "Curr word is %s\n\n", data_string));
    //(*curr_symbol) += data_len;

    return data_string;
}


void SyntaxError(const char *file_name, const char *function_name, int line_number) {

    assert(file_name);
    assert(function_name);

    fprintf(stderr, "SYNTAX ERROR from %s:%d:0 at %s:\n\n", file_name, line_number, function_name);
    return;
}

void SkipSpaces(char **curr_pos) {

    assert(curr_pos);

    while (isspace(**curr_pos) && **curr_pos != '\0')
        (*curr_pos)++;

}