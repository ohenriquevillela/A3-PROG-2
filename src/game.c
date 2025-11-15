#include "game.h" 
#include <allegro5/allegro5.h> // Para al_draw_bitmap

void draw_scaled_background(ALLEGRO_BITMAP* bg_image, float camera_x) {
    if (!bg_image) return;

    int img_w = al_get_bitmap_width(bg_image);
    int img_h = al_get_bitmap_height(bg_image);
    int disp_h = al_get_display_height(al_get_current_display());

    // Escala vertical para preencher altura da tela
    float scale = (float)disp_h / img_h;
    float draw_w = img_w * scale;
    float draw_h = img_h * scale;

    // Calcula o offset horizontal com wrap-around
    float scroll_x = fmod(camera_x, draw_w);  // Posição da imagem na tela

    if (scroll_x < 0) scroll_x += draw_w; // Corrige scroll negativo

    // Desenha a primeira cópia
    al_draw_scaled_bitmap(bg_image,
        0, 0, img_w, img_h,                 // fonte
        -scroll_x, 0, draw_w, draw_h,       // destino (posicionado com scroll)
        0);

    // Desenha a segunda cópia à direita para cobrir a tela
    al_draw_scaled_bitmap(bg_image,
        0, 0, img_w, img_h,
        -scroll_x + draw_w, 0, draw_w, draw_h,
        0);
}

void draw_menu(ALLEGRO_BITMAP* bg_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font_instr, int selected_opt) {
    // Obtém dimensões da tela
    int screen_w = al_get_display_width(al_get_current_display());
    int screen_h = al_get_display_height(al_get_current_display());

    // Desenhar a imagem de fundo proporcional e centralizada
    if (bg_image) {
        draw_scaled_background(bg_image, 0);
    } else {
        // Fallback: fundo preto
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }

    // Cores para os textos
    ALLEGRO_COLOR color_unselected = al_map_rgb(255, 255, 255); // Branco
    ALLEGRO_COLOR color_selected = al_map_rgb(255, 215, 0);     // Dourado

    // Desenhar o título do jogo
    al_draw_text(font,
                 color_unselected,
                 screen_w / 2,
                 screen_h / 4,
                 ALLEGRO_ALIGN_CENTER,
                 "RUN 'N GUN POGGERS");

    // Opção "Iniciar Jogo"
    al_draw_text(font,
                 (selected_opt == 1) ? color_selected : color_unselected,
                 screen_w / 2,
                 screen_h / 2,
                 ALLEGRO_ALIGN_CENTER,
                 "Iniciar Jogo");

    // Opção "Sair do Jogo"
    al_draw_text(font,
                 (selected_opt == 2) ? color_selected : color_unselected,
                 screen_w / 2,
                 screen_h / 2 + 50,
                 ALLEGRO_ALIGN_CENTER,
                 "Sair do Jogo");

    // Texto de instrução (fonte menor)
    al_draw_text(font_instr,
                 color_unselected,
                 screen_w / 2,
                 screen_h - 100,
                 ALLEGRO_ALIGN_CENTER,
                 "Use as setas para navegar e ENTER para confirmar");
}

void draw_gameplay(ALLEGRO_BITMAP* bg_image, float camera_x) {
    if (bg_image) {
        draw_scaled_background(bg_image, camera_x);
    } else {
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }
}

