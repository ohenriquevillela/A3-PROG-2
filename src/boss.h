// Arquivo: boss.h

#ifndef BOSS_H
#define BOSS_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <stdbool.h>
#include "player.h"
#include "projectile.h"


// --- Constantes do Boss ---
#define BOSS_SCALE 10.0f
#define BOSS_SPEED 70.0f
#define BOSS_FIRE_RATE 90
#define BOSS_IDEAL_DISTANCE 400.0f
#define BOSS_ATTACK_RANGE 1000.0f
#define BOSS_HEALTH 1000.0f

// --- Constantes de Animação do Boss ---
#define TOTAL_FRAMES_BOSS_ACTIVE 8 // <<< Animação dele andando e atirando
#define FRAME_DELAY_BOSS_ACTIVE 8
#define TOTAL_FRAMES_BOSS_DEATH 7
#define FRAME_DELAY_BOSS_DEATH 10

typedef enum {
    BOSS_STATE_ACTIVE,
    BOSS_STATE_DEATH
} BossState;

// Direção de movimento do Boss
typedef enum {
    BOSS_DIR_RIGHT,
    BOSS_DIR_LEFT
} BossDirection;


// --- Estrutura do Boss ---
typedef struct {
    float x, y;
    float health, max_health;
    bool active;
    BossState estado;
    BossDirection direcao;
    float speed;
    int fire_timer;
    int death_timer;
    ALLEGRO_BITMAP* sheets[2]; // 0 = Active, 1 = Death
    int frame_widths[2];
    int frame_heights[2];
    int total_frames[2];
    int frame_delays[2];
    int frame_atual;
    int contador_frames;

} Boss;

// --- Funções Públicas ---
Boss* boss_create(float start_x, float start_y);
void boss_destroy(Boss* boss);
void boss_update(Boss* boss, Player* player, bullet** all_shots_list_head, float dt);
void boss_draw(Boss* boss, float camera_x);

#endif