#include "enemy.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>      


// --- Funções Internas (static) ---
// Funções auxiliares que só serão usadas dentro deste arquivo.
static void enemy_update_animation(Enemy* e);
static void enemy_shoot(Enemy* e, Player* p, bullet** all_shots_list_head);


Enemy* enemy_create(float start_x) {
    Enemy* e = (Enemy*) malloc(sizeof(Enemy));
    if (!e) return NULL;

    // Inicialização dos dados
    e->x = start_x;
    e->y = BACKGROUND_GROUND_Y; // Usa a constante global do chão
    e->health = ENEMY_HEALTH;
    e->active = true;
    e->estado = ENEMY_STATE_WALK_AND_SHOOT;
    e->speed = ENEMY_SPEED;
    e->direcao = ESQUERDAE; 
    
    // Randomiza o timer inicial para que os inimigos não atirem todos em sincronia
    srand(time(NULL)); 
    e->fire_timer = rand() % ENEMY_FIRE_RATE;

    e->death_timer = 60; // Define 1 segundo (60 frames) para o corpo sumir

    e->frame_atual = 0;
    e->contador_frames = 0;

    // Dados de Animação
    const char* paths[] = { "assets/sprites/enemies/inimigo19x21.png", "assets/sprites/enemies/inimigoMorto19x9.png" };
    e->frame_widths[ENEMY_STATE_WALK_AND_SHOOT] = 21;
    e->frame_heights[ENEMY_STATE_WALK_AND_SHOOT] = 19;
    e->total_frames[ENEMY_STATE_WALK_AND_SHOOT] = TOTAL_FRAMES_ENEMY_WALK_SHOOT;
    e->frame_delays[ENEMY_STATE_WALK_AND_SHOOT] = FRAME_DELAY_ENEMY_WALK_SHOOT;

    e->frame_widths[ENEMY_STATE_DEATH] = 19;
    e->frame_heights[ENEMY_STATE_DEATH] = 9;
    e->total_frames[ENEMY_STATE_DEATH] = TOTAL_FRAMES_ENEMY_DEATH;
    e->frame_delays[ENEMY_STATE_DEATH] = FRAME_DELAY_ENEMY_DEATH;

    // Carrega as imagens
    e->sheets[ENEMY_STATE_WALK_AND_SHOOT] = al_load_bitmap(paths[0]);
    e->sheets[ENEMY_STATE_DEATH] = al_load_bitmap(paths[1]);

    if (!e->sheets[ENEMY_STATE_WALK_AND_SHOOT] || !e->sheets[ENEMY_STATE_DEATH]) {
        fprintf(stderr, "Falha ao carregar sprite do inimigo.\n");
        enemy_destroy(e);
        return NULL;
    }
    
    return e;
}

void enemy_destroy(Enemy* enemy) {
    if (!enemy) return;
    if (enemy->sheets[0]) al_destroy_bitmap(enemy->sheets[0]);
    if (enemy->sheets[1]) al_destroy_bitmap(enemy->sheets[1]);
    free(enemy);
}

void enemy_update(Enemy* e, Player* p, bullet** all_shots_list_head, float dt) {
    if (!e || !e->active) return;

    // --- Verifica se morreu ---
    if (e->health <= 0 && e->estado != ENEMY_STATE_DEATH) {
        e->estado = ENEMY_STATE_DEATH;
        e->frame_atual = 0;
        e->contador_frames = 0;
    }

    if (e->estado == ENEMY_STATE_DEATH) {
        enemy_update_animation(e);
        if (e->death_timer > 0) {
            e->death_timer--; // Faz a contagem regressiva
        }
        return;
    }

    // --- Se estiver morrendo, só atualiza animação ---
    if (e->estado == ENEMY_STATE_DEATH) {
        enemy_update_animation(e);
        return;
    }

    // --- Atualiza direção (vira para o jogador) ---
    if (p->x < e->x) {
        e->direcao = ESQUERDAE;
    } else {
        e->direcao = DIREITAE;
    }

    // --- Movimento automático para a esquerda ---
    e->x -= e->speed * dt;

    // --- Tiro ---
    e->fire_timer++;
    if (e->fire_timer >= ENEMY_FIRE_RATE) {
        bool olhando_para_jogador =
            (e->direcao == ESQUERDAE && p->x < e->x) ||
            (e->direcao == DIREITAE && p->x > e->x);

        if (olhando_para_jogador) {
            enemy_shoot(e, p, all_shots_list_head);
            e->fire_timer = 0;
        }
    }

    // --- Animação ---
    enemy_update_animation(e);
}