void draw_pause(ALLEGRO_BITMAP* bg_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font_instr, int selected_opt) {
    // Obtém dimensões da tela
    int screen_w = al_get_display_width(al_get_current_display());
    int screen_h = al_get_display_height(al_get_current_display());

    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Falha ao inicializar o addon de primitivas.\n");
        return;
    }

    al_init_primitives_addon();

    // Desenhar a imagem de fundo proporcional e centralizada
    if (bg_image) {
        draw_scaled_background(bg_image, 0);
        al_draw_filled_rectangle(0, 0, screen_w, screen_h, al_map_rgba(0, 0, 0, 128));  // semitransparente
    } else {
        // Fallback: fundo preto
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }

    // Cores para os textos
    ALLEGRO_COLOR color_unselected = al_map_rgb(255, 255, 255); // Branco
    ALLEGRO_COLOR color_selected = al_map_rgb(255, 215, 0);     // Dourado

    // Desenhar o título do MENU
    al_draw_text(font,
                 color_unselected,
                 screen_w / 2,
                 screen_h / 6,
                 ALLEGRO_ALIGN_CENTER,
                 "RUN 'N GUN POGGERS");

    // Opção "Voltar Jogo"
    al_draw_text(font,
                 (selected_opt == 1) ? color_selected : color_unselected,
                 screen_w / 2,
                 screen_h / 2,
                 ALLEGRO_ALIGN_CENTER,
                 "Voltar ao jogo");
                 
    // Opção "Voltar Jogo"
    al_draw_text(font,
                 (selected_opt == 2) ? color_selected : color_unselected,
                 screen_w / 2,
                 screen_h / 2 + 50,
                 ALLEGRO_ALIGN_CENTER,
                 "Voltar ao Menu");

    // Opção "Sair do Jogo"
    al_draw_text(font,
                 (selected_opt == 3) ? color_selected : color_unselected,
                 screen_w / 2,
                 screen_h / 2 + 100,
                 ALLEGRO_ALIGN_CENTER,
                 "Sair do Jogo");

    // Texto de instrução (fonte menor)
    al_draw_text(font_instr,
                 color_unselected,
                 screen_w / 2,
                 screen_h - 100,
                 ALLEGRO_ALIGN_CENTER,
                 "Use as setas para navegar e ENTER para confirmar");

}

void draw_scaled_bullet(bullet* b, ALLEGRO_BITMAP* sprite, float camera_x, ALLEGRO_DISPLAY* disp, float world_w, float world_h, int flip_flags) {
    if (!b || !sprite || !disp) return;

    int tela_w = al_get_display_width(disp);
    int tela_h = al_get_display_height(disp);
    float escala_x = (float)tela_w / BACKGROUND_ORIGINAL_W;
    float escala_y = (float)tela_h / BACKGROUND_ORIGINAL_H;

    float bullet_top_left_x = b->x - (world_w / 2.0f);
    float bullet_top_left_y = b->y - (world_h / 2.0f);
    
    float draw_x = (bullet_top_left_x - camera_x) * escala_x;
    float draw_y = bullet_top_left_y * escala_y;
    
    float largura_escalada = world_w * escala_x;
    float altura_escalada = world_h * escala_y;

    // Desenha o projétil usando as coordenadas, tamanhos E A FLAG DE FLIP CORRETOS
    al_draw_scaled_bitmap(sprite,
                          0, 0,
                          al_get_bitmap_width(sprite), al_get_bitmap_height(sprite),
                          draw_x, draw_y,
                          largura_escalada, altura_escalada,
                          flip_flags); // <<< USA A FLAG RECEBIDA
}

void draw_life_bar(Player *player) {
    // --- Configurações da Barra de Vida ---
    const float BARRA_X = 20;           // Posição X no canto da tela
    const float BARRA_Y = 20;          // Posição Y no canto da tela
    const float BARRA_LARGURA = 250;  // Largura total da barra
    const float BARRA_ALTURA = 30;   // Altura da barra

    // Calcula a porcentagem de vida 
    float porcentagem_vida = player->health / player->max_health;
    if (porcentagem_vida < 0) porcentagem_vida = 0; 

     // Calcula a largura da parte verde da barra
    float largura_vida_atual = BARRA_LARGURA * porcentagem_vida;

    // 1. Desenha o fundo da barra (vermelho)
    al_draw_filled_rectangle(BARRA_X, BARRA_Y, BARRA_X + BARRA_LARGURA, BARRA_Y + BARRA_ALTURA, al_map_rgb(200, 0, 0));

    // 2. Desenha a vida atual (verde) por cima do fundo
    if (largura_vida_atual > 0) {
        al_draw_filled_rectangle(BARRA_X, BARRA_Y, BARRA_X + largura_vida_atual, BARRA_Y + BARRA_ALTURA, al_map_rgb(0, 200, 0));
    }

     // Desenha uma borda preta para dar acabamento
    al_draw_rectangle(BARRA_X, BARRA_Y, BARRA_X + BARRA_LARGURA, BARRA_Y + BARRA_ALTURA, al_map_rgb(0, 0, 0), 2.0f);

}

