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

#define DEADLINE_RAM 0
#define DEADLINE_GDD 0

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
    cmd_RET,
    cmd_FLR
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
        "ret",
        "flr"
};

enum REGISTERS {
    AX, BX, CX, DX, EX, RX, NM, GNM, GL, GW, GCD, RMS, GO
};

const char registers[][MAXLENREGS] = {
        "ax", "bx", "cx", "dx", "ex", "rx", "nm", "gnm", "gl", "gw", "gcd", "rms", "go"
};
// nm - служебный регистр! Хранит начальный номер стека перед вызовом функции (для функции call)
// gnm - постоянно устанавливается в значение текущего номера элемента в стеке (Stack.number)
// go - Graphic on! При 1 выводится графика из видеопамяти и этот регистр обнуляется
// Запрещённые к переинициализации регистры для графики
// gl - graphic length
// gw - graphic width
// gcd - graphic color depth
// rms - RAM size

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

const __uint16_t RAM_size = 1000;

namespace grph {
    //BGRA
    const __uint16_t length = 1920;  // Длина
    const __uint16_t width = 300;
    const __uint16_t color_depth = 4; // Глубина цвета
}
#endif //MY_COMPUTER_COMPILATOR_H

























