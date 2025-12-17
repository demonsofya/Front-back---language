#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//#include "reading.h"
#include "lib/tree/diff_struct.h"
#include "lib/onegin/buffer.h"
#include "lib/tree/diff_dump.h"
#include "get_tokens.h"
#include "create_tokens_tree.h"

#define SYNTAX_ERROR { fprintf(stderr, "SYNTAX ERROR from " __FILE__ ":%d:0 at %s\n\n", __LINE__, __FUNCTION__); assert(NULL); }

Variable_t *variables_array = (Variable_t *) calloc(1, sizeof(Variable_t) * 1);
unsigned int variables_count = 0;

Function_t **functions_array = (Function_t **) calloc(1, sizeof(Function_t*) * 1);
unsigned int curr_function_num = -1;

unsigned int functions_count = 0;


#define INPUT_FILE_FOR_TREE "input.txt"


// На данном этапе программы ФУНКЦИЯ и ПЕРЕМЕННАЯ это одно и то же
// Поэтому в GetBrackets() я проверяю на а) скобки б) число в) переменную. То есть, я считаю что конечная лексическая единица - это функция, переменная, число

Tree_t *GetTokensGraph() {

    size_t buffer_size = 0; 
    char *buffer = GetBuffer(INPUT_FILE_FOR_TREE, &buffer_size);

//ON_DEBUG(buffer_begin = buffer);
//ON_DEBUG(fprintf(stderr, "%s", buffer));

    int node_array_size = 0;
    Node_t **node_array = GetTokensArray(&buffer, &node_array_size);
    assert(node_array);
    int curr_token_num = 0;

//ON_DEBUG(TreeDumpCreateGraphFile(node_array[curr_token_num]));
//ON_DEBUG(for(int i = 0; i < variables_count; i++) fprintf(stderr, "variable num %d is %s\n", i, variables_array[i].variable_name));

    if (!CHECK_IF_PROGRAM_BEGIN(node_array[curr_token_num]))
        SYNTAX_ERROR;
    curr_token_num++;

//ON_DEBUG(TreeDumpCreateGraphFile(node_array[curr_token_num]));

    Node_t *root = GetFunctionNode(node_array, &curr_token_num);
//ON_DEBUG(TreeDumpCreateGraphFile(root));

//ON_DEBUG(TreeDumpCreateGraphFile(node_array[curr_token_num]));

    while (CHECK_IF_NEW_FUNCTION(node_array[curr_token_num])) {
        Node_t *new_node = CreateNewFuncNode();
        new_node->left = root;
        new_node->right = GetFunctionNode(node_array, &curr_token_num);

        root = new_node;
    }

//ON_DEBUG(TreeDumpCreateGraphFile(node_array[curr_token_num]));

    if (!CHECK_IF_PROGRAM_END(node_array[curr_token_num]))
        SYNTAX_ERROR;
    curr_token_num++;

    Tree_t *tree = (Tree_t *) calloc(1, sizeof(Tree_t));
    tree->root = root;

    functions_count = curr_function_num + 1;
ON_DEBUG( for (int i = 0; i <= curr_function_num; i++) { fprintf(stderr, "\nCurr function %s num = %d, variables count is %d function array is\n", functions_array[i]->function_name, i, functions_array[i]->variables_count); for (int j = 0; j < functions_array[i]->variables_count; j++) {
fprintf(stderr, "Name is %s num is %d\n", functions_array[i]->variables_table[j].variable_name, functions_array[i]->variables_table[j].variable_num);}});
ON_DEBUG(fprintf(stderr, "\nfunctions count is %d\n", functions_count));
    return tree;
}

Node_t *GetFunctionNode(Node_t **tokens_array, int *curr_token_num) {

    assert(tokens_array);
    assert(curr_token_num);

    if (!CHECK_IF_NEW_FUNCTION(tokens_array[*curr_token_num]) || tokens_array[(*curr_token_num) + 1]->type != VariableType)
        SYNTAX_ERROR;
    
    curr_function_num++;
    value_t new_value = {};
    new_value.function_index_in_array = curr_function_num;

    Node_t *node = tokens_array[(*curr_token_num) + 1];
    (*curr_token_num)+= 2;

    functions_array = (Function_t **) realloc(functions_array, sizeof(Function_t *) * (curr_function_num + 1));
    functions_array[curr_function_num] = CreateNewFunction(node);

    node->right = GetFunctionParams(tokens_array, curr_token_num);
    node->left = GetFunctionBody(tokens_array, curr_token_num);

    node->type = FunctionType;
    node->value = new_value;

    return node;
}

