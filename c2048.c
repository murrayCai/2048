#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "c2048.h"

int c2048_init(
        c2048_t **c,
        uint w,uint h,
        cmalloc_f m,
        cfree_f f
        )
{
    if(NULL == c) return ERR_c2048_INIT_c2048_NULL;
    if(w <= c2048_MIN_WIDTH) return ERR_c2048_INIT_WIDTH_VALUE;
    if(h <= c2048_MIN_HEIGHT) return ERR_c2048_INIT_HEIGHT_VALUE;
    if(NULL == m) m = malloc;
    if(NULL == f) f = free;

    uint *data = (uint *)m(sizeof(uint)*w*h);
    if(NULL == data) return ERR_c2048_INIT_MEM_MALLOC_NULL;
    memset(data,0,sizeof(uint)*w*h);

    uint *d_zero_index = (uint *)m(sizeof(uint)*w*h);
    if(NULL == d_zero_index) return ERR_c2048_INIT_MEM_MALLOC_NULL;
    memset(d_zero_index,0,sizeof(uint)*w*h);

    *c = (c2048_t *)m(sizeof(c2048_t));
    if(NULL == *c) return ERR_c2048_INIT_MEM_MALLOC_NULL;
    memset(*c,0,sizeof(c2048_t));

    (*c)->d = data;
    (*c)->w = w;
    (*c)->h = h;
    (*c)->malloc = m;
    (*c)->free = f;
    (*c)->size = w*h;
    (*c)->d_zero_index = d_zero_index;

    return 0;
}

int c2048_finish(c2048_t *c){
    if(NULL == c) return ERR_c2048_FREE_NULL;
    if(NULL == c->d) return ERR_c2048_FREE_NULL;
    if(NULL == c->d_zero_index) return ERR_c2048_FREE_NULL;
    c->free(c->d);
    c->free(c->d_zero_index);
    c->free(c);
    return 0;
}

#define c2048_VALID(_c) \
    do{\
        if(NULL == (_c) ||\
                NULL == (_c)->d ||\
                0 >= (_c)->size ||\
                c2048_MIN_WIDTH > (_c)->w ||\
                c2048_MIN_HEIGHT > (_c)->h ||\
                NULL == (_c)->malloc ||\
                NULL == (_c)->free)\
        return ERR_c2048_BAD_c2048_T;\
        if(0 != (_c)->isOver) return ERR_c2048_GAME_OVER;\
    }while(0)

static int c2048_is_over(c2048_t *c){
    int x = 0, y = 0;
    uint r = 0, u = 0,v = 0;
    for(y=0;y < c->h;y++){
        for(x=0;x < c->w;x++){
            v = c2048_GET(c,x,y);
            if(0 == v) return 0;
            
            if(x < c->w - 1){
                r =  c2048_GET(c,x+1,y);
                if(0 == r || r == v) return 0;
            }

            if(y < c->h - 1){
                u = c2048_GET(c,x,y+1);
                if(0 == u || u == v) return 0;
            }
        }
    }

    return 1;
}

static int c2048_rand(c2048_t *c){
    uint index = 0,val = 2;
    uint size_zero = 0,i = 0,zero_index = 0;

    memset(c->d_zero_index,0,sizeof(uint)*c->size);
    for(;i < c->size; i++){
        if(0 == (c->d)[i]){
            (c->d_zero_index)[size_zero++] = i;
        }
    }

    if(0 == size_zero){
        c->isOver = 1;
        printf("game over 0\n");
        return ERR_c2048_GAME_OVER;
    }


    srandom(time(NULL));
    zero_index = random() % size_zero;
    index = (c->d_zero_index)[zero_index];
    (c->d)[index] = val;
    c->randVal = val;
    c->score += val;
    c->randx = index % c->w;
    c->randy = index / c->w;
    
    if(1 == size_zero){
        if(c2048_is_over(c)){
            c->isOver = 1;
            return ERR_c2048_GAME_OVER;
        }
    }
    return 0;
}

int c2048_start(c2048_t *c){
    c2048_VALID(c);
    return c2048_rand(c);
}

