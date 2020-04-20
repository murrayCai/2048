#ifndef _2048_CORE_H_
#define _2048_CORE_H_

#include <stdio.h>

#define c2048_MIN_WIDTH 2
#define c2048_MIN_HEIGHT 2
#define uint unsigned int

typedef enum {
    ERR_c2048_BEGIN = 0,
    ERR_c2048_FETAL = 1,
    ERR_c2048_BAD_c2048_T = 2,
    ERR_c2048_INIT_c2048_NULL = 3,
    ERR_c2048_INIT_HAS_INITED = 4,
    ERR_c2048_INIT_WIDTH_VALUE = 5,
    ERR_c2048_INIT_HEIGHT_VALUE = 6,
    ERR_c2048_INIT_MEM_MALLOC_NULL = 7,
    ERR_c2048_GAME_OVER = 8,
    ERR_c2048_FREE_NULL = 9,
    ERR_c2048_DISABLED_MOVE = 10,
    ERR_c2048_END
}err_c2048_e;

typedef enum {
    DIRECT_NONE,
    DIRECT_UP,
    DIRECT_DOWN,
    DIRECT_LEFT,
    DIRECT_RIGHT
}direct_e;

const char *c2048_direct_str(direct_e d);

typedef void *(*cmalloc_f)(size_t size);
typedef void (*cfree_f)(void *ptr);

typedef struct c2048_s c2048_t;
struct c2048_s{
    uint *d;
    uint *d_zero_index;
    uint size;
    uint isOver;
    uint w;
    uint h;
    uint score;
    direct_e direct;
    uint randVal;
    uint randx;
    uint randy;
    cfree_f free;
    cmalloc_f malloc;
};

int c2048_init(
        c2048_t **c,
        uint w,uint h,
        cmalloc_f m,
        cfree_f f
        );
int c2048_finish(c2048_t *c);
int c2048_start(c2048_t *c);
int c2048_left(c2048_t *c);
int c2048_right(c2048_t *c);
int c2048_up(c2048_t *c);
int c2048_down(c2048_t *c);

#define c2048_GET(_c,_x,_y) (_c)->d[(_y) * (_c)->w + (_x)]
// #define c2048_SET(_c,_x,_y,_v) (_c)->d[(_y) * (_c)->w + (_x)] = (_v)

#endif
