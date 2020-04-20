#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include "c2048.h"

#define IS_DEBUG 0
#define WIDTH 4
#define HEIGHT 4

const int color_str(uint v){
    switch(v){
        case 0:     return 0;
        case 2:     return 255;
        case 4:     return 10;
        case 8:     return 115;
        case 16:    return 103;
        case 32:    return 111;
        case 64:    return 56;
        case 128:   return 185;
        case 256:   return 226;
        case 512:   return 168;
        case 1024:  return 166;
        case 2048:  return 212;
        case 4096:  return 96;
        case 8192:  return 240;
        default:  return 255;
    }
}

c2048_t *c = NULL;

void render(){
    int x = 0, y = c->h - 1;
    uint val = 0;
    printf("\n====SCORE:[\e[1;31m%u\e[0m](\e[1;33m%s\e[0m => %u[%u,%u])====\n",
            c->score,c2048_direct_str(c->direct),
            c->randVal,c->randx,c->randy);
    for(;y >= 0;y--){
        for(x = 0;x < c->w;x++){
            val = c2048_GET(c,x,y);
            if(x == c->randx && y == c->randy){
                printf("\e[38;5;%d;48;5;166m%d\e[0m\t",color_str(val),val);
            }else{
                printf("\e[38;5;%dm%d\e[0m\t",color_str(val),val);
            }
        }
        printf("\n");
    }
}

      
#define CLEAR_SCREEN() printf("\e[1;1H\e[2J")

#define MOVE(direct) \
    do{ \
        int ret = c2048_##direct(c); \
        CLEAR_SCREEN(); \
        render();\
        switch(ret){ \
            case ERR_c2048_GAME_OVER: \
                printf("\n\n\e[1;31;40mGAME OVER!\n\e[0m");\
                goto end;\
                break; \
            case ERR_c2048_DISABLED_MOVE: \
                printf("\n\n\e[1;31;40mcan not move %s.\n\e[0m",#direct);\
                break;\
            case  0: \
                break; \
            default: \
                printf("\n\n\e[1;31;40m[ERROR]\tmove %s,code[%d]!\n\e[0m",#direct,ret);\
                goto end;\
                break;\
        } \
    }while(0)

void signal_callback(int signum) {
	printf("         TERMINATED         \n");
	printf("\033[?25h\033[m");
    c2048_finish(c);
	exit(signum);
}

int main(int argc,char *argv[]){

    signal(SIGINT,signal_callback);

    int ret = 0;
    ret = c2048_init(&c,WIDTH,HEIGHT,NULL,NULL);
    if(0 != ret){
        printf("init c2048 failed:code[%d]!\n",ret);
        return 1;
    }
    
    ret = c2048_start(c);
    if(0 != ret){
        printf("start failed:code[%d]!",ret);
        return 2;
    }
    CLEAR_SCREEN();
    render();

    char ch;
    int inLoop = 0;
    srandom(time(NULL));
    while(1){
        if(IS_DEBUG){
            inLoop = 0;
            ch = random() % 4 + 1;
            usleep(1);
        }else{
            ch = getchar();
        }

        switch(ch){
            case 4:
			case 97:	// 'a' key
			case 104:	// 'h' key
			case 68:	// left arrow
                if(0 == inLoop) {
                    MOVE(left);
                    inLoop = 1;
                }
                break;
            case 1:
			case 100:	// 'd' key
			case 108:	// 'l' key
			case 67:	// right arrow
                if(0 == inLoop){
                    MOVE(right);
                    inLoop = 1;
                }
                break;
            case 2:
			case 119:	// 'w' key
			case 107:	// 'k' key
			case 65:	// up arrow
                if(0 == inLoop){
                    MOVE(up);
                    inLoop = 1;
                }
                break;
            case 3:
			case 115:	// 's' key
			case 106:	// 'j' key
			case 66:	// down arrow
                if(0 == inLoop){
                    MOVE(down);
                    inLoop = 1;
                }
                break;
            case 0:
            case 10:
                inLoop = 0;
                break;
            default:
                break;
        }
    }

end:
    c2048_finish(c);
    return 0;
}
