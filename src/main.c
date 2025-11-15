#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h> // Necessário para desenhar primitivas (como os projéteis)

// Seus cabeçalhos
#include "game.h"
#include "player.h"    
#include "joystick.h"
#include "projectile.h" 
#include "enemy.h"      
#include "boss.h"

// Definições do Jogo
#define INICIAL_LARGURA 1280
#define INICIAL_ALTURA 720
#define FPS 60.0

Enemy* inimigos[MAX_INIMIGOS];
bullet* all_shots_list = NULL;

// --- Função de limpeza geral ---
void limpar_recursos(
    ALLEGRO_DISPLAY *disp, ALLEGRO_TIMER *timer, ALLEGRO_EVENT_QUEUE *queue,
    ALLEGRO_FONT *menu_font, ALLEGRO_FONT *intr_font,
    ALLEGRO_BITMAP *menu_background_img, ALLEGRO_BITMAP *game_background_img, ALLEGRO_BITMAP *projectile_sprite, ALLEGRO_BITMAP *projectile_sprite2,
    Player *player, joystick *controle)
{
    // Destrói os recursos na ordem inversa da criação
    if (controle) joystick_destroy(controle);
    if (player) player_destroy(player); 
    if (game_background_img) al_destroy_bitmap(game_background_img);
    if (menu_background_img) al_destroy_bitmap(menu_background_img);
    if (projectile_sprite) al_destroy_bitmap(projectile_sprite);
    if (projectile_sprite2) al_destroy_bitmap(projectile_sprite2);
    if (intr_font) al_destroy_font(intr_font);
    if (menu_font) al_destroy_font(menu_font);
    if (queue) al_destroy_event_queue(queue);
    if (timer) al_destroy_timer(timer);
    if (disp) al_destroy_display(disp);
}