Function_t *CreateNewFunction(Node_t *node) {

    assert(node);

    Function_t *new_function = (Function_t *) calloc(1, sizeof(Function_t));

    new_function->function_name = GET_VARIABLE_NAME(node);
    new_function->function_num = curr_function_num;
    new_function->variables_table = (Variable_t *) calloc(1, sizeof(Variable_t));
    new_function->function_hash = GET_VARIABLE_HASH(node);

    return new_function;
}

Node_t *GetFunctionParams(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

    int params_count = 0;

    if (tokens_array[*curr_token_num]->type != VariableType)
        return NULL;

    Node_t *node = tokens_array[*curr_token_num];
    CreateNewFunctionParam(tokens_array, curr_token_num);
    
    (*curr_token_num)++;

    while (tokens_array[*curr_token_num]->type == VariableType) {
        Node_t *new_node = CreateEmptyNode();
        new_node->left = node;
        new_node->right = tokens_array[*curr_token_num];

        CreateNewFunctionParam(tokens_array, curr_token_num);
        (*curr_token_num)++;
            
        node = new_node;
    }

    return node;
}

void CreateNewFunctionParam(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

    GET_CURR_PARAMS_COUNT += 1;
    GET_CURR_VARIABLES_COUNT += 1;
    GET_CURR_VARIABLES_TABLE = (Variable_t *) realloc(GET_CURR_VARIABLES_TABLE, sizeof(Variable_t) * GET_CURR_VARIABLES_COUNT);

    GET_CURR_VARIABLES_TABLE[GET_CURR_VARIABLES_COUNT - 1] = variables_array[tokens_array[*curr_token_num]->value.variable_index_in_array]; 
    GET_CURR_VARIABLES_TABLE[GET_CURR_VARIABLES_COUNT - 1].variable_name = GET_VARIABLE_NAME(tokens_array[*curr_token_num]);
    GET_CURR_VARIABLES_TABLE[GET_CURR_VARIABLES_COUNT - 1].variable_num = GET_CURR_VARIABLES_COUNT - 1;
//ON_DEBUG(fprintf(stderr, "Pos is %d Name is %s num is %d\n", GET_CURR_VARIABLES_COUNT - 1, GET_CURR_VARIABLES_TABLE[GET_CURR_VARIABLES_COUNT - 1].variable_name, GET_CURR_VARIABLES_TABLE[GET_CURR_VARIABLES_COUNT - 1].variable_num));
}

Node_t *GetFunctionBody(Node_t **tokens_array, int *curr_token_num) {

    assert(tokens_array);
    assert(curr_token_num);

//ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

    while(GET_SERVICE_STRUCTURE_TYPE(tokens_array[*curr_token_num]) == BodyBeginStructure && tokens_array[*curr_token_num]->type == ServiceType) {
        (*curr_token_num)++;

        Node_t *curr_node = GetFunctionElement(tokens_array, curr_token_num);
        Node_t *new_node = NULL;

        while (GET_SERVICE_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != BodyBeginStructure) {
           
            new_node = GetFunctionElement(tokens_array, curr_token_num);
            if (new_node == NULL)
                SYNTAX_ERROR;

            Node_t *new_empty_node = CreateEmptyNode();
            new_empty_node->left = curr_node;
            new_empty_node->right = new_node;

            curr_node = new_empty_node;

ON_DEBUG(TreeDumpCreateGraphFile(curr_node));
        }
ON_DEBUG(TreeDumpCreateGraphFile(curr_node));

        if (GET_SERVICE_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != BodyBeginStructure)
            SYNTAX_ERROR;
        (*curr_token_num)++;

        return curr_node;// Пока функцию оно воспринимает только одну
    }

    return NULL;
}

Node_t *GetFunctionElement(Node_t **tokens_array, int *curr_token_num) {

    assert(tokens_array);
    assert(curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));
ON_DEBUG(fprintf(stderr, "Getting body\n"));

    if (tokens_array[*curr_token_num]->type == ControlStructureType && GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) == IfOperator) 
        return GetIfOperator(tokens_array, curr_token_num);

    if (tokens_array[*curr_token_num]->type == ControlStructureType && GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) == BeginWhileCycle)
        return GetWhileCycle(tokens_array, curr_token_num);

    if (tokens_array[(*curr_token_num) + 1]->type == ControlStructureType && GET_CONTROL_STRUCTURE_TYPE(tokens_array[(*curr_token_num) + 1]) == EqualizationOperator)
        return GetOperator(tokens_array, curr_token_num);

    if (tokens_array[*curr_token_num]->type == ControlStructureType && GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) == DeclarationOperator)
        return GetNewVariable(tokens_array, curr_token_num);

    if (tokens_array[*curr_token_num]->type == ControlStructureType && GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) == InVariableOperator)
        return GetIn(tokens_array, curr_token_num);

    if (tokens_array[*curr_token_num]->type == ControlStructureType && GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) == ReturnOperator)
        return GetReturn(tokens_array, curr_token_num);

    if (tokens_array[*curr_token_num]->type == ControlStructureType && GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) == PrintOperator)
        return GetPrint(tokens_array, curr_token_num);    

    return GetUserFunction(tokens_array, curr_token_num);
}

