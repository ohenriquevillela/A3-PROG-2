#include <stdlib.h>
#include "player.h"

//--------------------------------------------------------------------------------------------

// Cria e inicializa o jogador
Player* player_create() {
    Player* p = (Player*) malloc(sizeof(Player));
    if (!p) return NULL;

    p->gun = pistol_create(); // Cria a arma aqui
    if (!p->gun) {
        player_destroy(p); // Se a arma não puder ser criada, limpe o jogador
        return NULL;
    }

    p->max_health = 200.0f;
    p->health = p->max_health; // Começa com a vida cheia
    p->invincibility_timer = 0; //  (começa vulnerável)
    p->sofreuPrimeiroDano = false;
    p->danoPorSeg = 0.0f;
    p->timer = 0;

    p->max_stamina = PLAYER_MAX_STAMINA;
    p->stamina = p->max_stamina; // Começa com estamina cheia

    p->fire_timer = 0;
    p->vel_y = 0;
    p->no_chao = true;

    p->x = 0;
    p->y = BACKGROUND_GROUND_Y;  
    p->estado = PARADO;
    p->direcao = DIREITA;
    p->frame_atual = 0;
    p->contador_frames = 0;

    // Dados de animação
    const char* caminhos_sprites[NUM_ESTADOS] = {
        "assets/sprites/player/sheetParado.png",
        "assets/sprites/player/sheetAndando.png",
        "assets/sprites/player/spritePulandoFrente.png",
        "assets/sprites/player/spriteAgachado.png",
        "assets/sprites/player/sheetAtiraParado.png",
        "assets/sprites/player/AtirandoAndando2.png",
        "assets/sprites/player/atiraAgachado.png",
        "assets/sprites/player/AtirandoAndandoCima.png",
        "assets/sprites/player/atiraParadoCima.png",
        "assets/sprites/player/PulaAtiraFrente.png",
        "assets/sprites/player/PulaAtiraCima.png",
        "assets/sprites/player/PulaAtiraBaixo.png",
        "assets/sprites/player/morte.png"
    };

    const int larguras[NUM_ESTADOS] = { 20, 24, 19, 19, 28, 30, 28, 23, 15, 28, 14, 15, 34 };
    const int alturas[NUM_ESTADOS]  = { 30, 30, 30, 23, 30, 30, 24, 43, 43, 31, 43, 30, 21 };
    
    const int totais[NUM_ESTADOS]   = { TOTAL_FRAMES_PARADO, TOTAL_FRAMES_ANDANDO, TOTAL_FRAMES_PULANDO, TOTAL_FRAMES_AGACHADO, TOTAL_FRAMES_ATIRA_PARADO, 
                                        TOTAL_FRAMES_ATIRA_ANDANDO, TOTAL_FRAMES_ATIRA_AGACHADO, TOTAL_FRAMES_ATIRA_ANDANDO_CIMA, TOTAL_FRAMES_ATIRA_PARADO_CIMA,
                                        TOTAL_FRAMES_ATIRA_PULANDO, TOTAL_FRAMES_ATIRA_PULANDO_CIMA, TOTAL_FRAMES_ATIRA_PULANDO_BAIXO, TOTAL_FRAMES_MORTE };

    const int delays[NUM_ESTADOS]   = { FRAME_DELAY_PARADO,  FRAME_DELAY_ANDANDO, FRAME_DELAY_PULANDO, FRAME_DELAY_AGACHADO, FRAME_DELAY_ATIRA_PARADO, 
                                        FRAME_DELAY_ATIRA_ANDANDO, FRAME_DELAY_ATIRA_AGACHADO, FRAME_DELAY_ATIRA_ANDANDO_CIMA, FRAME_DELAY_ATIRA_PARADO_CIMA,
                                        FRAME_DELAY_ATIRA_PULANDO, FRAME_DELAY_ATIRA_PULANDO_CIMA, FRAME_DELAY_ATIRA_PULANDO_BAIXO, FRAME_DELAY_MORTE };

    for (int i = 0; i < NUM_ESTADOS; i++) {
        p->sheets[i]         = al_load_bitmap(caminhos_sprites[i]);
        p->frame_widths[i]   = larguras[i];
        p->frame_heights[i]  = alturas[i];
        p->total_frames[i]   = totais[i];
        p->frame_delays[i]   = delays[i];

        if (!p->sheets[i]) {
            fprintf(stderr, "Falha ao carregar o spritesheet do estado %d.\n", i);
            player_destroy(p);
            return NULL;
        }
    }

    return p;
}

//--------------------------------------------------------------------------------------------

