#include "projectile.h"
#include <stdlib.h>
#include <stdio.h>

// Cria um projétil com velocidades
bullet* bullet_create(float x, float y, float vx, float vy, BulletOwner owner, bullet* next) { 
    bullet *new_bullet = (bullet*) malloc(sizeof(bullet));

    if (!new_bullet) return NULL;
    new_bullet->x = x;
    new_bullet->y = y;
    new_bullet->vel_x = vx;
    new_bullet->vel_y = vy;
    new_bullet->owner = owner;    
    new_bullet->next = (struct bullet*) next;
    return new_bullet;
}

// Move um ÚNICO projétil baseado na velocidade e no tempo
void bullet_move(bullet* b, float dt) {
    if (b) {
        b->x += b->vel_x * dt;
        b->y += b->vel_y * dt;
    }
}

void bullet_destroy(bullet *element) {
    free(element);
}

void add_shot_to_list(bullet** list_head, bullet* new_shot) {
    // Se o projétil a ser adicionado não existe, não faz nada.
    if (!new_shot) return;

    // 1. O campo "next" do novo projétil agora aponta para o que era o antigo início da lista.
    //    Se a lista estava vazia (*list_head era NULL), o "next" dele será NULL.
    new_shot->next = (struct bullet*) *list_head;

    // 2. O ponteiro do início da lista agora é atualizado para apontar para o novo projétil.
    //    Ele se tornou o novo primeiro item.
    *list_head = new_shot;
}
