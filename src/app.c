/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "app.h"
enum { BACK=170,PLAY=171,PREV=172,NEXT=173,CENTER=218,LEFT=222,RIGHT=223 };
void app_init(App *a,unsigned seed,unsigned best) {
    *a=(App){0};a->accent=0x9e80ff;game_init(&a->game,seed,best);
}
static int value(const char **p) {
    while(**p==' ')++*p;
    int sign=1;
    if(**p=='-')sign=-1,++*p;
    if(**p<'0'||**p>'9')return 1000000;
    int n=0;while(**p>='0'&&**p<='9'&&n<100000)n=n*10+*(*p)++-'0';
    return n*sign;
}
static int choose(App *a,int pos) {
    Game *g=&a->game;
    if(g->paused)return 0;
    if(g->selected<0) {g->selected=pos;g->cursor=pos;return 1;}
    if(g->selected==pos) {g->selected=-1;return 1;}
    int old=g->selected;
    if(game_swap(g,old,pos))return 1;
    g->selected=pos;g->cursor=pos;return 1;
}
int app_input(App *a,const char *message) {
    if(!message || !message[0] || message[1]!=' ')return 0;
    Game *g=&a->game;
    if(message[0]=='w') {
        const char *p=message+2;int delta=value(&p);
        if(delta>1000||delta< -1000)return 0;
        a->wheel_rem-=delta;
        int steps=a->wheel_rem/18;
        a->wheel_rem%=18;
        if(steps>12)steps=12;if(steps< -12)steps=-12;
        /* Full ring navigation, then wrap; every one of 36 tiles is reachable. */
        if(steps)g->cursor=(g->cursor+steps%36+36)%36;
        return steps!=0;
    }
    if(!message[2] || message[3]!=' ')return 0;
    const char *p=message+4;int x=value(&p);
    if(message[0]=='k' && message[2]=='u') {
        if(x==BACK) {
            if(g->paused)g->quit=1;
            else g->paused=1;
            g->selected=-1;return 1;
        }
        if(g->paused) {
            if(x==CENTER || x==PLAY) {g->paused=0;return 1;}
            return 0;
        }
        if(x==CENTER)return choose(a,g->cursor);
        if(x==LEFT||x==PREV)g->cursor=(g->cursor+35)%36;
        else if(x==RIGHT||x==NEXT)g->cursor=(g->cursor+1)%36;
        else if(x==PLAY)g->selected=-1;
        else return 0;
        return 1;
    }
    if(message[0]=='t' && message[2]=='u') {
        int y=value(&p);
        if(x<0||x>=SCREEN_W||y<0||y>=SCREEN_H)return 0;
        if(g->paused) {g->paused=0;return 1;}
        if(y<40) {g->paused=1;g->selected=-1;return 1;}
        if(x>=GRID_X && x<GRID_X+6*PITCH &&
           y>=GRID_Y && y<GRID_Y+6*PITCH) {
            int cx=(x-GRID_X)/PITCH,cy=(y-GRID_Y)/PITCH;
            return choose(a,cy*BOARD+cx);
        }
    }
    return 0;
}
