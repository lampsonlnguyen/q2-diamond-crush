/* SPDX-License-Identifier: GPL-3.0-or-later */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "app.h"
static void swipe(int dx,int dy) {
    App a;app_init(&a,42,0);
    int from=2*COLS+3,to=from+dy*COLS+dx;
    Game probe=a.game;int valid=game_swap(&probe,from,to);
    uint8_t before[ROWS][COLS];memcpy(before,a.game.cell,sizeof before);
    char msg[64];int x=GRID_X+3*PITCH+18,y=GRID_Y+2*PITCH+18;
    snprintf(msg,sizeof msg,"t d %d %d",x,y);app_input(&a,msg);
    snprintf(msg,sizeof msg,"t u %d %d",x+dx*25,y+dy*25);
    assert(app_input(&a,msg));assert(a.animation==1);
    assert(a.swap_a==from&&a.swap_b==to);
    app_tick(&a,90);assert(a.animation==1);
    app_tick(&a,90);
    if(valid){assert(!a.animation);assert(a.game.swaps==1);}
    else {assert(a.animation==2);app_tick(&a,140);assert(!a.animation);
          assert(!memcmp(before,a.game.cell,sizeof before));}
    assert(!game_has_matches(&a.game));assert(game_has_move(&a.game));
}
int main(void){
    swipe(1,0);swipe(-1,0);swipe(0,1);swipe(0,-1);
    assert(COLS==8 && ROWS==5 && CELLS==40);
    assert(GRID_X>=0 && GRID_X+COLS*PITCH<=SCREEN_W);
    assert(GRID_Y+ROWS*PITCH<=SCREEN_H);
    App a;app_init(&a,1234,0);
    assert(!game_has_matches(&a.game));
    assert(game_has_move(&a.game));
    for(unsigned i=1;i<200;i++){
        Game g;game_init(&g,i,0);
        assert(!game_has_matches(&g));
        assert(game_has_move(&g));
    }
    assert(app_input(&a,"w -18"));
    assert(a.game.cursor==18);
    assert(app_input(&a,"k u 218"));
    assert(a.game.selected==18);
    assert(a.game.sound==4); /* selection sound requested */
    assert(app_input(&a,"k u 170"));
    assert(a.game.paused);
    assert(app_input(&a,"k u 218"));
    assert(!a.game.paused);
    unsigned ok=0;
    for(int y=0;y<ROWS;y++)for(int x=0;x<COLS;x++){
        int cell=y*COLS+x;
        if(x<COLS-1){Game g=a.game;if(game_swap(&g,cell,cell+1)){ok=1;assert(g.sound==2||g.sound==3);assert(!game_has_matches(&g));assert(game_has_move(&g));}}
        if(y<ROWS-1){Game g=a.game;if(game_swap(&g,cell,cell+COLS)){ok=1;assert(!game_has_matches(&g));assert(game_has_move(&g));}}
    }
    assert(ok);
    app_init(&a,5,0);
    char msg[64];
    snprintf(msg,sizeof msg,"t d %d %d",GRID_X+18,GRID_Y+18);app_input(&a,msg);
    snprintf(msg,sizeof msg,"t u %d %d",GRID_X-10,GRID_Y+18);app_input(&a,msg);
    assert(!a.animation);
    a.game.cursor=CELLS-1;app_input(&a,"w -18");assert(a.game.cursor==0);
    puts("Diamond Crush core and controls passed");
    return 0;
}
