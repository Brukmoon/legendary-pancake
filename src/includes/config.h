/*
 * Configuration management.
 * @author Michal H.
 *
 */
#ifndef CONFIG_H
#define CONFIG_H

#define GAME_VERSION "0.0.2 [dev]"
#define GAME_NAME "Dead Zone"
// Title of the game window. (GAME_NAME GAME_VERSION)
#define WINDOW_TITLE GAME_NAME " " GAME_VERSION
#define FPS 60

// Normalized screen width and height.
// 1024x768 during development, tested up to 1920x1080 (FullHD)
#define SCREEN_WIDTH 1152
#define SCREEN_HEIGHT 864
//#define SCREEN_WIDTH 1920
//#define SCREEN_HEIGHT 1080

#define DATA_PATH "./assets/"
#define IMG_PATH DATA_PATH##"gfx/"
#define SOUND_PATH DATA_PATH##"sfx/"
#define SOUND_PATH DATA_PATH##"sfx/"
#define LEVEL_PATH DATA_PATH##"level/"

// good monospace font
#define FONT_TYPE "Consolas.ttf"
#define FONT_FILE DATA_PATH FONT_TYPE

// Code configuration.
#define CAMERA_LIMIT 1
#define SHOW_CONSOLE 1
#define SHOW_GRID 0
#define MUSIC_ON 1
#define SOUND_ON 1
#define DAMAGE_ON 1
#define FULLSCREEN_ON 0
/*
 * Python 3 must be supported by the system.
 * On Linux: Install python package.
 * On Windows: Install python application and modify PATH variable.
 *
 */
#define PYTHON_ON 1

// TODO: Dynamic configuration, load on init.

#endif // CONFIG_H