// Atualiza a animação do jogador conforme seu estado atual
void player_update_animation(Player* p) {
    if (!p) return;

    p->contador_frames++;

    int delay = p->frame_delays[p->estado];
    int total_frames = p->total_frames[p->estado];

    if (p->contador_frames >= delay) {
        p->contador_frames = 0;
        p->frame_atual = (p->frame_atual + 1) % total_frames;
    }
}

//--------------------------------------------------------------------------------------------

// Desenha o jogador na tela com base na resolução da tela e escala
void player_draw(Player* p, ALLEGRO_DISPLAY* display, float camera_x) {
    if (!p || !display) return;

    bool should_draw = true;
    
    // Se o jogador está invencível...
    if (p->invincibility_timer > 0) {
        if ((p->invincibility_timer / 6) % 2 == 0) {
            // ...cria a parte "invisível" do pisca-pisca.
            should_draw = false;
        }
    }

    // executa se should_draw' for verdadeira
    if (should_draw) {
        // Pega as dimensões da tela e calcula a escala
        int tela_w = al_get_display_width(display);
        int tela_h = al_get_display_height(display);
        float escala_x = (float)tela_w / BACKGROUND_ORIGINAL_W;
        float escala_y = (float)tela_h / BACKGROUND_ORIGINAL_H;

        // Calcula o tamanho final que o sprite do jogador terá na tela
        float largura_desenhada = p->frame_widths[p->estado] * PLAYER_SCALE * escala_x;
        float altura_desenhada  = p->frame_heights[p->estado] * PLAYER_SCALE * escala_y;

        float desenha_x = (p->x - camera_x) * escala_x;
        float desenha_y = (p->y - (p->frame_heights[p->estado] * PLAYER_SCALE)) * escala_y;

        // Pega o frame de animação correto
        ALLEGRO_BITMAP* sheet = p->sheets[p->estado];
        int sx = p->frame_atual * p->frame_widths[p->estado];
        int sy = 0;
        int flip = (p->direcao == ESQUERDA) ? ALLEGRO_FLIP_HORIZONTAL : 0;

        // Desenha o jogador com escala
        al_draw_scaled_bitmap(sheet, sx, sy,
                              p->frame_widths[p->estado], p->frame_heights[p->estado],
                              desenha_x, desenha_y,
                              largura_desenhada, altura_desenhada,
                              flip);
    }
}

//--------------------------------------------------------------------------------------------

// Atualiza o movimento e estado do jogador com base nos controles
void player_update_movement(Player* p, joystick* controle, bullet** all_shots_list_head, float dt) {
    if (p->invincibility_timer > 0) {
        p->invincibility_timer--;
    }
    if (p->estado == MORTE) {
        return;
    }
    if (!p || !controle) return;

    const float GRAVIDADE = 0.8f;
    const float PULO_FORCA = -18.0f;
    const float CHAO_Y = BACKGROUND_GROUND_Y;
    const float move_dist = PLAYER_SPEED_PER_SEC * dt;
    Estado estado_anterior = p->estado;

    // --- LÓGICA DE MOVIMENTO FÍSICO ---
    // Apenas atualiza a posição e a direção do jogador
    if (controle->right) {
        p->x += move_dist;
        p->direcao = DIREITA;
    } else if (controle->left) {
        p->x -= move_dist;
        p->direcao = ESQUERDA;
    }
    if (p->x < 10.0f) {
        p->x = 10.0f;
    }
    if (controle->up && p->no_chao) {
        p->vel_y = PULO_FORCA;
        p->no_chao = false;
    }
    if (!p->no_chao) {
        p->vel_y += GRAVIDADE;
        p->y += p->vel_y;
        if (p->y >= CHAO_Y) {
            p->y = CHAO_Y;
            p->vel_y = 0;
            p->no_chao = true;
        }
    }

    // --- LÓGICA DE ESTADO DE ANIMAÇÃO E TIRO  ---
    bool is_moving = controle->left || controle->right;
    bool shoot_up = controle->up2;
    // Só pode atirar para baixo se estiver no ar.
    bool shoot_down = controle->down2 && !p->no_chao; 
    // Só pode atirar na direção em que está virado.
    bool shoot_forward = (p->direcao == DIREITA && controle->right2) || (p->direcao == ESQUERDA && controle->left2);
    
    bool is_shooting = shoot_up || shoot_down || shoot_forward;

    // --- ESCOLHA DO ESTADO DE ANIMAÇÃO ---
    if (!p->no_chao) { // Lógica no Ar
        if (shoot_up) p->estado = PULANDO_ATIRANDO_CIMA;
        else if (shoot_down) p->estado = PULANDO_ATIRANDO_BAIXO;
        else if (shoot_forward) p->estado = PULANDO_ATIRANDO;
        else p->estado = PULANDO;
    } else { // Lógica no Chão
        if (is_moving) {
            if (shoot_up) p->estado = ATIRA_ANDANDO_CIMA;
            else if (shoot_forward) p->estado = ATIRA_ANDANDO;
            else p->estado = ANDANDO;
        } else if (controle->down) { // Agachado
            p->estado = shoot_forward ? ATIRA_AGACHADO : AGACHADO;
        } else { // Parado
            if (shoot_up) p->estado = ATIRA_PARADO_CIMA;
            else if (shoot_forward) p->estado = ATIRA_PARADO;
            else p->estado = PARADO;
        }
    }

    // --- LÓGICA DE ESTAMINA E TIRO CONTÍNUO ---
    if (is_shooting) {
        //  Só tenta atirar se tiver estamina suficiente 
        if (p->stamina >= PLAYER_STAMINA_COST_PER_SHOT) {
            p->fire_timer++;
            if (p->fire_timer >= PLAYER_FIRE_RATE) {
                p->fire_timer = 0;
                player_shoot(p, controle, all_shots_list_head);
            }
        }
    } else {
        // Se não está tentando atirar, a estamina se regenera
        p->fire_timer = 0; // Reseta o timer de tiro
        if (p->stamina < p->max_stamina) {
            p->stamina += PLAYER_STAMINA_REGEN_RATE * dt; // Regenera um pouco a cada frame
            // Garante que a estamina não ultrapasse o máximo
            if (p->stamina > p->max_stamina) {
                p->stamina = p->max_stamina;
            }
        }
    }

    // Reinicia a animação se o estado mudou
    if (p->estado != estado_anterior) {
        p->frame_atual = 0;
        p->contador_frames = 0;
    }
}



