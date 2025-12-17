#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../diff_struct.h"
#include "lib/tree/diff_dump.h"
#include "assembling.h"
#include "math.h"

#define OUTPUT_FILE_NAME "lang_output.txt"

#define SYNTAX_ERROR { fprintf(stderr, "SYNTAX ERROR from " __FILE__ ":%d:0 at %s\n\n", __LINE__, __FUNCTION__); assert(NULL); }

// AX - возвращаемое значение для функций
// BX - для работы с оперативной памятью (прямого обращения)
// CX - начало положения текущих переменных в оперативке
// DX - первый свободный элемент в оперативке

void CreateAsmFile(Tree_t *tokens_graph) {

    assert(tokens_graph);

    FILE *asm_file_ptr =  fopen(OUTPUT_FILE_NAME, "w");
    PrintAsmHeader(asm_file_ptr);
    PrintAsmFunc(asm_file_ptr, tokens_graph->root);
    PrintAsmEnd(asm_file_ptr);

    fclose(asm_file_ptr);
}

void PrintAsmHeader(FILE *asm_file_ptr) {

    assert(asm_file_ptr);

    fprintf(asm_file_ptr, "PUSH 0\n"
                          "POPREG BX\n"
                          "PUSH 0\n"
                          "POPREG CX\n"
                          "PUSH 0\n"
                          "POPREG DX\n"
                          "CALL :MAIN\n"
                          "HLT\n");
}

void PrintAsmEnd(FILE *asm_file_ptr) {

    assert(asm_file_ptr);

    fprintf(asm_file_ptr, "HLT");
}

void PrintAsmFunc(FILE *asm_file_ptr, Node_t *node) {
    
    assert(asm_file_ptr);
    assert(node);
ON_DEBUG(fprintf(stderr, "printing FUNC\n"));
    if (node->type == NewFuncType) {
        PrintAsmFunc(asm_file_ptr, node->left);
        PrintAsmFunc(asm_file_ptr, node->right);

        return;
    }

    if (node->type != FunctionType)
        SYNTAX_ERROR;

    fprintf(asm_file_ptr, ":%s\n", GET_FUNCTION_NAME(node));
ON_DEBUG(fprintf(stderr, "\nCurr function %s num = %d, variables count is %d function array is\n", functions_array[node->value.function_index_in_array]->function_name, node->value.function_index_in_array, functions_array[node->value.function_index_in_array]->variables_count); for (int j = 0; j < functions_array[node->value.function_index_in_array]->variables_count; j++) {
fprintf(stderr, "Name is %s num is %d\n", functions_array[node->value.function_index_in_array]->variables_table[j].variable_name, functions_array[node->value.function_index_in_array]->variables_table[j].variable_num);})
    
    fprintf(asm_file_ptr, "PUSHREG DX\n"
                          "PUSH %d\n"
                          "ADD\n"
                          "POPREG DX\n", GET_FUNCTION_VARIABLES_COUNT(node) - GET_FUNCTION_PARAMS_COUNT(node));
    PrintAsmNode(asm_file_ptr, node->left, functions_array[node->value.function_index_in_array]);
}

void PrintAsmNode(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct) {

    assert(asm_file_ptr);
    assert(node);
    assert(curr_function_struct);
ON_DEBUG(fprintf(stderr, "printing NODE\n"));
    switch(node->type) {
        case EmptyOperatorType:
            PrintAsmNode(asm_file_ptr, node->left, curr_function_struct);
            PrintAsmNode(asm_file_ptr, node->right, curr_function_struct);
            return;

        case NumberType:
            fprintf(asm_file_ptr, "PUSH %.0lf ;pushing number\n", round(node->value.num_type_value)); // потому что мой процессор работает только на интах
            return;

        case VariableType:
            PrintPushAsmVariable(asm_file_ptr, node, curr_function_struct);
            return;

        case OperationType:
            PrintAsmOperation(asm_file_ptr, node, curr_function_struct);
            return;

        case ComparisonOperatorType:
            PrintAsmComparison(asm_file_ptr, node, curr_function_struct);
            return;

        case ControlStructureType:
            switch(GET_CONTROL_STRUCTURE_TYPE(node)) {
                case IfOperator:
                    PrintIfStructure(asm_file_ptr, node, curr_function_struct);
                    return;

                case BeginWhileCycle:
                    PrintWhileStructure(asm_file_ptr, node, curr_function_struct);
                    return;

                case ReturnOperator:
                    PrintReturnFromAsmFunc(asm_file_ptr, node, curr_function_struct);
                    return;

                case PrintOperator:
                    PrintOutAsm(asm_file_ptr, node, curr_function_struct);
                    return;

                case InVariableOperator:
                    PrintInAsm(asm_file_ptr, node, curr_function_struct);
                    return;

                case EqualizationOperator:
                    PrintAsmEqualization(asm_file_ptr, node, curr_function_struct);
                    return;

                default:
                    SYNTAX_ERROR;
            }

        case FunctionCallType:
            PrintFunctionCallAsm(asm_file_ptr, node, curr_function_struct);
            return;

        default:
            SYNTAX_ERROR;
    }
}

