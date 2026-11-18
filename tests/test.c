/* SPDX-License-Identifier: GPL-3.0-or-later */
#include <assert.h>
#include <stdio.h>
#include "app.h"
int main(void){
    App a;app_init(&a,1234,0);
    assert(!game_has_matches(&a.game));
    assert(game_has_move(&a.game));
    for(unsigned i=1;i<200;i++){
        Game g;game_init(&g,i,0);
        assert(!game_has_matches(&g));
        assert(game_has_move(&g));
    }
    assert(app_input(&a,"w -18"));
    assert(a.game.cursor==15);
    assert(app_input(&a,"k u 218"));
    assert(a.game.selected==15);
    assert(app_input(&a,"k u 170"));
    assert(a.game.paused);
    assert(app_input(&a,"k u 218"));
    assert(!a.game.paused);
    unsigned ok=0;
    for(int y=0;y<BOARD;y++)for(int x=0;x<BOARD;x++){
        int cell=y*BOARD+x;
        if(x<BOARD-1){Game g=a.game;if(game_swap(&g,cell,cell+1)){ok=1;assert(!game_has_matches(&g));assert(game_has_move(&g));}}
        if(y<BOARD-1){Game g=a.game;if(game_swap(&g,cell,cell+BOARD)){ok=1;assert(!game_has_matches(&g));assert(game_has_move(&g));}}
    }
    assert(ok);
    puts("Diamond Crush core and controls passed");
    return 0;
}
