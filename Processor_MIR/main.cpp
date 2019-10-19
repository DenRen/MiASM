#include <cstdio>
#include "/home/tester/Documents/GitHub/MiASM/ASM_COMPILATOR_MIR/COMPILATOR.h"
#include "/home/tester/Documents/GitHub/MiASM/ASM_COMPILATOR_MIR/MyLib.h"
#include "/home/tester/Documents/GitHub/MiASM/ASM_COMPILATOR_MIR/Super_Stack_Library.h"

#define ERROR_MC {PRINT_ERROR("Error_in_MC") return 0;}
#define RAM_SIZE 100000

#define DEADLINE_RAM 1
#define bprintf_ON true

typedef struct struct_proc_t {
    void *PC;               // Указатель на текущую команду в MC
    const void *begin_MC;   // Начало MC
    size_t size_mc;         // В байтах
} proc_t;

bool get_comand (proc_t *proc, CmdCode_t *comand) {
    // Закончился ли MC
    if ((__uint8_t *) proc->PC >= (__uint8_t *) proc->begin_MC + proc->size_mc) {
        return false;
    }
    if (bprintf_ON) {
        bprintf (*(unsigned char *) proc->PC);
        printf ("-> ");
    }
    *comand = *(CmdCode_t *) proc->PC;
    proc->PC = (CmdCode_t *) proc->PC + 1;
    return true;
}

inline bool get_arg_comand (proc_t *proc, CmdCode_t *comand, number_t *imm, reg_t *reg) {
    if (comand->reg) {
        *reg = *(reg_t *) proc->PC;
        proc->PC = (reg_t *) proc->PC + 1;
    }
    if (comand->imm) {
        *imm = *(number_t *) proc->PC;
        proc->PC = (number_t *) proc->PC + 1;
    }
}

int main () {
    const char path_mc[] = "/home/tester/Documents/GitHub/MiASM/ASM_COMPILATOR_MIR/cmake-build-debug/machinecode.mir";
    proc_t proc = {};
    int state_func = 0;

    proc.PC = (void *) Read_File_To_Buffer (path_mc, &proc.size_mc, &state_func, false, true);
    proc.begin_MC = proc.PC;

    if (state_func != rftb::ALL_RIGHT) {
        PRINT_ERROR ("")
        free ((void *) proc.begin_MC);
        return 0;
    }

    //STACK_INIT_BEGIN// Инициализируем стек
    FILE *log_stk = fopen ("stk.log", "w");
    if (log_stk == nullptr) {
        PRINT_ERROR ("Error open file for create stack")
        free ((void *) proc.begin_MC);
        return 0;
    }

    stck::Secure_Stack_t Stack = {};

    Stack.size = 1000;
    Stack.Sec_Level = 2;
    Stack.expansion_coef = 0.2;
    Stack.dumpOn = true;
    Stack.dump_file = log_stk;

    if (StackConstruct (&Stack)) {
        free ((void *) proc.begin_MC);
        return 0;
    }
    //STACK_INIT_END//

    //REGISTERS_INIT_BEGIN//
    number_t REG[size_registers] = {0};
    //REGISTERS_INIT_END//

    //RAM_INIT_BEGIN//
    __uint8_t *RAM = (__uint8_t *) calloc (RAM_SIZE, 1);
    //RAN_INIT_END

    CmdCode_t comand = {};
    number_t value_imm = 0, value0 = 0, value1 = 0;
    reg_t num_reg = 0;
    number_t value_reg = 0;
    bool exit = false;

#include "Decoding_MC.h"
    //Выполняем команду за командой
    while (get_comand (&proc, &comand)) {

        if (bprintf_ON) {    // Транслирует текущие команды
            printf ("%s M:%d, I:%d, R:%d\n", commands[comand.code], comand.mem, comand.imm, comand.reg);
        }
        if (comand.mem)     // Специальная задержка по времени при использовании RAM
            sleep (DEADLINE_RAM);


        value_imm = value0 = value1 = 0;
        num_reg = 0;
        value_reg = 0;


        switch (comand.code) {
            case cmd_PUSH: {
                if (!get_arg_comand (&proc, &comand, &value_imm, &num_reg)) {
                    if (comand.mem) {
                        //push [ax + 5] - положить в стек число из ячейки RAM по номеру ax + 5
                        if (comand.reg)
                            value_reg = REG[num_reg];
                        if (push (&Stack, *(number_t *) (RAM + (int) floor (value_reg + value_imm)))) ERROR_STCK
                    } else {
                        if (comand.reg)
                            value_reg = REG[num_reg];
                        if (push (&Stack, value_reg + value_imm)) ERROR_STCK
                    }
                } else ERROR_MC
                break;
            }
            case cmd_POP: {
                if (!get_arg_comand (&proc, &comand, &value_imm, &num_reg)) {
                    if (comand.mem) {
                        //pop [ax + 5] - положить из стека число в ячейку RAM по номеру ax + 5
                        if (comand.reg)
                            value_reg = REG[num_reg];
                        if (pop (&Stack, &value0)) ERROR_STCK
                        *(number_t *) (RAM + (int) floor (value_reg + value_imm)) = value0;
                    } else {    //REG
                        if (pop (&Stack, &value_imm)) ERROR_STCK
                        REG[num_reg] = value_imm;
                    }
                } else ERROR_MC
                break;
            }
            case cmd_ADD: ELEMENTARY_OPERATION (+)
            case cmd_SUB: ELEMENTARY_OPERATION (-)
            case cmd_MUL: ELEMENTARY_OPERATION (*)
            case cmd_DIV: ELEMENTARY_OPERATION (/)
            case cmd_OUT: {
                if (pop (&Stack, &value_imm)) ERROR_STCK
                printf ("%.10lg\n", value_imm);
                if (push (&Stack, value_imm)) ERROR_STCK
                break;
            }
            case cmd_SQRT: ELEMENTARY_FUNC (sqrt)
            case cmd_SIN: ELEMENTARY_FUNC (sin)
            case cmd_COS: ELEMENTARY_FUNC (cos)
            case cmd_LOGN: ELEMENTARY_FUNC (log)
            case cmd_EXIT: {
                exit = true;
                break;
            }
            case cmd_JMP: {
                jmp_t jmp = *(jmp_t *) proc.PC;
                proc.PC = (__uint8_t *) proc.begin_MC + jmp;
                break;
            }
            default: ERROR_MC
        }
        if (exit)
            break;
    }

    free ((void *) proc.begin_MC);
    return 0;
}