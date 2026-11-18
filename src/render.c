/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "app.h"
static const uint32_t colors[6]={0xd7dde9,0x32dff1,0x76d98d,0xd4d8ef,0xc66bfa,0xff6478};
static void fill(uint32_t*d,unsigned s,int x,int y,int w,int h,uint32_t c){
    if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
    if(x+w>SCREEN_W)w=SCREEN_W-x;
    if(y+h>SCREEN_H)h=SCREEN_H-y;
    if(w<=0||h<=0)return;
    for(int i=x;i<x+w;i++)for(int j=y;j<y+h;j++)d[i*s+SCREEN_H-1-j]=0xff000000u|c;
}
static void line(uint32_t*d,unsigned s,int x,int y,int xx,int yy,uint32_t c){
    int dx=xx>x?xx-x:x-xx,dy=yy>y?yy-y:y-yy;
    int sx=x<xx?1:-1,sy=y<yy?1:-1,err=dx-dy;
    for(;;){fill(d,s,x,y,1,1,c);if(x==xx&&y==yy)break;
        int e=2*err;if(e> -dy){err-=dy;x+=sx;}if(e<dx){err+=dx;y+=sy;}}
}
static void border(uint32_t*d,unsigned s,int x,int y,int w,int h,uint32_t c){
    fill(d,s,x,y,w,1,c);fill(d,s,x,y+h-1,w,1,c);
    fill(d,s,x,y,1,h,c);fill(d,s,x+w-1,y,1,h,c);
}
/* 5x7 pixel font: uppercase, digits, spaces, and small punctuation. */
static const unsigned char glyph[40][7]={
{14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},
{30,17,17,17,17,17,30},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
{14,17,16,23,17,17,14},{17,17,17,31,17,17,17},{14,4,4,4,4,4,14},
{1,1,1,1,17,17,14},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
{17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},
{30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
{15,16,16,14,1,1,30},{31,4,4,4,4,4,4},{17,17,17,17,17,17,14},
{17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
{17,17,10,4,4,4,4},{31,1,2,4,8,16,31},
{14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},
{30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,30,1,1,17,14},
{6,8,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},
{14,17,17,15,1,2,12},{0,0,0,0,0,0,0},{0,4,4,4,4,0,4},
{0,0,0,31,0,0,0},{0,0,0,0,0,6,6}
};
static int index(char c){
    if(c>='a'&&c<='z')c-=32;
    if(c>='A'&&c<='Z')return c-'A';
    if(c>='0'&&c<='9')return c-'0'+26;
    if(c=='!')return 37;if(c=='-')return 38;if(c=='.')return 39;
    return 36;
}
static void text(uint32_t*d,unsigned s,int x,int y,const char*t,int k,uint32_t c){
    while(*t){
        const unsigned char *p=glyph[index(*t++)];
        for(int row=0;row<7;row++)for(int col=0;col<5;col++)
            if(p[row]&(16>>col))fill(d,s,x+col*k,y+row*k,k,k,c);
        x+=6*k;
    }
}
static void number(uint32_t*d,unsigned s,int x,int y,uint32_t n,int k,uint32_t c){
    char str[12];int len=0;do{str[len++]=(char)('0'+n%10);n/=10;}while(n&&len<10);
    for(int i=0;i<len/2;i++){char q=str[i];str[i]=str[len-1-i];str[len-1-i]=q;}
    str[len]=0;text(d,s,x,y,str,k,c);
}
static void circle(uint32_t*d,unsigned s,int cx,int cy,int r,uint32_t c){
    for(int yy=-r;yy<=r;yy++)for(int xx=-r;xx<=r;xx++)
        if(xx*xx+yy*yy<=r*r)fill(d,s,cx+xx,cy+yy,1,1,c);
}
static void piece(uint32_t*d,unsigned s,int x,int y,int type,int light){
    uint32_t c=colors[type];uint32_t shadow=light?0x252b34:0x0a1524;
    switch(type){
    case 0: /* thinking cat: ears, whiskers and paw at chin */
        fill(d,s,x+9,y+12,19,15,c);
        line(d,s,x+9,y+12,x+8,y+5,c);
        line(d,s,x+8,y+5,x+16,y+11,c);
        line(d,s,x+27,y+12,x+28,y+5,c);
        line(d,s,x+28,y+5,x+20,y+11,c);
        fill(d,s,x+13,y+17,3,2,shadow);fill(d,s,x+23,y+17,3,2,shadow);
        fill(d,s,x+18,y+22,3,2,0xea91a0);
        line(d,s,x+9,y+23,x+3,y+21,c);
        line(d,s,x+27,y+23,x+33,y+21,c);
        circle(d,s,x+17,y+28,5,c);fill(d,s,x+18,y+24,4,4,shadow);
        break;
    case 1: /* faceted diamond */
        line(d,s,x+9,y+10,x+26,y+10,c);line(d,s,x+5,y+18,x+30,y+18,c);
        line(d,s,x+9,y+10,x+5,y+18,c);line(d,s,x+26,y+10,x+30,y+18,c);
        line(d,s,x+5,y+18,x+18,y+32,c);line(d,s,x+30,y+18,x+18,y+32,c);
        line(d,s,x+13,y+10,x+12,y+18,c);line(d,s,x+23,y+10,x+24,y+18,c);
        line(d,s,x+12,y+18,x+18,y+32,c);line(d,s,x+24,y+18,x+18,y+32,c);
        fill(d,s,x+14,y+13,8,2,c);
        break;
    case 2: /* martini */
        line(d,s,x+7,y+10,x+29,y+10,c);line(d,s,x+7,y+10,x+18,y+23,c);
        line(d,s,x+29,y+10,x+18,y+23,c);
        line(d,s,x+18,y+23,x+18,y+31,c);line(d,s,x+10,y+31,x+26,y+31,c);
        line(d,s,x+24,y+12,x+31,y+5,c);circle(d,s,x+23,y+15,3,0xe55357);
        break;
    case 3: /* generic click-wheel music player */
        fill(d,s,x+10,y+5,17,28,c);
        fill(d,s,x+13,y+8,11,9,shadow);
        circle(d,s,x+18,y+25,6,0x67758b);
        circle(d,s,x+18,y+25,2,c);
        break;
    case 4: /* music note */
        fill(d,s,x+20,y+7,4,17,c);fill(d,s,x+23,y+7,8,4,c);
        line(d,s,x+23,y+7,x+29,y+13,c);
        circle(d,s,x+15,y+26,6,c);fill(d,s,x+14,y+21,6,3,c);
        break;
    case 5: /* headphones */
        for(int yy=8;yy<17;yy++) {
            int spread=yy<12?5:9;
            fill(d,s,x+18-spread,y+yy,2,1,c);
            fill(d,s,x+18+spread-1,y+yy,2,1,c);
        }
        line(d,s,x+13,y+8,x+23,y+8,c);
        fill(d,s,x+6,y+16,7,14,c);fill(d,s,x+23,y+16,7,14,c);
        fill(d,s,x+9,y+20,3,7,shadow);fill(d,s,x+23,y+20,3,7,shadow);
        break;
    }
}
void app_render(const App *a,uint32_t*d,unsigned s){
    uint32_t bg=a->light?0xf3f3f5:0x080b14;
    uint32_t panel=a->light?0xffffff:0x111827;
    uint32_t tile=a->light?0xe7e9f0:0x172237;
    uint32_t ink=a->light?0x20242c:0xf3f2fa;
    uint32_t dim=a->light?0x646978:0xa1a8bb;
    uint32_t border_c=a->light?0xadb4c2:0x354259;
    fill(d,s,0,0,SCREEN_W,SCREEN_H,bg);
    fill(d,s,36,7,303,40,panel);
    text(d,s,76,12,"DIAMOND CRUSH",2,ink);
    text(d,s,41,33,"Q2 POD",1,dim);
    text(d,s,235,33,"ENDLESS MATCH 3",1,dim);
    text(d,s,40,50,"SCORE",1,dim);
    number(d,s,40,61,a->game.score,1,ink);
    text(d,s,284,50,"BEST",1,dim);
    number(d,s,284,61,a->game.best,1,ink);
    for(int y=0;y<BOARD;y++)for(int x=0;x<BOARD;x++){
        int i=y*BOARD+x,px=GRID_X+x*PITCH,py=GRID_Y+y*PITCH+12;
        uint32_t tc=tile,frame=border_c;
        if(a->game.selected==i)tc=a->light?0xd8dcfb:0x303859;
        fill(d,s,px,py,CELL,CELL,tc);
        border(d,s,px,py,CELL,CELL,frame);
        piece(d,s,px,py,a->game.cell[y][x],a->light);
        if(a->game.cursor==i) {
            border(d,s,px-1,py-1,CELL+2,CELL+2,a->accent);
            border(d,s,px+1,py+1,CELL-2,CELL-2,a->accent);
        }
        if(a->game.selected==i) {
            fill(d,s,px+2,py+2,4,4,a->accent);
            fill(d,s,px+CELL-6,py+CELL-6,4,4,a->accent);
        }
    }
    text(d,s,54,302,"WHEEL SELECT  CENTER SWAP  BACK PAUSE",1,dim);
    if(a->game.paused){
        fill(d,s,56,115,263,97,panel);
        border(d,s,56,115,263,97,a->accent);
        text(d,s,118,135,"PAUSED",3,ink);
        text(d,s,95,180,"CENTER RESUME  BACK EXIT",1,dim);
    }
}
