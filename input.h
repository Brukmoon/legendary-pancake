#ifndef INPUT_H
#define INPUT_H

void process_input_menu(struct game* game);
void process_input_play(struct game* game);
void process_input_edit(struct game* game);

void update_menu(void);
void update_play(void);
void update_edit(void);

#endif // INPUT_H