Node_t *GetPrint(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

    if (GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != PrintOperator)
        SYNTAX_ERROR;
    (*curr_token_num)++;

    Node_t *node = CreateNewControlNode(NULL, NULL, CONTROL_STRUCTURES_ARRAY[PrintOperator]);
    node->left = GetLogicalOperator(tokens_array, curr_token_num);

    return node;
}

Node_t *GetReturn(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

    if (GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != ReturnOperator)
        SYNTAX_ERROR;
    (*curr_token_num)++;

    Node_t *node = CreateNewControlNode(NULL, NULL, CONTROL_STRUCTURES_ARRAY[ReturnOperator]);
    node->left = GetLogicalOperator(tokens_array, curr_token_num);
ON_DEBUG(TreeDumpCreateGraphFile(node));
    return node;
}

Node_t *GetIn(Node_t **tokens_array, int *curr_token_num) {

    assert(tokens_array);
    assert(curr_token_num);

    if (GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != InVariableOperator || tokens_array[(*curr_token_num) + 1]->type != VariableType)
        SYNTAX_ERROR;
    (*curr_token_num)++;

    Node_t *node = CreateNewControlNode(NULL, NULL, CONTROL_STRUCTURES_ARRAY[InVariableOperator]);
    node->left = tokens_array[*curr_token_num];
    (*curr_token_num)++;

ON_DEBUG(TreeDumpCreateGraphFile(node));
ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

    return node;
}

Node_t *GetNewVariable(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

    if (GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != DeclarationOperator || tokens_array[(*curr_token_num) + 1]->type != VariableType)
        SYNTAX_ERROR;   
    (*curr_token_num)++;

    GET_CURR_VARIABLES_COUNT += 1;
    GET_CURR_VARIABLES_TABLE = (Variable_t *) realloc(GET_CURR_VARIABLES_TABLE, sizeof(Variable_t) * GET_CURR_VARIABLES_COUNT);

    GET_CURR_VARIABLES_TABLE[GET_CURR_VARIABLES_COUNT - 1] = variables_array[tokens_array[*curr_token_num]->value.variable_index_in_array];
    GET_CURR_VARIABLES_TABLE[GET_CURR_VARIABLES_COUNT - 1].variable_num = GET_CURR_VARIABLES_COUNT - 1;
/*
    name_table[functions_count - 1].variables_count++;
    name_table[functions_count - 1].variables_table = (Variable_t *) realloc(name_table[functions_count - 1].variables_table, sizeof(Variable_t) * name_table[functions_count - 1].variables_count);

    name_table[functions_count - 1].variables_table[name_table[functions_count - 1].variables_count - 1] = variables_array[tokens_array[*curr_token_num]->value.variable_index_in_array];
    name_table[functions_count - 1].variables_table[name_table[functions_count - 1].variables_count - 1].variable_num = name_table[functions_count - 1].variables_count - 1;   
 */   
    return GetOperator(tokens_array, curr_token_num);
}

Node_t *GetIfOperator(Node_t **tokens_array, int *curr_token_num) {

    assert(tokens_array);
    assert(curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));
ON_DEBUG(fprintf(stderr, "Getting if\n"));

    if (GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != IfOperator)
        SYNTAX_ERROR;
    
    Node_t *curr_node = tokens_array[*curr_token_num];
    (*curr_token_num)++;

    curr_node->left = GetLogicalOperator(tokens_array, curr_token_num);
    curr_node->right = GetFunctionElement(tokens_array, curr_token_num);

    while (GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != IfElseOperator && GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != EndIfOperator) {
        Node_t *new_node = CreateEmptyNode();
        new_node->left = curr_node->right;
        new_node->right = GetFunctionElement(tokens_array, curr_token_num);

        curr_node->right = new_node;
    }

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

    if (GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) == IfElseOperator) {
        Node_t *new_node = tokens_array[*curr_token_num];
        (*curr_token_num)++;

        new_node->left = curr_node->right;
        new_node->right = GetFunctionElement(tokens_array, curr_token_num);

        curr_node->right = new_node;

        while (GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != EndIfOperator) {
            Node_t *new_node = CreateEmptyNode();
            new_node->left = curr_node->right->right;
            new_node->right = GetFunctionElement(tokens_array, curr_token_num);

            curr_node->right->right = new_node;
        }
    }

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

    if (GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != EndIfOperator)
        SYNTAX_ERROR;
    (*curr_token_num)++;

