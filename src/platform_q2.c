/* SPDX-License-Identifier: GPL-3.0-or-later
 * Q2 native frame and datagram input contract, adapted from q2-arcade.
 * Firmware-owned audio is enabled by q2app, with samples in assets/.
 */
#include "app.h"
long sc(long n,long a,long b,long c,long d,long e,long f);
__asm__(".text\n.set noreorder\n.globl sc\nsc:\n"
"move $2, $4\nmove $4, $5\nmove $5, $6\nmove $6, $7\n"
"lw $7, 16($29)\nlw $8, 20($29)\nlw $9, 24($29)\n"
"addiu $29, $29, -32\nsw $8, 16($29)\nsw $9, 20($29)\n"
"syscall\naddiu $29, $29, 32\nbeqz $7, 1f\nnop\n"
"subu $2, $0, $2\n1: jr $31\nnop\n"
".globl __start\n__start:\nlw $4, 0($29)\naddiu $5, $29, 4\n"
"li $8, -8\nand $29, $29, $8\naddiu $29, $29, -32\n"
"jal q2_main\nnop\nmove $4, $2\nli $2, 4246\nsyscall\n"
"1: b 1b\nnop\n.set reorder\n");
enum { SYS_READ=4003,SYS_OPEN=4005,SYS_CLOSE=4006,SYS_UNLINK=4010,
 SYS_IOCTL=4054,SYS_MMAP=4090,SYS_MUNMAP=4091,SYS_BIND=4169,
 SYS_RECV=4175,SYS_SOCKET=4183,SYS_POLL=4188,SYS_CLOCK_GETTIME=4263,
 FB_VSCREEN=0x4600,FB_FSCREEN=0x4602,FB_PAN=0x4606,
 O_RDWR=2,O_CLOEXEC=0x80000,MSG_DONTWAIT=0x40 };
#define SOCK "/tmp/q2-diamond-crush.input.sock"
static App a;
void *memset(void *p,int c,__SIZE_TYPE__ n){unsigned char*q=p;while(n--)*q++=(unsigned char)c;return p;}
void *memcpy(void *p,const void*q,__SIZE_TYPE__ n){unsigned char*d=p;const unsigned char*s=q;while(n--)*d++=*s++;return p;}
static unsigned now_ms(void){long t[2]={0};sc(SYS_CLOCK_GETTIME,1,(long)t,0,0,0,0);return (unsigned)t[0]*1000u+(unsigned)t[1]/1000000u;}
static int input_open(void){
    struct {unsigned short family;char path[108];} addr={1,SOCK};
    int fd=(int)sc(SYS_SOCKET,1,1,0,0,0,0);if(fd<0)return -1;
    sc(SYS_UNLINK,(long)SOCK,0,0,0,0,0);
    if(!sc(SYS_BIND,fd,(long)&addr,sizeof addr,0,0,0))return fd;
    sc(SYS_CLOSE,fd,0,0,0,0,0);return -1;
}
static int read_light(const uint32_t*d,unsigned stride){
    unsigned count=0,bright=0;
    for(int x=45;x<330;x+=45)for(int y=60;y<270;y+=35){
        uint32_t c=d[x*stride+SCREEN_H-1-y];
        unsigned l=((c>>16)&255)*299+((c>>8)&255)*587+(c&255)*114;
        bright+=l>128000;count++;
    }
    return bright>count/2;
}
static int config_light(int fallback) {
    char data[4096];
    int fd=(int)sc(SYS_OPEN,(long)"/mnt/data/config.ini",O_CLOEXEC,0,0,0,0);
    if(fd<0)return fallback;
    long n=sc(SYS_READ,fd,(long)data,sizeof data-1,0,0,0);
    sc(SYS_CLOSE,fd,0,0,0,0,0);
    if(n<=0)return fallback;
    data[n]=0;
    int ipod=0;
    for(char *p=data;*p;) {
        char *end=p;while(*end&&*end!='\n')end++;
        char *q=p;while(q<end&&(*q==' '||*q=='\t'))q++;
        if(q<end&&*q=='[') {
            const char tag[]="[IPOD]";
            int i=0;while(i<6&&q+i<end&&q[i]==tag[i])i++;
            ipod=i==6;
        } else if(ipod && end-q>=5 && q[0]=='L'&&q[1]=='I'&&q[2]=='G'&&q[3]=='H'&&q[4]=='T') {
            q+=5;while(q<end&&(*q==' '||*q=='\t'))q++;
            if(q<end&&*q++=='=') {
                while(q<end&&(*q==' '||*q=='\t'))q++;
                if(q<end&&*q=='0')return 0;
                if(q<end&&*q=='1')return 1;
                return fallback; /* Auto: inherit the current display. */
            }
        }
        p=*end?end+1:end;
    }
    return fallback;
}
int q2_main(void){
    unsigned var[40],fix[17];
    int fb=(int)sc(SYS_OPEN,(long)"/dev/fb0",O_RDWR|O_CLOEXEC,0,0,0,0);
    if(fb<0)return 1;
    if(sc(SYS_IOCTL,fb,FB_VSCREEN,(long)var,0,0,0)||
       sc(SYS_IOCTL,fb,FB_FSCREEN,(long)fix,0,0,0))return 1;
    unsigned stride=fix[11]/4,front=var[5],rows=fix[5]/fix[11];
    if(var[0]!=SCREEN_H||var[1]!=SCREEN_W||var[4]||var[6]!=32||
       var[8]!=16||fix[11]%4||stride<SCREEN_H||rows<SCREEN_W||
       front>rows-SCREEN_W)return 1;
    long mapped=sc(SYS_MMAP,0,(long)fix[5],3,1,fb,0);
    if(mapped<0&&mapped>=-4095)return 1;
    uint32_t *pixels=(uint32_t*)mapped;int fd=input_open();
    if(fd<0)return 1;
    app_init(&a,now_ms(),0);a.light=config_light(read_light(pixels+front*stride,stride));
    unsigned visible=front;
    int can_pan=rows>=2*SCREEN_W&&(front==0||front==SCREEN_W);
    struct {int fd;short events,revents;} poller={fd,1,0};
    int redraw=1;
    while(!a.game.quit) {
        if(redraw){
            unsigned target=can_pan?(visible?0:SCREEN_W):visible;
            app_render(&a,pixels+target*stride,stride);
            if(can_pan){var[5]=target;
                if(!sc(SYS_IOCTL,fb,FB_PAN,(long)var,0,0,0))visible=target;
                else {can_pan=0;app_render(&a,pixels+visible*stride,stride);}
            }
            redraw=0;
        }
        long ready=sc(SYS_POLL,(long)&poller,1,-1,0,0,0);
        if(ready==-4)continue;
        if(ready<0||!(poller.revents&1))break;
        char msg[64];
        long n=sc(SYS_RECV,fd,(long)msg,sizeof msg-1,MSG_DONTWAIT,0,0);
        if(n>0){msg[n]=0;if(app_input(&a,msg))redraw=1;}
    }
    if(visible!=front){var[5]=front;sc(SYS_IOCTL,fb,FB_PAN,(long)var,0,0,0);}
    sc(SYS_CLOSE,fd,0,0,0,0,0);sc(SYS_UNLINK,(long)SOCK,0,0,0,0,0);
    sc(SYS_MUNMAP,mapped,(long)fix[5],0,0,0,0);sc(SYS_CLOSE,fb,0,0,0,0,0);
    return a.game.quit?0:1;
}
