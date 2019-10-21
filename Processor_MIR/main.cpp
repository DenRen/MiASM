#include <cstdio>
#include "/home/tester/Documents/GitHub/MiASM/ASM_COMPILATOR_MIR/COMPILATOR.h"
#include "/home/tester/Documents/GitHub/MiASM/ASM_COMPILATOR_MIR/MyLib.h"
#include "/home/tester/Documents/GitHub/MiASM/ASM_COMPILATOR_MIR/Super_Stack_Library.h"

#define ERROR_MC {PRINT_ERROR("Error_in_MC") return 0;}

#define bprintf_ON true

typedef struct struct_proc_t {
    void *PC;               // Указатель на текущую команду в MC
    const void *begin_MC;   // Начало MC
    size_t size_mc;         // В байтах
} proc_t;

bool get_comand(proc_t *proc, CmdCode_t *comand) {
    // Закончился ли MC
    if ((__uint8_t *) proc->PC >= (__uint8_t *) proc->begin_MC + proc->size_mc) {
        return false;
    }
    if (bprintf_ON) {
        bprintf(*(unsigned char *) proc->PC);
        printf("-> ");
    }
    *comand = *(CmdCode_t *) proc->PC;
    proc->PC = (CmdCode_t *) proc->PC + 1;
    return true;
}

bool get_arg_comand(proc_t *proc, CmdCode_t *comand, number_t *imm, reg_t *reg) {
    if (comand->reg) {
        *reg = *(reg_t *) proc->PC;
        proc->PC = (reg_t *) proc->PC + 1;
    }
    if (comand->imm) {
        *imm = *(number_t *) proc->PC;
        proc->PC = (number_t *) proc->PC + 1;
    }
    return false;
}

int SHOW_GDD(__uint8_t *RAM, FILE *gpp_log);

