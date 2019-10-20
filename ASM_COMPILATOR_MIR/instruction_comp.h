//
// Created by tester on 15.10.2019.
//

#ifndef MY_COMPUTER_INSTRUCTION_COMP_H
#define MY_COMPUTER_INSTRUCTION_COMP_H

#endif //MY_COMPUTER_INSTRUCTION_COMP_H

// Получает аргумент типа
/*
 * n, r, n+r, [n], [r], [n+r]
 * */
#define GET_ARG \
bool number = false, reg = false, memopen = false;                                                  \
                    int type = 0;                                                                   \
                    if (!GET_ELEM && data->state_func == conv::CMD_TOO_LARGE) SYNTAXERR             \
                    TYPE                                                                            \
                    while (type != -1) {                                                            \
                        if (number_iter_compil == 0)                                                \
                            printf("%s ", element);                                                 \
                        NEXT                                                                        \
                        switch (type) {                                                             \
                            case conv::NUMBER:                                                      \
                                if (!number) {                                                      \
                                    comand.imm = 1;                                                 \
                                    arg_num = strtod(element, nullptr);                             \
                                    number = true;                                                  \
                                    break;                                                          \
                                } else SYNTAXERR                                                    \
                            case conv::REGISTER:                                                    \
                                if (!reg) {                                                         \
                                    comand.reg = 1;                                                 \
                                                                                                    \
                                    for (unsigned j = 0; j < size_registers ; j++)                  \
                                        if (!strcmp (element, registers[j])){                       \
                                            arg_reg = j;                                            \
                                            break;                                                  \
                                        }                                                           \
                                                                                                    \
                                    reg = true;                                                     \
                                    break;                                                          \
                                } else SYNTAXERR                                                    \
                            case conv::RAM_TYPE_OPEN:                                               \
                                if (!memopen) {                                                     \
                                    memopen = true;                                                 \
                                    break;                                                          \
                                } else SYNTAXERR                                                    \
                            case conv::RAM_TYPE_CLOSE:                                              \
                                if (memopen && (comand.imm || comand.reg)){                         \
                                    comand.mem = 1;                                                 \
                                    break;                                                          \
                                }                                                                   \
                                else SYNTAXERR                                                      \
                            default:                                                                \
                                break;                                                              \
                        }                                                                           \
                        if(!GET_ELEM)                                                               \
                            break;                                                                  \
                        TYPE                                                                        \
                    }                                                                               \
                                                                                                    \
                    if (number_iter_compil == 0) {                                                  \
                        printf ("\tREG:%d, NUM:%lg\t", arg_reg, arg_num);                           \
                        printf ("M:%d, I:%d, R:%d\n", comand.mem, comand.imm, comand.reg);          \
                    }


switch ((const unsigned) i) {
    case cmd_PUSH: {
        GET_ARG
        break;
    }
    // Команды, которые не требуют аргументов и могут положить результат выполнения команды в регистер или RAM
    // Т.е. невозможна ситуация pop 5 + ax, почему? Ну объясните тогда это мне denrenruslan@gmail.com )))
    case cmd_POP: // Не забудем, что невозможна ситуация: pop 5 + ax
    case cmd_ADD: // ADD - сложить два числа в стеке и положить результат в стек. ADD [cx + 3] сложить и положить результат в RAM [cx + 3]
    case cmd_SUB: // SUB - вычесть их верхнего нижний элемент стека и положить результат в стек. SUB [cx + 3] положить результат в RAM [cx + 3]
    case cmd_MUL:
    case cmd_DIV:
    case cmd_SQRT:
    case cmd_SIN:
    case cmd_LOGN:
    case cmd_COS: {
        GET_ARG
        if (!comand.mem && comand.imm) SYNTAXERR //pop 5
        break;
    }
    case cmd_EXIT: {
        exit = true;
        break;
    }
    case cmd_OUT:
    case cmd_RET:
        break;

    case cmd_JA:        //Функции, которые принимают словестный аргумент
    case cmd_JAE:
    case cmd_JB:
    case cmd_JBE:
    case cmd_JE:
    case cmd_JNE:
    case cmd_JMP:
    case cmd_CALL: {
        if (!GET_ELEM){
            data->state_stack = conv::SYNTAX_ERROR;
            data->state_func = conv::SYNTAX_ERROR;
            return nullptr;
        }
        NEXT
        if (number_iter_compil == 0)
            printf("%s\n", element);
        break;
    }
    default: SYNTAXERR
}