static int c2048_move(uint *d,uint size){
    int i = 0,v0 = 0,p0 = 0;
    int pz = -1;
    int hasMoved = 0;
    // merge
    for(;i < size;i++){
        if(0 == d[i]) continue;
        if(0 == v0){
            v0 = d[i];
            p0 = i;
        }else{
            if(v0 == d[i]){
                d[p0] *= 2;
                d[i] = 0;
                v0 = 0;
                p0 = 0;
                hasMoved = 1;
            }else{
                v0 = d[i];
                p0 = i;
            }
        }
    }

    // move
    for(i=0;i<size;i++){
        if(0 == d[i]){
            if(-1 == pz) pz = i;
        }else{
            if(-1 == pz) continue;
            d[pz++] = d[i];
            d[i] = 0;
            hasMoved = 1;
        }
    }

    return hasMoved ? 0 : 1;
}

static int c2048_move_x(c2048_t *c,int isRight){
    uint *d = (uint *)c->malloc(sizeof(uint) * c->w);
    memset(d,0,sizeof(uint) * c->w);
    int hasMoved = 0;
    int i = 0,y = 0,posd = 0; 
    for(;y < c->h;y++){
        for(i=0;i < c->w;i++){
            posd = 0 == isRight ? (y * c->w + i) : (y * (c->w) + c->w - i - 1);
            d[i] = c->d[posd];
        }
        if(0 == c2048_move(d,c->w))
            hasMoved = 1;
        for(i=0;i < c->w;i++){
            posd = 0 == isRight ? (y * c->w + i) : (y * (c->w) + c->w - i - 1);
            c->d[posd] = d[i];
        }
    }

    c->free(d);
    return hasMoved ? 0 : 1;
}

static int c2048_move_y(c2048_t *c,int isUp){
    uint *d = (uint *)c->malloc(sizeof(uint) * c->h);
    memset(d,0,sizeof(uint) * c->h);
    int hasMoved = 0;
    int x = 0,i = 0,posd = 0; 
    for(x=0;x < c->w;x++){
        for(i=0;i < c->h;i++){
            posd = 0 == isUp ? (i * c->w + x) : ((c->h - i - 1) * (c->w) + x);
            d[i] = c->d[posd];
        }
        if(0 == c2048_move(d,c->h))
            hasMoved = 1;
        for(i=0;i < c->h;i++){
            posd = 0 == isUp ? (i * c->w + x) : ((c->h - i - 1) * (c->w) + x);
            c->d[posd] = d[i];
        }
    }
    c->free(d);
    return hasMoved ? 0 : 1;
}

const char *c2048_direct_str(direct_e d){
    if(d == DIRECT_NONE) return "NONE";
    else if(DIRECT_UP == d) return "UP";
    else if(DIRECT_DOWN == d) return "DOWN";
    else if(DIRECT_LEFT == d) return "LEFT";
    else if(DIRECT_RIGHT == d) return "RIGHT";
    else return "UNKNOWN";
}

int c2048_left(c2048_t *c){
    c2048_VALID(c);
    if(0 != c2048_move_x(c,0)){
        return ERR_c2048_DISABLED_MOVE;
    }
    c->direct = DIRECT_LEFT;
    return c2048_rand(c);
}

int c2048_right(c2048_t *c){
    c2048_VALID(c);
    if( 0 != c2048_move_x(c,1)) 
        return ERR_c2048_DISABLED_MOVE;
    c->direct = DIRECT_RIGHT;
    return c2048_rand(c);
}

int c2048_up(c2048_t *c){
    c2048_VALID(c);
    if( 0 != c2048_move_y(c,1))
        return ERR_c2048_DISABLED_MOVE;
    c->direct = DIRECT_UP;
    return c2048_rand(c);
}

int c2048_down(c2048_t *c){
    c2048_VALID(c);
    if(0 != c2048_move_y(c,0))
        return ERR_c2048_DISABLED_MOVE;
    c->direct = DIRECT_DOWN;
    return c2048_rand(c);
}


