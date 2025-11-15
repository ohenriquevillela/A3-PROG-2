#ifndef _GAME_H_
#define _GAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro5.h>														//Biblioteca base do Allegro
#include <allegro5/allegro_font.h>												   //Biblioteca de fontes do Allegro
#include <allegro5/allegro_primitives.h>
#include "projectile.h"
#include "player.h"
#include "enemy.h"
#include "boss.h"

#define MAX_INIMIGOS 2

typedef enum {
    MENU,
    PAUSE,
    GAMEPLAY,
    GAME_OVER,
    VICTORY
} Gerenciador_Estados;

typedef enum {
    PHASE_ENEMIES,
    PHASE_BOSS
} GamePhase;
// Dentro de main()


void draw_menu(ALLEGRO_BITMAP* bg_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font_instr, int selected_opt);
void draw_pause(ALLEGRO_BITMAP* bg_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font_instr, int selected_opt);
void draw_gameplay(ALLEGRO_BITMAP* bg_image, float camera_x);
void draw_scaled_background(ALLEGRO_BITMAP* bg_image, float camera_x);
void draw_scaled_bullet(bullet* b, ALLEGRO_BITMAP* sprite, float camera_x, ALLEGRO_DISPLAY* disp, float world_w, float world_h, int flip_flags);
void draw_life_bar(Player *player);
void draw_gameover(ALLEGRO_BITMAP* bg_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font_instr, int selected_opt);
void reset_game_state(Player** player_ptr, joystick** controle_ptr, float* camera_x_ptr, Enemy* inimigos[], bullet** shots_list_ptr, Boss** boss_ptr, GamePhase* phase, int* spawned, int* defeated);
void draw_boss_life_bar(Boss* boss, ALLEGRO_DISPLAY* disp);
void draw_victory(ALLEGRO_BITMAP* bg_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font_instr, int selected_opt);
void draw_stamina_bar(Player* player);
bool check_collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
#endif // _GAME_H_