//
// Created by tester on 19.10.2019.
//

#ifndef PROCESSOR_DECODING_MC_H
#define PROCESSOR_DECODING_MC_H

#define ERROR_STCK {PRINT_ERROR("") return 0;}

#define ELEMENTARY_OPERATION(zn)                                                                                                                      \
    {                                                                                                                                                 \
                if (!get_arg_comand (&proc, &comand, &value_imm, &num_reg)) {                                                                         \
                    if (comand.mem) {                                                                                                                 \
                        if (comand.reg)                                                                                                               \
                            value_reg = REG[num_reg];                                                                                                 \
                        if (pop (&Stack, &value0)) ERROR_STCK                                                                                         \
                        if (pop (&Stack, &value1)) ERROR_STCK                                                                                         \
                        *(number_t *) (RAM + (int) floor (value_reg + value_imm)) = value1 zn value0;                                                 \
                    } else if (comand.reg) {                                                                                                          \
                        if (pop (&Stack, &value0)) ERROR_STCK                                                                                         \
                        if (pop (&Stack, &value1)) ERROR_STCK                                                                                         \
                        REG[num_reg] = value1 zn value0;                                                                                              \
                    } else {                                                                                                                          \
                        if (pop (&Stack, &value0)) ERROR_STCK                                                                                         \
                        if (pop (&Stack, &value1)) ERROR_STCK                                                                                         \
                        if (push (&Stack, value1 zn value0)) ERROR_STCK                                                                               \
                    }                                                                                                                                 \
                } else ERROR_MC                                                                                                                       \
                break;                                                                                                                                \
            }
#define ELEMENTARY_FUNC(func)                                                                                                                         \
            {                                                                                                                                         \
                if (!get_arg_comand (&proc, &comand, &value_imm, &num_reg)) {                                                                         \
                    if (comand.mem) {                                                                                                                 \
                        if (comand.reg)                                                                                                               \
                            value_reg = REG[num_reg];                                                                                                 \
                        if (pop (&Stack, &value0)) ERROR_STCK                                                                                         \
                        *(number_t *) (RAM + (int) floor (value_reg + value_imm)) = func(value0);                                                     \
                    } else if (comand.reg) {                                                                                                          \
                        if (pop (&Stack, &value0)) ERROR_STCK                                                                                         \
                        REG[num_reg] = func(value0);                                                                                                  \
                    } else {                                                                                                                          \
                        if (pop (&Stack, &value0)) ERROR_STCK                                                                                         \
                        if (push (&Stack, func(value0))) ERROR_STCK                                                                                   \
                    }                                                                                                                                 \
                } else ERROR_MC                                                                                                                       \
                break;                                                                                                                                \
            }

#endif //PROCESSOR_DECODING_MC_H