// Lógica de desenho com escala, consistente com o jogador
void enemy_draw(Enemy* e, float camera_x) {
    if (!e || !e->active) return;
    
    ALLEGRO_DISPLAY* disp = al_get_current_display();
    if(!disp) return;

    int tela_w = al_get_display_width(disp);
    int tela_h = al_get_display_height(disp);
    float escala_x = (float)tela_w / BACKGROUND_ORIGINAL_W;
    float escala_y = (float)tela_h / BACKGROUND_ORIGINAL_H;

    float largura_desenhada = e->frame_widths[e->estado] * ENEMY_SCALE * escala_x;
    float altura_desenhada  = e->frame_heights[e->estado] * ENEMY_SCALE * escala_y;
    
    float desenha_x = (e->x - camera_x) * escala_x;
    float desenha_y = (e->y - (e->frame_heights[e->estado] * ENEMY_SCALE)) * escala_y;

    ALLEGRO_BITMAP* sheet = e->sheets[e->estado];
    int sx = e->frame_atual * e->frame_widths[e->estado];
    int sy = 0;
    
    int flip = (e->direcao == ESQUERDAE) ? ALLEGRO_FLIP_HORIZONTAL : 0;
    
    // Use a variável 'flip' na chamada final
    al_draw_scaled_bitmap(sheet, sx, sy, 
                          e->frame_widths[e->estado], e->frame_heights[e->estado],
                          desenha_x, desenha_y, 
                          largura_desenhada, altura_desenhada, flip); 
}

static void enemy_update_animation(Enemy* e) {
    if (!e) return;

    int total_frames = e->total_frames[e->estado];
    
    // Trava a animação no último frame se for a de morte
    if (e->estado == ENEMY_STATE_DEATH && e->frame_atual == total_frames - 1) {
        return;
    }

    e->contador_frames++;
    if (e->contador_frames >= e->frame_delays[e->estado]) {
        e->contador_frames = 0;
        e->frame_atual = (e->frame_atual + 1) % total_frames;
    }
}

static void enemy_shoot(Enemy* e, Player* p, bullet** all_shots_list_head) {
    if (!e || !p) return;

    // Posição de início do tiro 
    float startX = e->x; 
    float startY = e->y - (e->frame_heights[e->estado] * ENEMY_SCALE / 2.0f);

    // Lógica de Mira no Jogador
    float dx = p->x - e->x;
    float dy = p->y - e->y;
    float distance = sqrt(dx*dx + dy*dy);
    
    float velX = 0, velY = 0;
    if (distance > 0) {
        velX = (dx / distance) * BULLET_SPEED;
        velY = (dy / distance) * BULLET_SPEED;
    } else {
        // Se a distância for 0, atira reto para a esquerda
        velX = -BULLET_SPEED;
    }
    
    // Cria o projétil do inimigo e o adiciona na lista global
    bullet* new_shot = bullet_create(startX, startY, velX, velY, OWNER_ENEMY, NULL);
    add_shot_to_list(all_shots_list_head, new_shot);
}

// Tenta spawnar o inimigo
void try_spawn_enemy(Enemy* inimigos[], int max, float camera_x, int screen_w, int *spawned_count) {
    for (int i = 0; i < max; i++) {
        // Encontra o primeiro "espaço" vazio para um novo inimigo
        if (inimigos[i] == NULL) {
            float spawn_x = camera_x + screen_w + 1390.0f; // Um pouco pra fora da tela
            inimigos[i] = enemy_create(spawn_x);
            if (inimigos[i]) {
                (*spawned_count)++;
            }
            return; // Cria apenas um por vez e sai
        }
    }
}