#ifndef PLAYER_H
#define PLAYER_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include "joystick.h" // Usado diretamente em player_update_movement
#include "Pistol.h"
#include "projectile.h"


// --- Constantes do Jogador ---
#define PLAYER_SCALE 8.0f 
#define TOTAL_FRAMES_AGACHADO 1 
#define TOTAL_FRAMES_PULANDO 1 
#define TOTAL_FRAMES_PARADO 6 
#define TOTAL_FRAMES_ANDANDO 8
#define TOTAL_FRAMES_ATIRA_PARADO 2
#define TOTAL_FRAMES_ATIRA_ANDANDO 8
#define TOTAL_FRAMES_ATIRA_AGACHADO 2
#define TOTAL_FRAMES_ATIRA_PULANDO 2
#define TOTAL_FRAMES_ATIRA_PULANDO_CIMA 2
#define TOTAL_FRAMES_ATIRA_PULANDO_BAIXO 2
#define TOTAL_FRAMES_ATIRA_PARADO_CIMA 2
#define TOTAL_FRAMES_ATIRA_ANDANDO_CIMA 8
#define TOTAL_FRAMES_MORTE 5

#define FRAME_DELAY_AGACHADO 1
#define FRAME_DELAY_PULANDO 1
#define FRAME_DELAY_PARADO 15
#define FRAME_DELAY_ANDANDO 10
#define FRAME_DELAY_ATIRA_PARADO 7
#define FRAME_DELAY_ATIRA_ANDANDO 10
#define FRAME_DELAY_ATIRA_ANDANDO_CIMA 10
#define FRAME_DELAY_ATIRA_PARADO_CIMA 7
#define FRAME_DELAY_ATIRA_AGACHADO 7
#define FRAME_DELAY_ATIRA_PULANDO 7
#define FRAME_DELAY_ATIRA_PULANDO_CIMA 7
#define FRAME_DELAY_ATIRA_PULANDO_BAIXO 7
#define FRAME_DELAY_ANDANDO 10   // 10
#define FRAME_DELAY_MORTE 17


#define PLAYER_HORIZONTAL_OFFSET_RATIO 2.0f // 2.0f = centro da tela
#define PLAYER_FIRE_RATE 10 // Atira a cada 10 frames (6 tiros por segundo a 60 FPS)

#define BACKGROUND_GROUND_Y 850.0f
#define BACKGROUND_ORIGINAL_W 2816.0f
#define BACKGROUND_ORIGINAL_H 1385.0f
#define PLAYER_SPEED_PER_SEC 350.0f

#define PLAYER_MAX_STAMINA 100.0f
#define PLAYER_STAMINA_COST_PER_SHOT 20.0f // Custo para atirar
#define PLAYER_STAMINA_REGEN_RATE 15.0f  // Pontos de estamina regenerados por segundo


#define NUM_ESTADOS 13


#define BURN 5


// Estados de animação
typedef enum {
    PARADO,   // Sem movimento
    ANDANDO,   // Movimento lateral
    PULANDO,
    AGACHADO,
    ATIRA_PARADO,
    ATIRA_ANDANDO,
    ATIRA_AGACHADO,
    ATIRA_ANDANDO_CIMA,
    ATIRA_PARADO_CIMA,
    PULANDO_ATIRANDO,
    PULANDO_ATIRANDO_CIMA,
    PULANDO_ATIRANDO_BAIXO,
    MORTE
} Estado;

// Direção do jogador
typedef enum {
    DIREITA,
    ESQUERDA,
    CIMA,
    BAIXO
} Direcao;

// Estrutura do jogador
typedef struct {
    float x, y;
    Estado estado;
    Direcao direcao;
    

    // Animações
    ALLEGRO_BITMAP* sheets[NUM_ESTADOS];
    int frame_widths[NUM_ESTADOS];
    int frame_heights[NUM_ESTADOS];
    int total_frames[NUM_ESTADOS];
    int frame_delays[NUM_ESTADOS];

    int frame_atual;
    int contador_frames;
    int invincibility_timer;

    // Física
    float vel_y;      // Velocidade vertical
    bool no_chao;     // Está no chão?

    // Equipamento
    pistol *gun;
    int fire_timer;

    // Vida
    float health;       
    float max_health;   

    // Estamina
    float stamina;
    float max_stamina;

    // Modificação
    bool sofreuPrimeiroDano;
    float danoPorSeg;
    int timer;
} Player;

// Funções públicas
Player* player_create();
void player_destroy(Player* p);
void player_update_animation(Player* p);
void player_draw(Player* p, ALLEGRO_DISPLAY* display, float camera_x);
void player_update_movement(Player* p, joystick* controle, bullet** all_shots_list_head, float dt);
void player_shoot(Player* p, joystick* controle, bullet** all_shots_list_head);

#endif
