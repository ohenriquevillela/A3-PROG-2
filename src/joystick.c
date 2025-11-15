#include <stdlib.h>
#include "joystick.h"

// Cria e inicializa o controle
joystick* joystick_create() {
    joystick* joy = (joystick*) malloc(sizeof(joystick));
    if (joy) {
        joy->right = 0;
        joy->left = 0;
        joy->up = 0;
        joy->down = 0;
        joy->right2 = 0;
        joy->left2 = 0;
        joy->up2 = 0;
        joy->down2 = 0;
    }
    return joy;
}

// Libera memória
void joystick_destroy(joystick *element) {
    if (element) {
        free(element);
    }
}

// Funções de ativação/desativação
void joystick_right(joystick *element) {
    if (element) element->right = !element->right;
}

void joystick_left(joystick *element) {
    if (element) element->left = !element->left;
}

void joystick_up(joystick *element) {
    if (element) element->up = !element->up;
}

void joystick_down(joystick *element) {
    if (element) element->down = !element->down;
}

// Funções de ativação/desativação
void joystick_right2(joystick *element) {
    if (element) element->right2 = !element->right2;
}

void joystick_left2(joystick *element) {
    if (element) element->left2 = !element->left2;
}

void joystick_up2(joystick *element) {
    if (element) element->up2= !element->up2;
}

void joystick_down2(joystick *element) {
    if (element) element->down2 = !element->down2;
}