void draw_gameover(ALLEGRO_BITMAP* bg_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font_instr, int selected_opt) {
    // Obtém dimensões da tela
    int screen_w = al_get_display_width(al_get_current_display());
    int screen_h = al_get_display_height(al_get_current_display());

    // Desenhar a imagem de fundo proporcional e centralizada
    if (bg_image) {
        draw_scaled_background(bg_image, 0);
    } else {
        // Fallback: fundo preto
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }

    // Cores para os textos
    ALLEGRO_COLOR color_unselected = al_map_rgb(255, 255, 255); // Branco
    ALLEGRO_COLOR color_selected = al_map_rgb(255, 215, 0);     // Dourado

    // Desenhar o game over
    al_draw_text(font,
                 color_unselected,
                 screen_w / 2,
                 screen_h / 4,
                 ALLEGRO_ALIGN_CENTER,
                 "GAME OVER");

    // Opção "Iniciar Jogo"
    al_draw_text(font,
                 (selected_opt == 1) ? color_selected : color_unselected,
                 screen_w / 2,
                 screen_h / 2,
                 ALLEGRO_ALIGN_CENTER,
                 "Voltar ao Menu");

    // Opção "Sair do Jogo"
    al_draw_text(font,
                 (selected_opt == 2) ? color_selected : color_unselected,
                 screen_w / 2,
                 screen_h / 2 + 50,
                 ALLEGRO_ALIGN_CENTER,
                 "Sair do Jogo");

    // Texto de instrução (fonte menor)
    al_draw_text(font_instr,
                 color_unselected,
                 screen_w / 2,
                 screen_h - 100,
                 ALLEGRO_ALIGN_CENTER,
                 "Use as setas para navegar e ENTER para confirmar");
}

void draw_victory(ALLEGRO_BITMAP* bg_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font_instr, int selected_opt) {
    // Obtém dimensões da tela
    int screen_w = al_get_display_width(al_get_current_display());
    int screen_h = al_get_display_height(al_get_current_display());

    // Desenhar a imagem de fundo proporcional e centralizada
    if (bg_image) {
        draw_scaled_background(bg_image, 0);
    } else {
        // Fallback: fundo preto
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }

    // Cores para os textos
    ALLEGRO_COLOR color_unselected = al_map_rgb(255, 255, 255); // Branco
    ALLEGRO_COLOR color_selected = al_map_rgb(255, 215, 0);     // Dourado

    // Desenhar o game over
    al_draw_text(font,
                 color_unselected,
                 screen_w / 2,
                 screen_h / 4,
                 ALLEGRO_ALIGN_CENTER,
                 "VICTORY!");

    // Voltar ao Menu
    al_draw_text(font,
                 (selected_opt == 1) ? color_selected : color_unselected,
                 screen_w / 2,
                 screen_h / 2,
                 ALLEGRO_ALIGN_CENTER,
                 "Voltar ao Menu");

    // Opção "Sair do Jogo"
    al_draw_text(font,
                 (selected_opt == 2) ? color_selected : color_unselected,
                 screen_w / 2,
                 screen_h / 2 + 50,
                 ALLEGRO_ALIGN_CENTER,
                 "Sair do Jogo");

    // Texto de instrução (fonte menor)
    al_draw_text(font_instr,
                 color_unselected,
                 screen_w / 2,
                 screen_h - 100,
                 ALLEGRO_ALIGN_CENTER,
                 "Use as setas para navegar e ENTER para confirmar");
}

void reset_game_state(Player** player_ptr, joystick** controle_ptr, float* camera_x_ptr, Enemy* inimigos[], bullet** shots_list_ptr, 
                      Boss** boss_ptr, GamePhase* phase, int* spawned, int* defeated) 
{
    //  Limpa o jogador
    if (player_ptr && *player_ptr) {
        player_destroy(*player_ptr);
        *player_ptr = NULL;
    }

    //  Limpa o controle
    if (controle_ptr && *controle_ptr) {
        joystick_destroy(*controle_ptr);
        *controle_ptr = NULL;
    }

    // Limpa o chefe, se ele existir >>>
    if (boss_ptr && *boss_ptr) {
        boss_destroy(*boss_ptr);
        *boss_ptr = NULL;
    }

    // Limpa todos os inimigos do array
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        if (inimigos[i] != NULL) {
            enemy_destroy(inimigos[i]);
            inimigos[i] = NULL;
        }
    }

    // Limpa todos os projéteis restantes da lista
    bullet* current = *shots_list_ptr;
    while (current != NULL) {
        bullet* to_remove = current;
        current = (bullet*)current->next;
        bullet_destroy(to_remove);
    }

    *shots_list_ptr = NULL;

    // 6. Reseta a câmera
    if (camera_x_ptr) {
        *camera_x_ptr = 0.0f;
    }

    // Reseta as variáveis de fluxo de jogo
    *phase = PHASE_ENEMIES;
    *spawned = 0;
    *defeated = 0;
}


