#include <stdlib.h>
#include "Pistol.h"


pistol* pistol_create() {
    pistol *new_pistol = (pistol*) malloc(sizeof(pistol));
    if (!new_pistol) return NULL;
    new_pistol->shots = NULL;
    return new_pistol;
}

void pistol_destroy(pistol *element) {
    if (!element) return;
    
    bullet *current = element->shots;
    bullet *next;

    while (current != NULL) {
        next = (bullet*) current->next;
        bullet_destroy(current);
        current = next;
    }
    
    free(element);
}