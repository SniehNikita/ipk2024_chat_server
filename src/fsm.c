/**
 * @file fsm.c
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 19.04.2024
 * @brief Finite state machine for client. 
 */

#include "fsm.h"

int fsm_map[6][9] = {
                /* t_nu t_au t_re t_nr t_ms t_jo t_er t_by t_no */ 
    /* s_nu */   { s_nu,s_nu,s_nu,s_nu,s_nu,s_nu,s_nu,s_nu,s_nu },
    /* s_ac */   { s_nu,s_au,s_nu,s_nu,s_nu,s_er,s_nu,s_nu,s_nu },
    /* s_au */   { s_nu,s_au,s_op,s_au,s_er,s_er,s_er,s_er,s_er },
    /* s_op */   { s_nu,s_er,s_er,s_er,s_op,s_op,s_en,s_en,s_op },
    /* s_er */   { s_nu,s_en,s_en,s_en,s_en,s_en,s_en,s_en,s_en },
    /* s_en */   { s_nu,s_en,s_en,s_en,s_en,s_en,s_en,s_en,s_en }
};

fsm_state_t fsm_next(fsm_state_t state, fsm_transition_t transition) {
    return fsm_map[state][transition];
}