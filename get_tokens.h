#include "../diff_struct.h"

// Переменные - только английские буквы потому что а хули нет
// Программа начинается со слова НАЧАЛО и заканчивается на слово КОНЕЦ
// ВСЕ ДОЛЖНО БЫТЬ КАПСОМ ИНАЧЕ СИНТАКСИЧЕСКАЯ ОШИБКА (кроме переменных)
// имена переменных должны начинаться с буквы, цифры после

Node_t **GetTokensArray(char **curr_symbol, int *tokens_count);

Node_t *GetNumberToken(char **curr_symbol);
Node_t *GetVariableToken(char **curr_symbol);
Node_t *GetOperationToken(char **curr_symbol);

char *GetRussianWord(char **data);
char *GetWord(char **curr_symbol, size_t data_len);

void SyntaxError(const char *file_name, const char *function_name, int line_number);
void SkipSpaces(char **curr_pos);
