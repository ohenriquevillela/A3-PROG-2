#ifndef __PROJECTILE__
#define __PROJECTILE__

#define BULLET_SPEED 700.0f
#define BOSS_BULLET_SIZE 60.0f 

//  Define os tipos de "donos" possíveis para um projétil
typedef enum {
    OWNER_PLAYER,
    OWNER_ENEMY,
    BOSS
} BulletOwner;

typedef struct bullet {
    float x;
    float y;
    float vel_x, vel_y;
    BulletOwner owner;    
    struct bullet *next;
} bullet;


bullet* bullet_create(float x, float y, float vx, float vy, BulletOwner owner, bullet* next);
void bullet_move(bullet* b, float dt);
void bullet_destroy(bullet *element);
void add_shot_to_list(bullet** list_head, bullet* new_shot);

#endif