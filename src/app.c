/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "app.h"
enum { BACK=170,PLAY=171,PREV=172,NEXT=173,CENTER=218,LEFT=222,RIGHT=223 };
void app_init(App *a,unsigned seed,unsigned best) {
    *a=(App){0};a->accent=0x9e80ff;game_init(&a->game,seed,best);a->drag=-1;
}
static int value(const char **p) {
    while(**p==' ')++*p;
    int sign=1;
    if(**p=='-')sign=-1,++*p;
    if(**p<'0'||**p>'9')return 1000000;
    int n=0;while(**p>='0'&&**p<='9'&&n<100000)n=n*10+*(*p)++-'0';
    return n*sign;
}
static int swap_start(App *a,int from,int to) {
    int dx=from%COLS-to%COLS,dy=from/COLS-to/COLS;
    if(from<0||to<0||from>=CELLS||to>=CELLS||dx*dx+dy*dy!=1)return 0;
    a->swap_a=from;a->swap_b=to;a->animation=1;a->animation_ms=0;
    a->game.selected=-1;return 1;
}
int app_tick(App *a,unsigned elapsed) {
    if(!a->animation||a->game.paused)return 0;
    a->animation_ms+=elapsed;
    unsigned duration=a->animation==1?180:140;
    if(a->animation_ms>=duration) {
        a->animation_ms=0;
        if(a->animation==1 && !game_swap(&a->game,a->swap_a,a->swap_b))
            a->animation=2;
        else a->animation=0;
    }
    return 1;
}
static int grid_pos(int x,int y) {
    if(x<GRID_X||x>=GRID_X+COLS*PITCH||y<GRID_Y||y>=GRID_Y+ROWS*PITCH)return -1;
    return (y-GRID_Y)/PITCH*COLS+(x-GRID_X)/PITCH;
}
static int choose(App *a,int pos) {
    Game *g=&a->game;
    if(g->paused)return 0;
    if(g->selected<0) {g->selected=pos;g->cursor=pos;return 1;}
    if(g->selected==pos) {g->selected=-1;return 1;}
    int old=g->selected;
    if(swap_start(a,old,pos))return 1;
    g->selected=pos;g->cursor=pos;return 1;
}
int app_input(App *a,const char *message) {
    if(!message || !message[0] || message[1]!=' ')return 0;
    Game *g=&a->game;
    if(message[0]=='w') {
        if(a->animation||g->paused)return 0;
        const char *p=message+2;int delta=value(&p);
        if(delta>1000||delta< -1000)return 0;
        a->wheel_rem-=delta;
        int steps=a->wheel_rem/18;
        a->wheel_rem%=18;
        if(steps>12)steps=12;
        if(steps< -12)steps=-12;
        /* Full ring navigation, then wrap; every one of 35 tiles is reachable. */
        if(steps)g->cursor=(g->cursor+steps%CELLS+CELLS)%CELLS;
        return steps!=0;
    }
    if(!message[2] || message[3]!=' ')return 0;
    const char *p=message+4;int x=value(&p);
    if(message[0]=='k' && message[2]=='u') {
        if(x==BACK) {
            a->animation=0;a->drag=-1;
            if(g->paused)g->quit=1;
            else g->paused=1;
            g->selected=-1;return 1;
        }
        if(a->animation)return 0;
        if(g->paused) {
            if(x==CENTER || x==PLAY) {g->paused=0;return 1;}
            return 0;
        }
        if(x==CENTER)return choose(a,g->cursor);
        if(x==LEFT||x==PREV)g->cursor=(g->cursor+CELLS-1)%CELLS;
        else if(x==RIGHT||x==NEXT)g->cursor=(g->cursor+1)%CELLS;
        else if(x==PLAY)g->selected=-1;
        else return 0;
        return 1;
    }
    if(message[0]=='t' && (message[2]=='d'||message[2]=='u')) {
        if(a->animation)return 0;
        int y=value(&p);
        if(x<0||x>=SCREEN_W||y<0||y>=SCREEN_H){a->drag=-1;return 0;}
        if(message[2]=='d') {
            a->drag=grid_pos(x,y);a->drag_x=x;a->drag_y=y;return 0;
        }
        int from=a->drag;a->drag=-1;
        if(a->animation)return 0;
        if(g->paused) {g->paused=0;return 1;}
        if(y<40) {g->paused=1;g->selected=-1;return 1;}
        int dx=x-a->drag_x,dy=y-a->drag_y;
        int ax=dx<0?-dx:dx,ay=dy<0?-dy:dy;
        if(from>=0 && (ax>=12||ay>=12)) {
            int col=from%COLS,row=from/COLS;
            if(ax>ay)col+=dx>0?1:-1;else row+=dy>0?1:-1;
            if(col<0||col>=COLS||row<0||row>=ROWS)return 0;
            return swap_start(a,from,row*COLS+col);
        }
        int pos=grid_pos(x,y);
        if(pos>=0)return choose(a,pos);
    }
    return 0;
}