void draw_boss_life_bar(Boss* boss, ALLEGRO_DISPLAY* disp) {
    if (!boss || !disp) return;

    // Configurações da Barra de Vida do Chefe ---
    const float BARRA_LARGURA = 400.0f; // Uma barra maior para o chefe
    const float BARRA_ALTURA = 30.0f;
    const float BARRA_Y = 20.0f; // Mesma altura da barra do jogador
    
    // Coloca a barra no canto direito da tela
    float screen_w = al_get_display_width(disp);
    const float BARRA_X = screen_w - BARRA_LARGURA - 20.0f; 

    // Calcula a porcentagem de vida
    float porcentagem_vida = boss->health / boss->max_health;
    if (porcentagem_vida < 0) porcentagem_vida = 0;
    
    float largura_vida_atual = BARRA_LARGURA * porcentagem_vida;

    // Desenha o fundo da barra (vermelho)
    al_draw_filled_rectangle(BARRA_X, BARRA_Y, BARRA_X + BARRA_LARGURA, BARRA_Y + BARRA_ALTURA, al_map_rgb(150, 0, 0));

    // Desenha a vida atual (roxa) por cima
    if (largura_vida_atual > 0) {
        al_draw_filled_rectangle(BARRA_X, BARRA_Y, BARRA_X + largura_vida_atual, BARRA_Y + BARRA_ALTURA, al_map_rgb(128, 0, 128)); // Cor Roxa
    }

    // Desenha uma borda
    al_draw_rectangle(BARRA_X, BARRA_Y, BARRA_X + BARRA_LARGURA, BARRA_Y + BARRA_ALTURA, al_map_rgb(255, 255, 255), 2.0f);
}

void draw_stamina_bar(Player* player) {
    if (!player) return;

    // Posição e tamanho (logo abaixo da barra de vida)
    const float BARRA_X = 20;
    const float BARRA_Y = 55;          // Um pouco mais para baixo
    const float BARRA_LARGURA = 200;
    const float BARRA_ALTURA = 15;   // Uma barra mais fina

    float porcentagem_estamina = player->stamina / player->max_stamina;
    if (porcentagem_estamina < 0) porcentagem_estamina = 0;
    
    float largura_estamina_atual = BARRA_LARGURA * porcentagem_estamina;

    // Fundo da barra (cinza escuro)
    al_draw_filled_rectangle(BARRA_X, BARRA_Y, BARRA_X + BARRA_LARGURA, BARRA_Y + BARRA_ALTURA, al_map_rgb(50, 50, 50));

    // Estamina atual (amarelo)
    if (largura_estamina_atual > 0) {
        al_draw_filled_rectangle(BARRA_X, BARRA_Y, BARRA_X + largura_estamina_atual, BARRA_Y + BARRA_ALTURA, al_map_rgb(255, 215, 0));
    }

    // Borda
    al_draw_rectangle(BARRA_X, BARRA_Y, BARRA_X + BARRA_LARGURA, BARRA_Y + BARRA_ALTURA, al_map_rgb(0, 0, 0), 2.0f);
}


bool check_collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    // Condição 1: Se a borda direita do Retângulo 1 está à esquerda da borda esquerda do Retângulo 2
    if ((x1 + w1) < x2) {
        return false;
    }

    // Condição 2: Se a borda esquerda do Retângulo 1 está à direita da borda direita do Retângulo 2
    if (x1 > (x2 + w2)) {
        return false;
    }

    // Condição 3: Se a borda inferior do Retângulo 1 está acima da borda superior do Retângulo 2
    if ((y1 + h1) < y2) {
        return false;
    }

    // Condição 4: Se a borda superior do Retângulo 1 está abaixo da borda inferior do Retângulo 2
    if (y1 > (y2 + h2)) {
        return false;
    }

    // Se nenhuma das condições de "não colisão" for verdadeira, então eles estão colidindo.
    return true;
}