/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "game.h"
static unsigned rnd(Game *g) {
    g->seed ^= g->seed << 13;
    g->seed ^= g->seed >> 17;
    g->seed ^= g->seed << 5;
    return g->seed;
}
static void exchange(Game *g,int a,int b) {
    uint8_t t=g->cell[a/COLS][a%COLS];
    g->cell[a/COLS][a%COLS]=g->cell[b/COLS][b%COLS];
    g->cell[b/COLS][b%COLS]=t;
}
static int marks(const Game *g,uint8_t mark[ROWS][COLS]) {
    int count=0;
    for(int y=0;y<ROWS;y++) for(int x=0;x<COLS;x++) mark[y][x]=0;
    for(int y=0;y<ROWS;y++) for(int x=0;x<COLS;) {
        int end=x+1;
        while(end<COLS && g->cell[y][end]==g->cell[y][x]) end++;
        if(end-x>=3) for(int k=x;k<end;k++) mark[y][k]=1;
        x=end;
    }
    for(int x=0;x<COLS;x++) for(int y=0;y<ROWS;) {
        int end=y+1;
        while(end<ROWS && g->cell[end][x]==g->cell[y][x]) end++;
        if(end-y>=3) for(int k=y;k<end;k++) mark[k][x]=1;
        y=end;
    }
    for(int y=0;y<ROWS;y++) for(int x=0;x<COLS;x++) count+=mark[y][x];
    return count;
}
int game_has_matches(const Game *g) {
    uint8_t mark[ROWS][COLS]; return marks(g,mark)>0;
}
int game_has_move(const Game *g) {
    Game t=*g;
    for(int y=0;y<ROWS;y++) for(int x=0;x<COLS;x++) {
        int a=y*COLS+x;
        if(x+1<COLS) {exchange(&t,a,a+1);
            if(game_has_matches(&t)) return 1;
            exchange(&t,a,a+1);}
        if(y+1<ROWS) {exchange(&t,a,a+COLS);
            if(game_has_matches(&t)) return 1;
            exchange(&t,a,a+COLS);}
    }
    return 0;
}
void game_shuffle(Game *g) {
    /* Build with no initial matches; regenerate until at least one legal swap.
       Cap attempts to avoid an unbounded startup loop on a broken RNG. */
    for(int attempt=0;attempt<10000;attempt++) {
        for(int y=0;y<ROWS;y++) for(int x=0;x<COLS;x++) {
            unsigned v=rnd(g)%TYPES;
            for(int tries=0;tries<TYPES;tries++,v=(v+1)%TYPES) {
                if(x>=2 && g->cell[y][x-1]==v && g->cell[y][x-2]==v) continue;
                if(y>=2 && g->cell[y-1][x]==v && g->cell[y-2][x]==v) continue;
                break;
            }
            g->cell[y][x]=(uint8_t)v;
        }
        if(game_has_move(g)) return;
    }
    /* The fallback board is always legal: swapping the centre pair
       produces a horizontal triple of diamonds. */
    for(int y=0;y<ROWS;y++) for(int x=0;x<COLS;x++)
        g->cell[y][x]=(uint8_t)((x+2*y)%TYPES);
    g->cell[2][1]=1;g->cell[2][2]=2;g->cell[2][3]=1;g->cell[3][2]=1;
}
void game_init(Game *g,uint32_t seed,uint32_t best) {
    *g=(Game){0};g->seed=seed?seed:0x1234abcd;
    g->best=best;g->selected=-1;g->cursor=(ROWS/2)*COLS+COLS/2;game_shuffle(g);
}
int game_swap(Game *g,int a,int b) {
    if(g->paused || a<0 || b<0 || a>=CELLS || b>=CELLS ||
       (a/COLS==b/COLS ? (a-b!=1 && b-a!=1) :
        (a%COLS!=b%COLS || (a-b!=COLS && b-a!=COLS)))) return 0;
    exchange(g,a,b);
    if(!game_has_matches(g)) {
        exchange(g,a,b);g->sound=1;return 0;
    }
    ++g->swaps;
    int chain=0;
    uint8_t mark[ROWS][COLS];
    while(chain<256) {
        int n=marks(g,mark);
        if(!n) break;
        chain++;
        g->score+=(uint32_t)n*10u*(unsigned)(chain<8?chain:8);
        for(int x=0;x<COLS;x++) {
            int write=ROWS-1;
            for(int y=ROWS-1;y>=0;y--)
                if(!mark[y][x]) g->cell[write--][x]=g->cell[y][x];
            while(write>=0) g->cell[write--][x]=(uint8_t)(rnd(g)%TYPES);
        }
    }
    if(g->score>g->best)g->best=g->score;
    if(!game_has_move(g) || game_has_matches(g)) game_shuffle(g);
    g->selected=-1;g->cursor=b;g->sound=chain>1?3:2;
    return 1;
}
