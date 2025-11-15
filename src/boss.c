// Arquivo: boss.c

#include "boss.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Protótipos

#define BOSS_MOUTH_OFFSET_X 500.0f
#define BOSS_MOUTH_OFFSET_Y 25.0f

// Função de tiro corrigida
static void boss_shoot(Boss* b, Player* p, bullet** all_shots_list_head) {
    if (!b || !p) return;

    const float DISTANCIA_FRENTE = 500.0f;  // Distância para FRENTE (a partir do centro do boss)
    const float DISTANCIA_ALTURA = 25.0f;  // Distância para BAIXO (a partir do centro do boss)

    // 1. Calcula a posição inicial do tiro
    float startX = b->x;
    float startY = b->y + DISTANCIA_ALTURA; 

    // 2. Aplica o ajuste horizontal baseado na DIREÇÃO do chefe
    if (b->direcao == BOSS_DIR_RIGHT) {
        startX += DISTANCIA_FRENTE; // Se virado para a direita, SOMA para ir para frente
    } else { // Se virado para a esquerda
        startX -= DISTANCIA_FRENTE; // Se virado para a esquerda, SUBTRAI para ir para frente
    }

    // --- Mira no jogador ---
    float player_h = p->frame_heights[p->estado] * PLAYER_SCALE;
    float player_target_y = p->y - (player_h / 2.0f);
    float dx = p->x - startX; // Vetor de mira a partir do ponto de disparo
    float dy = player_target_y - startY;
    float distance = sqrt(dx*dx + dy*dy);
    
    float velX = 0, velY = 0;
    if (distance > 0) {
        velX = (dx / distance) * BULLET_SPEED;
        velY = (dy / distance) * BULLET_SPEED;
    } else {
        velX = (b->direcao == BOSS_DIR_LEFT) ? -BULLET_SPEED : BULLET_SPEED;
    }
    
    // --- Cria e adiciona o projétil ---
    bullet* new_shot = bullet_create(startX, startY, velX, velY, BOSS, NULL);
    add_shot_to_list(all_shots_list_head, new_shot);
}


// Função de animação 
static void boss_update_animation(Boss* b) {
    if (!b) return;

    int total_frames = b->total_frames[b->estado];

    if (b->estado == BOSS_STATE_DEATH && b->frame_atual == total_frames - 1) {
        return;
    }

    b->contador_frames++;
    if (b->contador_frames >= b->frame_delays[b->estado]) {
        b->contador_frames = 0;
        b->frame_atual = (b->frame_atual + 1) % total_frames;
    }
}



Boss* boss_create(float start_x, float start_y) {
    Boss* b = (Boss*) malloc(sizeof(Boss));
    if (!b) return NULL;

    b->x = start_x;
    b->y = start_y;
    b->health = BOSS_HEALTH;
    b->max_health = BOSS_HEALTH;
    b->active = true;
    b->estado = BOSS_STATE_ACTIVE;
    b->direcao = BOSS_DIR_LEFT;
    b->speed = BOSS_SPEED;
    b->fire_timer = rand() % BOSS_FIRE_RATE;
    b->death_timer = 120;
    b->frame_atual = 0;
    b->contador_frames = 0;

    // Carrega os spritesheets 
    b->sheets[BOSS_STATE_ACTIVE] = al_load_bitmap("assets/sprites/enemies/ATTACK_79x69.png"); 
    b->sheets[BOSS_STATE_DEATH] = al_load_bitmap("assets/sprites/enemies/DEATH_79X69.png");
    
    if (!b->sheets[BOSS_STATE_ACTIVE] || !b->sheets[BOSS_STATE_DEATH]) {
        fprintf(stderr, "Falha ao carregar sprite do boss.\n");
        boss_destroy(b);
        return NULL;
    }

    // Configura as animações
    b->frame_widths[BOSS_STATE_ACTIVE] = 79; 
    b->frame_heights[BOSS_STATE_ACTIVE] = 69;
    b->total_frames[BOSS_STATE_ACTIVE] = TOTAL_FRAMES_BOSS_ACTIVE;
    b->frame_delays[BOSS_STATE_ACTIVE] = FRAME_DELAY_BOSS_ACTIVE;

    b->frame_widths[BOSS_STATE_DEATH] = 79; 
    b->frame_heights[BOSS_STATE_DEATH] = 69;
    b->total_frames[BOSS_STATE_DEATH] = TOTAL_FRAMES_BOSS_DEATH;
    b->frame_delays[BOSS_STATE_DEATH] = FRAME_DELAY_BOSS_DEATH;
    
    return b;
}