ON_DEBUG(TreeDumpCreateGraphFile(curr_node));

    return curr_node;
}

Node_t *GetWhileCycle(Node_t **tokens_array, int *curr_token_num) {
  
    assert(tokens_array);
    assert(curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));
ON_DEBUG(fprintf(stderr, "Getting while\n"));

    if (tokens_array[*curr_token_num]->value.control_structure_type_value.control_structure_type != BeginWhileCycle) 
        return NULL;

    Node_t *curr_node = tokens_array[*curr_token_num];
    (*curr_token_num)++;
 
ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));   

    curr_node->left = GetLogicalOperator(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

    curr_node->right = GetFunctionElement(tokens_array, curr_token_num);

    while (GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != EndWhileCycle) {
        Node_t *new_node = CreateEmptyNode();
        new_node->left = curr_node->right;
        new_node->right = GetFunctionElement(tokens_array, curr_token_num);

        curr_node->right = new_node;
    }

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

    if (tokens_array[*curr_token_num]->value.control_structure_type_value.control_structure_type != EndWhileCycle)
        SYNTAX_ERROR;
    (*curr_token_num)++;

ON_DEBUG(TreeDumpCreateGraphFile(curr_node));

    return curr_node;
}

Node_t *GetOperator(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));
ON_DEBUG(fprintf(stderr, "Getting =\n"));

    if (tokens_array[*curr_token_num]->type != VariableType)
        SYNTAX_ERROR;

    Node_t *curr_right_node = tokens_array[*curr_token_num]; // у нас присваивание слева направо
    (*curr_token_num)++;

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

    if (GET_CONTROL_STRUCTURE_TYPE(tokens_array[*curr_token_num]) != EqualizationOperator || curr_right_node == NULL) 
        SYNTAX_ERROR;

    Node_t *curr_node = tokens_array[*curr_token_num];
    (*curr_token_num)++;

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));  

    curr_node->right = curr_right_node;
    curr_node->left = GetLogicalOperator(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(curr_node));

    return curr_node;
}

Node_t *GetLogicalOperator(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));
ON_DEBUG(fprintf(stderr, "Getting logical op\n"));

    Node_t *node = GetComparisonOperator(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(node));
ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

    while (tokens_array[*curr_token_num]->type == LogicalOperatorType) {
        
        Node_t *curr_node = tokens_array[*curr_token_num];
        (*curr_token_num)++;

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

        curr_node->left = CopyNode(node);
        curr_node->right = GetComparisonOperator(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

        free(node);
        node = curr_node;
    }

ON_DEBUG(TreeDumpCreateGraphFile(node));

    return node;
}

Node_t *GetComparisonOperator(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);
ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));
ON_DEBUG(fprintf(stderr, "Getting comparison\n"));

    Node_t *node = GetSubstractionOrAddictionOperation(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(node));

    while (tokens_array[*curr_token_num]->type == ComparisonOperatorType) {
        
        Node_t *curr_node = tokens_array[*curr_token_num];
        (*curr_token_num)++;

        curr_node->left = CopyNode(node);
        curr_node->right = GetSubstractionOrAddictionOperation(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

        free(node);
        node = curr_node;
    }

ON_DEBUG(TreeDumpCreateGraphFile(node));

    return node;
}

Node_t *GetSubstractionOrAddictionOperation(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));
ON_DEBUG(fprintf(stderr, "Getting + and -\n"));

    Node_t *node = GetMultiplicationOrDivisionOperation(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(node));

    while ((GET_OPERATION_TYPE(tokens_array[*curr_token_num]) == SubstractionOperation || GET_OPERATION_TYPE(tokens_array[*curr_token_num]) == AddictionOperation) 
            && tokens_array[*curr_token_num]->type == OperationType) {
        
        Node_t *curr_node = tokens_array[*curr_token_num];
        (*curr_token_num)++;
assert(node);
        curr_node->left = CopyNode(node);
ON_DEBUG(fprintf(stderr, "after getting node copy"));
        curr_node->right = GetMultiplicationOrDivisionOperation(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

        free(node);
        node = curr_node;
    }

ON_DEBUG(TreeDumpCreateGraphFile(node));

    return node;
}