void PrintFunctionCallAsm(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct) {
    
    assert(asm_file_ptr);
    assert(node);
    assert(asm_file_ptr);
ON_DEBUG(fprintf(stderr, "printing CALL\n"));
    fprintf(asm_file_ptr, "PUSHREG CX ;starting function call\n"
                          "POPM [DX]\n"
                          "PUSHREG DX\n"
                          "PUSH 1\n"
                          "ADD\n"
                          "POPREG DX\n"); // на DX лежит старое значение CX, DX теперь равно DX + 1

    Node_t *curr_empty_node = node->right;
    for (int i = 0; i < GET_FUNCTION_PARAMS_COUNT(node->left); i++) { // представим, что у маня есть миддл-энд и call вывзывает ноду типа функция а не хуй пойми что
        if (curr_empty_node->type == EmptyOperatorType)
            PrintAsmNode(asm_file_ptr, curr_empty_node->right, curr_function_struct);
        else    
            PrintAsmNode(asm_file_ptr, curr_empty_node, curr_function_struct);

        fprintf(asm_file_ptr, "POPM [DX] ;already printed param num %d\n"
                              "PUSHREG DX\n"
                              "PUSH 1\n"
                              "ADD\n"
                              "POPREG DX\n", i+1);

        curr_empty_node = curr_empty_node->left;
    } // Сейчас DX = новое CX + params_count - для нового CX надо вычесть все это дело
    fprintf(asm_file_ptr, "PUSHREG DX ;CX -> DX - param_count\n"
                          "PUSH %d\n"
                          "SUB\n"
                          "POPREG CX\n", GET_FUNCTION_PARAMS_COUNT(node->left));
    fprintf(asm_file_ptr, "CALL :%s ;Calling function\n", GET_FUNCTION_NAME(node->left));
    fprintf(asm_file_ptr, "PUSHREG CX\n"
                          "PUSH -1\n"
                          "ADD\n"
                          "POPREG DX\n"
                          "PUSHM [DX]\n"
                          "POPREG CX\n"
                          "PUSHREG AX ;Return value of function\n");
}

void PrintInAsm(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct) {
    
    assert(asm_file_ptr);
    assert(node);
    assert(asm_file_ptr);

ON_DEBUG(fprintf(stderr, "printing IN\n"));

    PutVariableNumberInBX(asm_file_ptr, node->left, curr_function_struct);
    fprintf(asm_file_ptr, "IN\n"
                          "POPM [BX] ; printing IN\n");
}

void PrintAsmEqualization(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct) {
    
    assert(asm_file_ptr);
    assert(node);
    assert(asm_file_ptr);

ON_DEBUG(fprintf(stderr, "printing equalization\n"));

    PrintAsmNode(asm_file_ptr, node->left, curr_function_struct);

    PutVariableNumberInBX(asm_file_ptr, node->right, curr_function_struct);
    fprintf(asm_file_ptr, "POPM [BX] ; printing equalization\n");
}

void PutVariableNumberInBX(FILE *asm_file_ptr, Node_t* node, Function_t *curr_function_struct) {
    
    assert(asm_file_ptr);
    assert(node);
    assert(curr_function_struct);

ON_DEBUG(fprintf(stderr, "printing num in BX\n"));
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    int curr_param_num = node->value.variable_index_in_array;
    fprintf(asm_file_ptr, "PUSHREG CX\n"
                          "PUSH %d\n", curr_param_num);
    fprintf(asm_file_ptr, "ADD\n"
                          "POPREG BX ; putting var index in BX\n");

ON_DEBUG(fprintf(stderr, "end of printing num in BX\n"));
}

void PrintReturnFromAsmFunc(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct) {
    
    assert(asm_file_ptr);
    assert(node);
    assert(asm_file_ptr);

ON_DEBUG(fprintf(stderr, "printing RETURN\n"));

    PrintAsmNode(asm_file_ptr, node->left, curr_function_struct);
    fprintf(asm_file_ptr, "POPREG AX\n"
                          "RET ; printing return from function\n");
}

void PrintOutAsm(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct) {
    
    assert(asm_file_ptr);
    assert(node);
    assert(asm_file_ptr);

ON_DEBUG(fprintf(stderr, "printing OUT\n"));

    PrintAsmNode(asm_file_ptr, node->left, curr_function_struct);
    fprintf(asm_file_ptr, "OUT\n");
}