int main() {
    // --- Variáveis de Inicialização da Allegro ---
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_EVENT_QUEUE* queue = NULL;
    ALLEGRO_DISPLAY *disp = NULL;
    ALLEGRO_FONT *menu_font = NULL, *intr_font = NULL;
    ALLEGRO_BITMAP *menu_background_img = NULL, *game_background_img = NULL, *projectile_sprite = NULL, *projectile_sprite2 = NULL;

    // --- Inicialização da Allegro e Addons ---
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar a Allegro.\n"); return -1;
    }
    if (!al_install_keyboard()) {
        fprintf(stderr, "Falha ao instalar o teclado.\n"); return -1;
    }
    al_init_font_addon();
    if (!al_init_ttf_addon()) {
        fprintf(stderr, "Falha ao inicializar addon ttf.\n"); return -1;
    }
    if (!al_init_image_addon()) {
        fprintf(stderr, "Falha ao inicializar o addon de imagem.\n"); return -1;
    }
    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Falha ao inicializar o addon de primitivas.\n"); return -1;
    }

    // --- Criação de Recursos da Allegro ---
    timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        fprintf(stderr, "Falha ao criar o timer.\n"); return -1;
    }
    
    // Configura a janela para ser redimensionável
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    disp = al_create_display(INICIAL_LARGURA, INICIAL_ALTURA);
    if (!disp) {
        fprintf(stderr, "Falha ao criar a janela.\n");
        limpar_recursos(NULL, timer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        return -1;
    }
    al_set_window_title(disp, "Run 'n Gun");

    queue = al_create_event_queue();
    if (!queue) {
        fprintf(stderr, "Falha ao criar a fila de eventos.\n");
        limpar_recursos(disp, timer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        return -1;
    }

    // --- Carregamento de Assets ---
    menu_font = al_load_font("assets/background/Daydream.ttf", 36, 0);
    intr_font = al_load_font("assets/background/Daydream.ttf", 16, 0);
    menu_background_img = al_load_bitmap("assets/background/BackMenu.png");
    game_background_img = al_load_bitmap("assets/background/BackJogo.png");
    projectile_sprite = al_load_bitmap("assets/sprites/player/tiro.png");
    projectile_sprite = al_load_bitmap("assets/sprites/player/tiro.png");
    projectile_sprite2 = al_load_bitmap("assets/sprites/enemies/tiro2.png");

    if (!menu_font || !intr_font || !menu_background_img || !game_background_img || !projectile_sprite) {
        fprintf(stderr, "Erro ao carregar recursos (fonte ou imagem).\n");
        limpar_recursos(disp, timer, queue, menu_font, intr_font, menu_background_img, game_background_img, projectile_sprite, projectile_sprite2, NULL, NULL);
        return -1;
    }

    // --- Registro de Fontes de Eventos ---
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    // --- Variáveis de Estado do Jogo ---
    Gerenciador_Estados estado = MENU;
    Player* player = NULL;
    joystick* controle = NULL;
    Boss* boss = NULL;
    int menu_selection = 1;
    int pause_selection = 1;
    int gameover_selection = 1;
    int victory_selection = 1;
    float camera_x = 0.0f;
    bool done = false, redraw = true;
    GamePhase game_phase = PHASE_ENEMIES;
    int enemies_spawned_count = 0;        
    int enemies_defeated_count = 0;    


    for (int i = 0; i < MAX_INIMIGOS; i++) {
        inimigos[i] = NULL;
    }


    al_start_timer(timer);

    // ===================================================================================
    // ---                             LOOP PRINCIPAL                                ---
    // ===================================================================================

    while (!done) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        // --- Lógica de Eventos ---
        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                if (estado == GAMEPLAY && player) {
                    //  LÓGICA DE SPAWN             
                    //  Spawner de Inimigos 
                    // funciona durante a fase de inimigos E se ainda não criamos todos os 6
                    if (game_phase == PHASE_ENEMIES && enemies_spawned_count < MAX_INIMIGOS) {
                        static int spawn_timer = 0;
                        spawn_timer++;
                        if (spawn_timer > 480) {
                            try_spawn_enemy(inimigos, MAX_INIMIGOS, camera_x, al_get_display_width(disp), &enemies_spawned_count);
                            spawn_timer = 0;
                        }
                    }

                    // --- Spawner do BOSS ---
                    // Checa se todos os inimigos foram criados E derrotados, E se o boss ainda não existe
                    if (game_phase == PHASE_ENEMIES && 
                        enemies_spawned_count >= MAX_INIMIGOS && 
                        enemies_defeated_count >= MAX_INIMIGOS &&
                        boss == NULL) {
                        
                        // Cria o boss voando, um pouco à frente da câmera
                        float spawn_x = camera_x + al_get_display_width(disp) - 1200;
                        float spawn_y = 580; // Altura em que ele vai voar
                        boss = boss_create(spawn_x, spawn_y);
                        
                        // Muda a fase do jogo para que os inimigos comuns não apareçam mais
                        game_phase = PHASE_BOSS;
                    }

                    // ATUALIZAR POSIÇÕES DE TODOS OS OBJETOS

                    float old_player_x = player->x;
                    player_update_movement(player, controle, &all_shots_list, 1.0 / FPS);
                    player_update_animation(player);

                    float old_enemy_x[MAX_INIMIGOS];
                    for (int i = 0; i < MAX_INIMIGOS; i++) {
                        if (inimigos[i] != NULL) {
                            old_enemy_x[i] = inimigos[i]->x;
                            enemy_update(inimigos[i], player, &all_shots_list, 1.0 / FPS);
                        }
                    }

                    for (bullet* b = all_shots_list; b != NULL; b = (bullet*)b->next) {
                        bullet_move(b, 1.0 / FPS);
                    }

                    float old_boss_x = 0;
                    if (boss && boss->active) {
                        old_boss_x = boss->x;
                        boss_update(boss, player, &all_shots_list, 1.0 / FPS);
                    }

                    // CHECAR COLISÕES E APLICAR CONSEQUÊNCIAS

                    // --- Colisão Física (Jogador vs Inimigos) ---
                    float player_w = player->frame_widths[player->estado] * PLAYER_SCALE;
                    float player_h = player->frame_heights[player->estado] * PLAYER_SCALE;
                    float player_box_y = player->y - player_h;
                    
                    for (int i = 0; i < MAX_INIMIGOS; i++) {
                        if (inimigos[i] && inimigos[i]->active && inimigos[i]->estado != ENEMY_STATE_DEATH) {
                            float enemy_w = inimigos[i]->frame_widths[inimigos[i]->estado] * ENEMY_SCALE;
                            float enemy_h = inimigos[i]->frame_heights[inimigos[i]->estado] * ENEMY_SCALE;
                            float enemy_box_y = inimigos[i]->y - enemy_h;
                            
                            if (check_collision(player->x, player_box_y, player_w, player_h, inimigos[i]->x, enemy_box_y, enemy_w, enemy_h)) {
                                player->x = old_player_x;
                                inimigos[i]->x = old_enemy_x[i];
                            }
                        }
                    }

                    // Jogador vs Boss
                    if (boss && boss->active && boss->estado != BOSS_STATE_DEATH) {
                        
                        // --- AJUSTE DA CAIXA DE COLISÃO DO BOSS ---
                        const float PADDING_X = 550.0f; // Adiciona na largura total 
                        const float PADDING_Y = 40.0f; // Adiciona  na altura total 

                        // Calcula o tamanho base do sprite
                        float base_boss_w = boss->frame_widths[boss->estado] * BOSS_SCALE;
                        float base_boss_h = boss->frame_heights[boss->estado] * BOSS_SCALE;

                        // Aplica o padding para criar a caixa de colisão final, maior
                        float boss_collision_w = base_boss_w + PADDING_X;
                        float boss_collision_h = base_boss_h + PADDING_Y;
                        
                        // Recalcula o canto superior esquerdo baseado na nova caixa maior, para mantê-la centralizada
                        float boss_box_x = boss->x - (boss_collision_w / 2.0f);
                        float boss_box_y = boss->y - (boss_collision_h / 2.0f);
                        
                        if (check_collision(player->x, player_box_y, player_w, player_h, boss_box_x, boss_box_y, boss_collision_w, boss_collision_h)) {
                            player->x = old_player_x;
                            boss->x = old_boss_x;
                        }
                    }

                    // --- Colisão de Dano e Remoção de Projéteis ---
                    bullet* prev = NULL;
                    bullet* current = all_shots_list;
                    bullet* next = NULL;

                    while (current != NULL) {
                        next = (bullet*)current->next;
                        bool shot_removed = false;
                        
                        float bullet_w = 10, bullet_h = 10; // Tamanho da caixa de colisão da bala

                        // Calcula o canto superior esquerdo do projétil a partir de seu centro (current->x, current->y)
                        float bullet_box_x = current->x - (bullet_w / 2.0f);
                        float bullet_box_y = current->y - (bullet_h / 2.0f);
                        bool hit_an_enemy = false;

                        // Se o projétil é de um INIMIGO, checa colisão com o JOGADOR
                        if (current->owner == OWNER_ENEMY) {
                            if (check_collision(player->x, player_box_y, player_w, player_h, bullet_box_x, bullet_box_y, bullet_w, bullet_h)) {
                                if (player->invincibility_timer <= 0) {
                                    player->health -= 10.0f;               // DANO QUE O PLAYER RECEBE
                                    player->invincibility_timer = 120;     // TEMPINHO QUE O PLAYER FICA INVENCÍVEL
                                }
                                shot_removed = true;
                            }
                        } else if (current->owner == BOSS) {
                            if (check_collision(player->x, player_box_y, player_w, player_h, bullet_box_x, bullet_box_y, bullet_w, bullet_h)) {
                                if (player->invincibility_timer <= 0) {
                                    if (!player->sofreuPrimeiroDano) {
                                        player->health -= 10.0f;               // DANO QUE O PLAYER RECebe
                                        //layer->sofreuPrimeiroDano = true;
                                        player->danoPorSeg += 15.0f;
                                        player->invincibility_timer = 120;     // TEMPINHO QUE O PLAYER FICA INVENCÍVEL

                                    }
                                }
                                shot_removed = true;
                            }
                        }

                        if (player->danoPorSeg > 0) {
                            player->timer++;
                            if(player->timer >= 60) {
                                player->health -= BURN;
                                player->danoPorSeg -= BURN;
                                player->timer = 0;
                            }
                        }

                        // Se o projétil é do JOGADOR, checa colisão com TODOS os inimigos
                        else if (current->owner == OWNER_PLAYER) {
                            for (int i = 0; i < MAX_INIMIGOS; i++) {
                                if (inimigos[i] && inimigos[i]->active && inimigos[i]->estado != ENEMY_STATE_DEATH) {
                                    float enemy_w = inimigos[i]->frame_widths[inimigos[i]->estado] * ENEMY_SCALE;
                                    float enemy_h = inimigos[i]->frame_heights[inimigos[i]->estado] * ENEMY_SCALE;
                                    float enemy_box_y = inimigos[i]->y - enemy_h;
                                    
                                    if (check_collision(inimigos[i]->x, enemy_box_y, enemy_w, enemy_h, bullet_box_x, bullet_box_y, bullet_w, bullet_h)) {
                                        inimigos[i]->health -= 25.0f;
                                        shot_removed = true;
                                        hit_an_enemy = true;
                                        break;
                                    }
                                }
                            }
                        }

                        // Se não acertou um inimigo comum, checa o boss
                        if (!hit_an_enemy && boss && boss->active && boss->estado != BOSS_STATE_DEATH) {
                            float boss_w = boss->frame_widths[boss->estado] * BOSS_SCALE;
                            float boss_h = boss->frame_heights[boss->estado] * BOSS_SCALE;
                            float boss_box_y = boss->y - (boss_h / 2.0f);
                            float boss_box_x = boss->x - (boss_w / 2.0f);
                            if (check_collision(boss_box_x, boss_box_y, boss_w, boss_h, bullet_box_x, bullet_box_y, bullet_w, bullet_h)) {
                                boss->health -= 20.0f; // Dano do tiro do jogador no chefe
                                shot_removed = true;
                            }
                        }

                        // Se projétil saiu da tela
                        int screen_w = al_get_display_width(disp);
                        if (!shot_removed && (current->x < camera_x - 1500 || current->x > camera_x + screen_w + 1500)) {
                            shot_removed = true;
                        }

                        // Se precisa remover o projétil
                        if (shot_removed) {
                            if (prev == NULL) all_shots_list = next;
                            else prev->next = (struct bullet*) next;
                            bullet_destroy(current);
                        } else {
                            prev = current;
                        }
                        current = next;
                    }


                    // LÓGICA DE ESTADO FINAL E CÂMERA

                    // --- Lógica de Morte do Jogador ---
                    if (player->health <= 0 && player->estado != MORTE) {
                        player->estado = MORTE;
                        player->frame_atual = 0;
                        player->contador_frames = 0;
                    }

                    if (player->estado == MORTE && player->frame_atual == TOTAL_FRAMES_MORTE - 1) {
                        estado = GAME_OVER;
                    }
                    
                    // --- Lógica de Remoção dos Inimigos Comuns ---
                    for (int i = 0; i < MAX_INIMIGOS; i++){
                        if(inimigos[i] != NULL && 
                           inimigos[i]->estado == ENEMY_STATE_DEATH && 
                           inimigos[i]->death_timer <= 0) {
                            
                            enemies_defeated_count++;
                            enemy_destroy(inimigos[i]);
                            inimigos[i] = NULL;
                        }
                    }

                    // INÍCIO DA LÓGICA DE VITÓRIA 
                    // Se o chefe existe E está no estado de morte E seu timer de desaparecimento zerou...
                    if (boss && boss->estado == BOSS_STATE_DEATH && boss->death_timer <= 0) {
                        boss_destroy(boss);
                        boss = NULL;
                        estado = VICTORY;
                    }

                    //  ATUALIZAÇÃO DA CÂMERA         
                    int screen_w_cam = al_get_display_width(disp);
                    
                    // Fator de escala apenas para o eixo X, necessário para os cálculos
                    float escala_x_cam = (float)screen_w_cam / BACKGROUND_ORIGINAL_W;

                    // Defina as bordas da "zona morta" 
                    float left_boundary_px = screen_w_cam * 0.35f;
                    float right_boundary_px = screen_w_cam * 0.65f;

                    // Calcula a posição do jogador NA TELA (já considerando a escala)
                    float player_sprite_w = player->frame_widths[player->estado] * PLAYER_SCALE * escala_x_cam;
                    float player_pos_on_screen_x = (player->x - camera_x) * escala_x_cam;
                    float player_center_on_screen_x = player_pos_on_screen_x + (player_sprite_w / 2.0f);


                    // 3. Ajusta a câmera se o centro do jogador cruzar as bordas
                    if (player_center_on_screen_x > right_boundary_px) {
                        // Converte a borda da tela de volta para o "mundo" para ajustar a câmera
                        camera_x = player->x + (player_w/2.0f) - (right_boundary_px / escala_x_cam);
                    } else if (player_center_on_screen_x < left_boundary_px) {
                        camera_x = player->x + (player_w/2.0f) - (left_boundary_px / escala_x_cam);
                    }

                    // 4. Garante que a câmera não passe do início do nível
                    if (camera_x < 0) {
                        camera_x = 0;
                    } // Fim do if (estado == GAMEPLAY && player)
                }

                redraw = true;
                break;

            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                al_acknowledge_resize(event.display.source);
                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                switch(estado) {
                    case MENU:
                        if (event.keyboard.keycode == ALLEGRO_KEY_UP) menu_selection = 1;
                        else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) menu_selection = 2;
                        else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                            if (menu_selection == 1) {
                                // Cria os objetos do jogo ao iniciar
                                if (!player) player = player_create();
                                if (!controle) controle = joystick_create();
                                estado = GAMEPLAY;
                            } else {
                                done = true;
                            }
                        }
                        break;

                    case GAMEPLAY:
                        // Movimento (WASD)
                        if (event.keyboard.keycode == ALLEGRO_KEY_W) controle->up = true;
                        if (event.keyboard.keycode == ALLEGRO_KEY_A) controle->left = true;
                        if (event.keyboard.keycode == ALLEGRO_KEY_S) controle->down = true;
                        if (event.keyboard.keycode == ALLEGRO_KEY_D) controle->right = true;
                        // Tiro (Setas)
                        if (event.keyboard.keycode == ALLEGRO_KEY_UP) controle->up2 = true;
                        if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) controle->left2 = true;
                        if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) controle->down2 = true;
                        if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) controle->right2 = true;
                        
                        // Pausar o jogo
                        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                            estado = PAUSE;
                            pause_selection = 1; // Reseta a seleção do menu de pause
                        }

                        break;

                    case PAUSE:
                        if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                            pause_selection = (pause_selection == 1) ? 3 : pause_selection - 1;
                        } else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                            pause_selection = (pause_selection == 3) ? 1 : pause_selection + 1;
                        } else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                            if (pause_selection == 1) estado = GAMEPLAY;      // Voltar ao jogo
                            else if (pause_selection == 2) {
                                reset_game_state(&player, &controle, &camera_x, inimigos, &all_shots_list, &boss, &game_phase, &enemies_spawned_count, &enemies_defeated_count);
                                estado = MENU;      // Voltar ao menu
                            } 
                            else if (pause_selection == 3) done = true;        // Sair do Jogo
                        } else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                            estado = GAMEPLAY; // Também volta ao jogo com ESC
                        }
                        break;
                    
                    case GAME_OVER:
                        if (event.keyboard.keycode == ALLEGRO_KEY_UP) gameover_selection = 1;
                        else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) gameover_selection = 2;
                        else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                            if (gameover_selection == 1) { // Opção "Voltar ao Menu"
                                reset_game_state(&player, &controle, &camera_x, inimigos, &all_shots_list, &boss, &game_phase, &enemies_spawned_count, &enemies_defeated_count);
                                estado = MENU;
                            } else { // Opção "Sair"
                                done = true;
                            }
                        }
                        break;
                    
                    case VICTORY:
                        if (event.keyboard.keycode == ALLEGRO_KEY_UP) victory_selection = 1;
                        else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) victory_selection = 2;
                        else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                            if (victory_selection == 1) {
                                reset_game_state(&player, &controle, &camera_x, inimigos, &all_shots_list, &boss, &game_phase, &enemies_spawned_count, &enemies_defeated_count);
                                estado = MENU;      // Voltar ao menu
                            } else {
                                done = true;
                            }
                        }
                        break;
                }

                break;

            case ALLEGRO_EVENT_KEY_UP:
                if (estado == GAMEPLAY) {
                    // Movimento (WASD)
                    if (event.keyboard.keycode == ALLEGRO_KEY_W) controle->up = false;
                    if (event.keyboard.keycode == ALLEGRO_KEY_A) controle->left = false;
                    if (event.keyboard.keycode == ALLEGRO_KEY_S) controle->down = false;
                    if (event.keyboard.keycode == ALLEGRO_KEY_D) controle->right = false;
                    // Tiro (Setas)
                    if (event.keyboard.keycode == ALLEGRO_KEY_UP) controle->up2 = false;
                    if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) controle->left2 = false;
                    if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) controle->down2 = false;
                    if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) controle->right2 = false;
                }
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        // --- Seção de Desenho ---
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;

            switch (estado) {
                case MENU:
                    draw_menu(menu_background_img, menu_font, intr_font, menu_selection);
                    break;

                case GAMEPLAY:
                    // Desenha o fundo com parallax scroll
                    draw_gameplay(game_background_img, camera_x);
                    
                    // Desenha cada inimigo vivo
                    for (int i = 0; i < MAX_INIMIGOS; i++) {
                        if (inimigos[i] != NULL) {
                            enemy_draw(inimigos[i], camera_x);
                        }
                    }
                    
                    // Desenha o jogador (por cima dos inimigos)
                    if (player) player_draw(player, disp, camera_x);
                    
                    // Desenha cada projétil da lista global
                    for (bullet* b = all_shots_list; b != NULL; b = (bullet*)b->next) {
                        
                        //  Escolher o Sprite e o Tamanho 
                        ALLEGRO_BITMAP* sprite_para_usar = NULL;
                        float bullet_world_w, bullet_world_h;

                        if (b->owner == OWNER_PLAYER) {
                            sprite_para_usar = projectile_sprite;
                            bullet_world_w = 10; bullet_world_h = 10;
                        } 
                        else if (b->owner == BOSS) {
                            sprite_para_usar = projectile_sprite2; // Sprite do chefe
                            bullet_world_w = BOSS_BULLET_SIZE;
                            bullet_world_h = BOSS_BULLET_SIZE;
                        }
                        else { // OWNER_ENEMY
                            sprite_para_usar = projectile_sprite; // Inimigo comum usa o tiro pequeno
                            bullet_world_w = 10; bullet_world_h = 10;
                        }

                        // Lógica de Flip (Virar a Imagem) ---
                        int flip_flags = 0; 

                        // Apenas vira a imagem se o tiro for do chefe E ele estiver virado para a direita
                        if (b->owner == BOSS && boss && boss->direcao == BOSS_DIR_RIGHT) {
                            flip_flags = ALLEGRO_FLIP_HORIZONTAL;
                        }
                        
                        // Chamar a Função de Desenho ---
                        draw_scaled_bullet(b, sprite_para_usar, camera_x, disp, bullet_world_w, bullet_world_h, flip_flags);
                    }

                    draw_life_bar(player);
                    draw_stamina_bar(player);

                    if (boss && boss->active) {
                        boss_draw(boss, camera_x);
                        draw_boss_life_bar(boss, disp);
                    }

                    break;

                case PAUSE:
                    // Desenha a cena do jogo por baixo
                    draw_gameplay(game_background_img, camera_x);
                    if (player) player_draw(player, disp, camera_x);
                    // E então o menu de pause por cima
                    draw_pause(game_background_img, menu_font, intr_font, pause_selection);
                    break;

                case GAME_OVER:
                    // Desenha a cena do jogo por baixo
                    draw_gameplay(game_background_img, camera_x);
                    draw_gameover(game_background_img, menu_font, intr_font, gameover_selection);
                    break;
                
                case VICTORY:
                    draw_victory(menu_background_img, menu_font, intr_font, victory_selection);
            }

            al_flip_display();
        }
    }

    // --- Liberação de Recursos ---
    limpar_recursos(disp, timer, queue, menu_font, intr_font, menu_background_img, game_background_img, projectile_sprite, projectile_sprite2, player, controle);
    return 0;
}