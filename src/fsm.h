/**
 * @file fsm.h
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Finite state machine for client. 
 */

#ifndef __FSM_H__

#define __FSM_H__

typedef enum {
    s_nu = 0, // NULL - Do nothing
    s_ac = 1, // Accept
    s_au = 2, // Auth
    s_op = 3, // Open
    s_er = 4, // Error
    s_en = 5  // End
} fsm_state_t;

typedef enum {
    t_nu = 0, // NULL
    t_au = 1, // Auth
    t_re = 2, // Reply
    t_nr = 3, // Not Reply
    t_ms = 4, // Message
    t_jo = 5, // Join
    t_er = 6, // Error
    t_by = 7, // Bye
    t_no = 8  // No message
} fsm_transition_t;

/**
 * @brief Calculates next state from given state with given transition
 * 
 * @param state Starting state
 * @param transition Transition
 * @return fsm_state_t 
 */
fsm_state_t fsm_next(fsm_state_t state, fsm_transition_t transition);

#endif