void PrintPushAsmVariable(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct) {

    assert(asm_file_ptr);
    assert(node);
    assert(asm_file_ptr);

ON_DEBUG(fprintf(stderr, "printing VARIABLE\n"));

    PutVariableNumberInBX(asm_file_ptr, node, curr_function_struct);
    fprintf(asm_file_ptr, "PUSHM [BX] ; printing variable\n");
}

void PrintAsmOperation(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct) {
    
    assert(asm_file_ptr);
    assert(node);
    assert(asm_file_ptr);

ON_DEBUG(fprintf(stderr, "printing OPERATION\n"));

    if (GET_OPERATION_ASM_NAME(node) == NULL)
        SYNTAX_ERROR;

    if (GET_OPERATION_ARGUMENTS_COUNT(node) == Binary) {
        PrintAsmNode(asm_file_ptr, node->left, curr_function_struct);
        PrintAsmNode(asm_file_ptr, node->right, curr_function_struct);
    } else 
        PrintAsmNode(asm_file_ptr, node->left, curr_function_struct);

    fprintf(asm_file_ptr, "%s ; printing operation\n", GET_OPERATION_ASM_NAME(node));
}

void PrintAsmComparison(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct) {
    
    assert(asm_file_ptr);
    assert(node);
    assert(asm_file_ptr);

    PrintAsmNode(asm_file_ptr, node->left, curr_function_struct);
    PrintAsmNode(asm_file_ptr, node->right, curr_function_struct);

ON_DEBUG(fprintf(stderr, "printing COMPARISON\n"));

    switch(GET_COMPARISON_OPERATOR_TYPE(node)) {
        case EqualityComparison:
            fprintf(asm_file_ptr, "JNE ");
            break;

        case LessComparison:
            fprintf(asm_file_ptr, "JAE ");
            break;

        case LessOrEqualComparison:
            fprintf(asm_file_ptr, "JA ");
            break;

        case GreaterComparison:
            fprintf(asm_file_ptr, "JBE ");
            break;   
        
        case GreaterOrEqualityComparison:
            fprintf(asm_file_ptr, "JB ");
            break;

        case DenialComparison:
            fprintf(asm_file_ptr, "JE ");
            break;

        default:
            SYNTAX_ERROR;
    }
}

void PrintIfStructure(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct) {
    
    assert(asm_file_ptr);
    assert(node);
    assert(asm_file_ptr);

ON_DEBUG(fprintf(stderr, "printing IF\n"));

    static int if_number = 0;
    int curr_if_number = if_number;
    if_number++;

    if (node->left->type == ComparisonOperatorType) {
        PrintAsmNode(asm_file_ptr, node->left, curr_function_struct);
        fprintf(asm_file_ptr, ":end_if_%d\n", curr_if_number);

    } else {
        PrintAsmNode(asm_file_ptr, node->left, curr_function_struct);
        fprintf(asm_file_ptr, "PUSH 0\n",
                              "JE :end_if_%d\n", curr_if_number);   
    }    

    if (node->right->type == ControlStructureType && GET_CONTROL_STRUCTURE_TYPE(node->right) == IfElseOperator) {
ON_DEBUG(fprintf(stderr, "printing ELSE\n"));
        PrintAsmNode(asm_file_ptr, node->right->left, curr_function_struct);
        
        fprintf(asm_file_ptr, "JUMP :end_if_else_%d\n", curr_if_number);
        fprintf(asm_file_ptr, ":end_if_%d\n", curr_if_number);

        PrintAsmNode(asm_file_ptr, node->right->right, curr_function_struct);
        fprintf(asm_file_ptr, ":end_if_else_%d\n", curr_if_number);

        return;
    }

    PrintAsmNode(asm_file_ptr, node->right, curr_function_struct);
    fprintf(asm_file_ptr, ":end_if_%d\n", curr_if_number);
}

void PrintWhileStructure(FILE *asm_file_ptr, Node_t *node, Function_t *curr_function_struct) {
    
    assert(asm_file_ptr);
    assert(node);
    assert(asm_file_ptr);

ON_DEBUG(fprintf(stderr, "printing WHILE\n"));

    static int while_number = 0;
    while_number++;
    fprintf(asm_file_ptr, ":while_%d\n", while_number);

    if (node->left->type == ComparisonOperatorType) {
        PrintAsmNode(asm_file_ptr, node->left, curr_function_struct);
        fprintf(asm_file_ptr, ":end_while_%d\n", while_number);

    } else {
        PrintAsmNode(asm_file_ptr, node->left, curr_function_struct);
        fprintf(asm_file_ptr, "PUSH 0\n",
                              "JE :end_while_%d\n", while_number);   
    }  

    PrintAsmNode(asm_file_ptr, node->right, curr_function_struct);
    fprintf(asm_file_ptr, "JUMP :while_%d\n"
                          ":end_while_%d\n", while_number, while_number);
}