void boss_destroy(Boss* boss) {
    if (!boss) return;
    if (boss->sheets[0]) al_destroy_bitmap(boss->sheets[0]);
    if (boss->sheets[1]) al_destroy_bitmap(boss->sheets[1]);
    free(boss);
}

void boss_update(Boss* b, Player* p, bullet** all_shots_list_head, float dt) {
    if (!b || !b->active || !p) return;

    // Checa e processa a morte
    if (b->health <= 0 && b->estado != BOSS_STATE_DEATH) {
        b->estado = BOSS_STATE_DEATH;
        b->frame_atual = 0;
        b->contador_frames = 0;
    }
    if (b->estado == BOSS_STATE_DEATH) {
        boss_update_animation(b);
        if (b->death_timer > 0) b->death_timer--;
        return; // Para tudo se estiver morrendo
    }

    // --- IA de Direção: Sempre encara o jogador ---
    if (p->x > b->x) {
        b->direcao = BOSS_DIR_RIGHT;
    } else {
        b->direcao = BOSS_DIR_LEFT;
    }

    // --- IA de Movimento Reativo ---
    float distance_to_player = fabs(p->x - b->x);
    if (distance_to_player > BOSS_IDEAL_DISTANCE) {
        if (b->direcao == BOSS_DIR_RIGHT) b->x += b->speed * dt;
        else b->x -= b->speed * dt;
    }
    else if (distance_to_player < BOSS_IDEAL_DISTANCE - 50) {
        if (b->direcao == BOSS_DIR_RIGHT) b->x -= b->speed * dt;
        else b->x += b->speed * dt;
    }
    
    // --- IA de Tiro ---
    b->fire_timer++;
    if (b->fire_timer >= BOSS_FIRE_RATE) {
        boss_shoot(b, p, all_shots_list_head);
        b->fire_timer = 0;
    }
    
    boss_update_animation(b);
}

// Função de desenho que estava faltando
void boss_draw(Boss* b, float camera_x) {
    if (!b || !b->active) return;
    
    ALLEGRO_DISPLAY* disp = al_get_current_display();
    if(!disp) return;

    int tela_w = al_get_display_width(disp);
    int tela_h = al_get_display_height(disp);
    float escala_x = (float)tela_w / BACKGROUND_ORIGINAL_W;
    float escala_y = (float)tela_h / BACKGROUND_ORIGINAL_H;

    float largura_desenhada = b->frame_widths[b->estado] * BOSS_SCALE * escala_x;
    float altura_desenhada  = b->frame_heights[b->estado] * BOSS_SCALE * escala_y;
    
    // Como o boss voa, seu 'y' pode ser o centro. Vamos calcular o topo para desenhar.
    float desenha_x = (b->x - camera_x) * escala_x;
    float desenha_y = (b->y - (b->frame_heights[b->estado] * BOSS_SCALE / 2.0f)) * escala_y;

    ALLEGRO_BITMAP* sheet = b->sheets[b->estado];
    int sx = b->frame_atual * b->frame_widths[b->estado];
    int sy = 0;
    int flip = (b->direcao == BOSS_DIR_RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
    
    al_draw_scaled_bitmap(sheet, sx, sy, 
                          b->frame_widths[b->estado], b->frame_heights[b->estado],
                          desenha_x, desenha_y, 
                          largura_desenhada, altura_desenhada, flip);
}


