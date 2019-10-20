//
// Created by tester on 10.10.2019.
//

// Костыль ))
#define ___END '\0'

/*
 * ax, bx, cx, dx
*/

#ifndef MY_COMPUTER_COMPILATOR_H
#define MY_COMPUTER_COMPILATOR_H

#define MAXLENCOMM 100
#define MAXLENREGS 5

typedef double number_t;
typedef __uint8_t comand_t;
typedef __uint8_t reg_t;
typedef __int64_t jmp_t;

enum COMMANDS {
    cmd_PUSH,
    cmd_POP,
    cmd_ADD,
    cmd_SUB,
    cmd_MUL,
    cmd_DIV,
    cmd_OUT,
    cmd_EXIT,
    cmd_SQRT,
    cmd_SIN,
    cmd_COS,
    cmd_LOGN,
    cmd_JMP,
    cmd_JA,
    cmd_JAE,
    cmd_JB,
    cmd_JBE,
    cmd_JE,
    cmd_JNE,
    cmd_CALL,
    cmd_RET
};

const char commands[][MAXLENCOMM] = {
        "push",
        "pop",
        "add",
        "sub",
        "mul",
        "div",
        "out",
        "exit",
        "sqrt",
        "sin",
        "cos",
        "logn",
        "jmp",
        "ja",
        "jae",
        "jb",
        "jbe",
        "je",
        "jne",
        "call",
        "ret"
};

enum REGISTERS {
    AX, BX, CX, DX, RX, NM
};

const char registers[][MAXLENREGS] = {
        "ax", "bx", "cx", "dx", "rx", "nm"
};
// nm - служебный регистр! Хранит начальный номер стека перед вызовом функции (для функции call)

const unsigned size_commands = sizeof (commands) / MAXLENCOMM;
const unsigned size_registers = sizeof (registers) / MAXLENREGS;
const unsigned max_quantity_portals = 1024;

const char symb_comment = ';';
const char symb_separator = ' '; // Не должен быть буквой цифрой иначе порежет слова и числа)
const char symb_portal = ':';

// Разрешённые символы. '[]' - для обращение к RAM,
// '+' - для удобной работы с массивами в, к примеру,
// [ax + 5] обратится к пятому элементу массива в RAM
const char necess_symb[] = {
        '[', ']', '+', ':', ___END
};

const char RAM_symb[] = {
        '[', ']', ___END
};

const char critically_forbidden_characters[] = {
        '-', '*', '/', '\\', ___END
};

struct Compiler_t {
    char *buf;              // Буффер, где лежит необр. текстовые команды
    void *MC;               // Итоговый машинный код
    size_t size_buf;      // Размер буффера в байтах
    size_t count_elem;    // Кол-во элементов в буфере (Пример: "push 5" -> count == 2)
    int state_stack;        // Код ошибки самого стека (например SYNTAX_ERROR)
    int state_func;         // Код ошибки последней функции, которая его обрабатывала (например CALLOC_ERROR)
};

struct CmdCode_t {
    comand_t code   : 5;
    comand_t reg    : 1;
    comand_t imm    : 1;
    comand_t mem    : 1;
};

#endif //MY_COMPUTER_COMPILATOR_H

