Node_t *GetMultiplicationOrDivisionOperation(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));
ON_DEBUG(fprintf(stderr, "Getting * and /\n"));

    Node_t *node = GetPowOperation(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(node));

    while ((GET_OPERATION_TYPE(tokens_array[*curr_token_num]) == MultiplicationOperation || GET_OPERATION_TYPE(tokens_array[*curr_token_num]) == DivisionOperation) 
            && tokens_array[*curr_token_num]->type == OperationType) {
        
        Node_t *curr_node = tokens_array[*curr_token_num];
        (*curr_token_num)++;
assert(node);
ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

        curr_node->left = CopyNode(node);
ON_DEBUG(fprintf(stderr, "after getting node copy"));
        curr_node->right = GetPowOperation(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

        free(node);
        node = curr_node;
    }

ON_DEBUG(TreeDumpCreateGraphFile(node));

    return node;
}

Node_t *GetPowOperation(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));
//ON_DEBUG(fprintf(stderr, "Getting pow\n"));

    Node_t *node = GetBracket(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(node));

    while (GET_OPERATION_TYPE(tokens_array[*curr_token_num]) == PowOperation && tokens_array[*curr_token_num]->type == OperationType) {
        
        Node_t *curr_node = tokens_array[*curr_token_num];
        (*curr_token_num)++;

        curr_node->left = CopyNode(node);
        curr_node->right = GetBracket(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(tokens_array[*curr_token_num]));

        free(node);
        node = curr_node;
    }

ON_DEBUG(TreeDumpCreateGraphFile(node));

    return node;
}

Node_t *GetBracket(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

    Node_t *node = NULL;

    if (CHECK_IF_OPEN_BRACKET(tokens_array[*curr_token_num])) {
        (*curr_token_num)++;
        node = GetLogicalOperator(tokens_array, curr_token_num);

        if (!CHECK_IF_CLOSE_BRACKET(tokens_array[*curr_token_num]))
            SYNTAX_ERROR;

        (*curr_token_num)++;
        return node;
    }
    
    if (tokens_array[*curr_token_num]->type == NumberType){ 
        node = tokens_array[*curr_token_num];

        (*curr_token_num)++;
        return node;
    }
    
    if (tokens_array[*curr_token_num]->type == OperationType && GET_OPERATION_ARGUMENTS_COUNT(tokens_array[*curr_token_num]) == Unary) {
        node = tokens_array[*curr_token_num];
        (*curr_token_num)++;

        if (!CHECK_IF_OPEN_BRACKET(tokens_array[*curr_token_num]))
            SYNTAX_ERROR;    
        (*curr_token_num)++;

        node->left = GetLogicalOperator(tokens_array, curr_token_num);
        node->right = NULL;

        if (!CHECK_IF_CLOSE_BRACKET(tokens_array[*curr_token_num]))
            SYNTAX_ERROR;
        (*curr_token_num)++;
        
        return node;
    }
    
    if (tokens_array[*curr_token_num]->type == VariableType) {
        if (CHECK_IF_OPEN_BRACKET(tokens_array[(*curr_token_num) + 1]))         
            return GetUserFunction(tokens_array, curr_token_num);

        else
            return tokens_array[(*curr_token_num)++];
    }
    
    return NULL;
}

Node_t *GetUserFunction(Node_t **tokens_array, int *curr_token_num) {
    
    assert(tokens_array);
    assert(curr_token_num);

    if (tokens_array[*curr_token_num]->type != VariableType || !CHECK_IF_OPEN_BRACKET(tokens_array[(*curr_token_num) + 1])) 
        SYNTAX_ERROR;
ON_DEBUG(fprintf(stderr, "Getting user function\n"));
    Node_t *node = (Node_t *) calloc(1, sizeof(Node_t));
    node->left = tokens_array[*curr_token_num];
    node->type = FunctionCallType;

    (*curr_token_num) += 2;
    node->right = GetLogicalOperator(tokens_array, curr_token_num);

    while (GET_SERVICE_STRUCTURE_TYPE(tokens_array[*curr_token_num]) == ComaStructure && tokens_array[*curr_token_num]->type == ServiceType) {
        (*curr_token_num)++;

        Node_t *new_node = CreateEmptyNode();
        new_node->left = node->right;
        new_node->right = GetLogicalOperator(tokens_array, curr_token_num);

ON_DEBUG(TreeDumpCreateGraphFile(new_node));
        node->right = new_node;
    }

    if (!CHECK_IF_CLOSE_BRACKET(tokens_array[*curr_token_num]))
        SYNTAX_ERROR;

ON_DEBUG(TreeDumpCreateGraphFile(node));
    (*curr_token_num)++;
    return node;
}
