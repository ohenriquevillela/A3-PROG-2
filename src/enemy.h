#ifndef ENEMY_H
#define ENEMY_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <stdbool.h>
#include "player.h"
#include "projectile.h"

// --- Constantes Específicas do Inimigo ---
#define ENEMY_SCALE 10.0f
#define ENEMY_SPEED 100.0f      // Velocidade do inimigo em pixels por segundo
#define ENEMY_FIRE_RATE 80     // Atira a cada tantos
#define ENEMY_HEALTH 100.0f    // Vida padrão do inimigo

// Constantes de Animação do Inimigo
#define TOTAL_FRAMES_ENEMY_WALK_SHOOT 8
#define FRAME_DELAY_ENEMY_WALK_SHOOT 10
#define TOTAL_FRAMES_ENEMY_DEATH 1    
#define FRAME_DELAY_ENEMY_DEATH 8


typedef enum {
    ENEMY_STATE_WALK_AND_SHOOT,
    ENEMY_STATE_DEATH
} EnemyState;

typedef enum {
    DIREITAE,
    ESQUERDAE
} DirecaoE;


// --- Estrutura do Inimigo ---
typedef struct {
    float x, y;
    float health;
    bool active;        // Para saber se este inimigo está em uso
    EnemyState estado;
    float speed;
    int fire_timer;     // Cooldown individual para cada inimigo
    int death_timer;
    DirecaoE direcao;

    // Animações
    ALLEGRO_BITMAP* sheets[2]; // 0 = WalkAndShoot, 1 = Death
    int frame_widths[2];
    int frame_heights[2];
    int total_frames[2];
    int frame_delays[2];
    int frame_atual;
    int contador_frames;

} Enemy;

// --- Funções Públicas ---

// Cria um inimigo em uma posição X inicial
Enemy* enemy_create(float start_x);

// Destrói o inimigo, liberando a memória
void enemy_destroy(Enemy* enemy);

// controla movimento e tiro
void enemy_update(Enemy* enemy, Player* player, bullet** all_shots_list_head, float dt);

// Desenha o inimigo na tela, considerando a câmera e a escala
void enemy_draw(Enemy* enemy, float camera_x);

void try_spawn_enemy(Enemy* inimigos[], int max, float camera_x, int screen_w, int *spawned_count);

#endif