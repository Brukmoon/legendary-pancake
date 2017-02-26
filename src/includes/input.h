/**
 * Process input and react to it.
 * @author Michal H.
 *
 */
#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

// Process input for each state.
bool process_input_menu(struct game* game);
bool process_input_play(struct game* game);
bool process_input_edit(struct game* game);
bool process_input_preedit(struct game* game);
bool process_input_preplay(struct game* game);

// Update the game.
void update_menu(struct game* game);
void update_play(struct game* game);
void update_edit(struct game* game);

#endif // INPUT_H