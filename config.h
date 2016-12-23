/*
 * Configuration management.
 * @author Michal H.
 *
 */
#ifndef CONFIG_H
#define CONFIG_H

#define GAME_VERSION "0.0.0 [dev]"
#define GAME_NAME "Dead Zone"
// Title of the game window. (GAME_NAME GAME_VERSION)
#define WINDOW_TITLE GAME_NAME " " GAME_VERSION

// Normalized screen width and height.
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 800

// TODO: Dynamic configuration, load on init.

#endif // CONFIG_H