//--------------------------------------------------------------------------------------------

void player_shoot(Player* p, joystick* controle, bullet** all_shots_list_head) {
    if (!p || !controle || !all_shots_list_head) return;

    float velX = 0, velY = 0;
    bool atirou = false; // Começa como falso

    // --- VERIFICA AS CONDIÇÕES DE TIRO VÁLIDAS ---
    if (controle->up2 && !controle->down) { // Tiro para cima 
        velY = -BULLET_SPEED;
        atirou = true;
    } else if (controle->down2 && !p->no_chao) { // Tiro para baixo só é válido no ar
        velY = BULLET_SPEED;
        atirou = true;
    } else if (controle->right2 && p->direcao == DIREITA) { // Tiro para a direita só é válido se virado para a direita
        velX = BULLET_SPEED;
        atirou = true;
    } else if (controle->left2 && p->direcao == ESQUERDA) { // Tiro para a esquerda só é válido se virado para a esquerda
        velX = -BULLET_SPEED;
        atirou = true;
    }

    if (!atirou) return; // Se nenhuma condição válida foi encontrada, não cria a bala.

    // --- POSIÇÃO DE NASCIMENTO DO PROJÉTIL ---
    float altura_sprite = p->frame_heights[p->estado] * PLAYER_SCALE;
    float largura_sprite = p->frame_widths[p->estado] * PLAYER_SCALE;
    float startY = p->y - (altura_sprite / 1.55f); // Ajuste de onde nasce o sprite do player
    float centro_x_jogador = p->x + (largura_sprite / 2.0f);
    const float DISTANCIA_CENTRO_ARMA = largura_sprite / 2.0f;
    float startX = (p->direcao == DIREITA) ? (centro_x_jogador + DISTANCIA_CENTRO_ARMA) : (centro_x_jogador - DISTANCIA_CENTRO_ARMA);
    if (velY != 0) startX = centro_x_jogador;

    // --- CRIA O PROJÉTIL ---
    bullet* new_shot = bullet_create(startX, startY, velX, velY, OWNER_PLAYER, NULL);
    add_shot_to_list(all_shots_list_head, new_shot);

    p->stamina -= PLAYER_STAMINA_COST_PER_SHOT;
}


// Libera os recursos associados ao jogador
void player_destroy(Player* p) {
    if (!p) return;

    for (int i = 0; i < NUM_ESTADOS; i++) {
        if (p->sheets[i]) {
            al_destroy_bitmap(p->sheets[i]);
            p->sheets[i] = NULL;
        }
    }

    if (p->gun) {
        pistol_destroy(p->gun); 
    }

    free(p);
}