int main() {
    const char path_mc[] = "/home/tester/Documents/GitHub/MiASM/ASM_COMPILATOR_MIR/cmake-build-debug/machinecode.mir";
    const char gpp_file[] = "gpp_log";
    proc_t proc = {};
    int state_func = 0;

    proc.PC = (void *) Read_File_To_Buffer(path_mc, &proc.size_mc, &state_func, false, true);
    proc.begin_MC = proc.PC;

    if (state_func != rftb::ALL_RIGHT) {
        PRINT_ERROR ("")
        free((void *) proc.begin_MC);
        return 0;
    }

    //STACK_INIT_BEGIN// Инициализируем стек
    FILE *log_stk = fopen("stk.log", "w");
    if (log_stk == nullptr) {
        PRINT_ERROR ("Error open file for create stack")
        free((void *) proc.begin_MC);
        return 0;
    }

    stck::Secure_Stack_t Stack = {};

    Stack.size = 3;
    Stack.Sec_Level = 2;
    Stack.expansion_coef = 0.2;
    Stack.dumpOn = true;
    Stack.dump_file = log_stk;

    if (StackConstruct(&Stack)) {
        free((void *) proc.begin_MC);
        return 0;
    }
    //STACK_INIT_END//

    //REGISTERS_INIT_BEGIN//
    number_t REG[size_registers] = {0};
    //INIT_GRAPHIC_REG
    REG[GL] = grph::length;
    REG[GW] = grph::width;
    REG[GCD] = grph::color_depth;
    //REGISTERS_INIT_END//

    // Создаю файл, который будет использоваться для передачи
    // графики на монитор
    FILE *gpp_log = fopen(gpp_file, "w");
    if (gpp_log == nullptr) {
        PRINT_ERROR ("ERROR CREATE GPP_FILE")
        StackDestruct(&Stack);
        fclose(log_stk);
        return 0;
    }


    //RAM+GDD_INIT_BEGIN//
    // При обращении к RAM используется задержка по времени DEADLINE_RAM
    // При обращении к GDD используется задержка по времени DEADLINE_GDD
    __uint8_t *RAM = (__uint8_t *) calloc(RAM_size +
                                          grph::length * grph::width * grph::color_depth, 1);

    REG[RMS] = RAM_size - 1;    // Максимальный номер для обращения к RAM
    //RAN+GDD_INIT_END//

    CmdCode_t comand = {};
    number_t value_imm = 0, value0 = 0, value1 = 0;
    reg_t num_reg = 0;
    number_t value_reg = 0;
    bool exit = false;
    size_t number_func = 0;
    size_t NM0 = 0;

    bool RAM_sleep = false;
    bool GDD_sleep = false;


#include "Decoding_MC.h"
    //Выполняем команду за командой
    while (get_comand(&proc, &comand)) {

        if (bprintf_ON) {    // Транслирует текущие команды
            printf("%s M:%d, I:%d, R:%d\n", commands[comand.code], comand.mem, comand.imm, comand.reg);
        }


        value_imm = value0 = value1 = 0;
        num_reg = 0;
        value_reg = 0;

        if (comand.mem)
            RAM_sleep = true;

        switch (comand.code) {
            case cmd_PUSH: {
                if (!get_arg_comand(&proc, &comand, &value_imm, &num_reg)) {
                    if (comand.mem) {
                        //push [ax + 5] - положить в стек число из ячейки RAM по номеру ax + 5
                        if (comand.reg)
                            value_reg = REG[num_reg];
                        if (push(&Stack, *(number_t *) (RAM + (int) floor(value_reg + value_imm)))) ERROR_STCK
                    } else {
                        if (comand.reg)
                            value_reg = REG[num_reg];
                        if (push(&Stack, value_reg + value_imm)) ERROR_STCK
                    }
                } else ERROR_MC
                break;
            }
            case cmd_POP: { // Имеет воможность класть значения в видеокарту
                if (!get_arg_comand(&proc, &comand, &value_imm, &num_reg)) {
                    if (comand.mem) {
                        //pop [ax + 5] - положить из стека число в ячейку RAM по номеру ax + 5
                        if (comand.reg)
                            value_reg = REG[num_reg];
                        if (pop(&Stack, &value0)) ERROR_STCK

                        if ((size_t) floor(value_reg + value_imm) <= RAM_size) // Положить в оперативную память
                            *(number_t *) (RAM + (int) floor(value_reg + value_imm)) = value0;
                        else {
                            *(__uint8_t *) (RAM + (int) floor(value_reg + value_imm)) = (__uint8_t) floor(value0);
                            RAM_sleep = false;
                            GDD_sleep = true;
                        }
                    } else if (comand.reg) {    //REG
                        if (pop(&Stack, &value_imm)) ERROR_STCK
                        REG[num_reg] = value_imm;
                    } else {    // Удалить из стека
                        if (pop(&Stack, &value_imm)) ERROR_STCK
                    }
                } else ERROR_MC
                break;
            }
            case cmd_ADD: ELEMENTARY_OPERATION (+)
            case cmd_SUB: ELEMENTARY_OPERATION (-)
            case cmd_MUL: ELEMENTARY_OPERATION (*)
            case cmd_DIV: ELEMENTARY_OPERATION (/)
            case cmd_OUT: {
                if (pop(&Stack, &value_imm)) ERROR_STCK
                printf("%.33lg\n", value_imm);
                if (push(&Stack, value_imm)) ERROR_STCK
                break;
            }
            case cmd_SQRT: ELEMENTARY_FUNC (sqrt)
            case cmd_SIN: ELEMENTARY_FUNC (sin)
            case cmd_COS: ELEMENTARY_FUNC (cos)
            case cmd_LOGN: ELEMENTARY_FUNC (log)
            case cmd_FLR: ELEMENTARY_FUNC (floor)
            case cmd_EXIT: {
                exit = true;
                break;
            }
            case cmd_JMP: {
                jmp_t jmp = *(jmp_t *) proc.PC;
                proc.PC = (__uint8_t *) proc.begin_MC + jmp;
                break;
            }
            case cmd_JA: IF_FUNC (>)
            case cmd_JAE: IF_FUNC (>=)
            case cmd_JB: IF_FUNC (<)
            case cmd_JBE: IF_FUNC (<=)
            case cmd_JE: IF_FUNC (==)
            case cmd_JNE: IF_FUNC (!=)
            case cmd_CALL: {
                //Кладём текущий номер последнего элемента в стеке
                if (number_func++) {
                    if (push(&Stack, NM0)) ERROR_STCK
                    NM0 = Stack.number - 1;
                } else {
                    NM0 = Stack.number;
                    if (push(&Stack, NM0)) ERROR_STCK
                }

                jmp_t jmp = *(jmp_t *) proc.PC;
                number_t sdfasdf = (number_t) ((__uint8_t *) ((jmp_t *) proc.PC + 1) - (__uint8_t *) proc.begin_MC);
                if (push(&Stack,
                         (number_t) ((__uint8_t *) ((jmp_t *) proc.PC + 1) - (__uint8_t *) proc.begin_MC))) ERROR_STCK
                proc.PC = (__uint8_t *) proc.begin_MC + jmp;
                break;
            }
            case cmd_RET: {
                // Очищаем стек от локальных переменных функции, из которой мы хотим выйти
                __int32_t quant_call_pops = Stack.number - (__uint32_t) REG[NM] - 2;
                number_t sdfsdf = (__uint32_t) REG[NM];
                number_t temp_value = 0;
                while (quant_call_pops-- > 0)
                    if (pop(&Stack, &temp_value)) ERROR_STCK
                // Перемещаем PC
                if (pop(&Stack, &temp_value)) ERROR_STCK
                proc.PC = (__uint8_t *) proc.begin_MC + (__uint32_t) temp_value;
                if (pop(&Stack, &REG[NM])) ERROR_STCK
                break;
            }
            default: ERROR_MC
        }

        if (exit)
            break;

        REG[GNM] = Stack.number;

        if (RAM_sleep)     // Специальная задержка по времени при использовании RAM
            sleep(DEADLINE_RAM);
        else if (GDD_sleep)
            sleep(DEADLINE_GDD);

        RAM_sleep = false;
        GDD_sleep = false;

        // Вывод видеоряда
        if (REG[GO])
            if (SHOW_GDD(RAM, gpp_log)) ERROR_STCK;

    }

    free((void *) proc.begin_MC);
    return 0;
}

int SHOW_GDD(__uint8_t *RAM, FILE *gpp_log) {
    size_t quantity_bytes_gdd = grph::length * grph::width * grph::color_depth;
    if (fwrite(RAM + RAM_size, 1, quantity_bytes_gdd, gpp_log) != grph::length * grph::width * grph::color_depth) {
        PRINT_ERROR ("ERROR GDD")
        return 1;
    }

    system("cp gpp_log /dev/fb0");

    fseek(gpp_log, 0, SEEK_SET);
    return 0;
}