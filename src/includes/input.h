/**
 * Process input and react to it.
 * @author Michal H.
 *
 */
#ifndef INPUT_H
#define INPUT_H

// Process input for every state.
void process_input_menu(struct game* game);
void process_input_play(struct game* game);
void process_input_edit(struct game* game);

// Update the game.
void update_menu(struct game* game);
void update_play(struct game* game);
void update_edit(struct game* game);

#endif // INPUT_H