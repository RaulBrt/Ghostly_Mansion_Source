#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <graphics.h>
static double conv = (3.14159265359/180);
int res[2] = {1000,1000};

//Classify stuff=======================================================

class player{
	public:
	int alcance;
    int tamanho;
    class pos{
    	public:
        int x;
        int y;
        int angle;
        int hit[2];
        int relx;
		int rely;
    };
    pos pos;
};
class enemy{
	public:
		int tamanho;
		int color[3];
		class pos{
			public:
				int x;
				int y;
				int relx;
				int rely;
				int angle;
				int hitangle;
				int hit[2];
		};
	pos pos;
};
class background{
	public:
		int larg;
		int alt;
		class pos{
			public:
				int x;
				int y;
		};
	pos pos;
};
player player;
enemy enemy;
background bac;
int xpos=0;
int ypos=0;
int posx = 0;
int posy = 0;
int hitx = 0;
int hity = 0;
//=======================================================
void screenflashlight(int angle,int centerx,int centery,int radius){
	double nangle=angle*(conv);
	double mangle=(angle-30)*(conv);
	xpos=(int)(centerx+cos(nangle)*radius);
	ypos=(int)(centery+sin(nangle)*radius);
	posx=(int)(centerx+cos(mangle)*radius);
	posy=(int)(centery+sin(mangle)*radius);
}
void hitbox(int x, int y, int angle, int centerx, int centery){
	double h = sqrt(((x-centerx)*(x-centerx))+((y-centery)*(y-centery)));
	hitx = (int)(h*cos(angle*conv))+centerx;
	hity = (int)(h*sin(angle*conv))+centery;

}
//========================================================
int main(){
	//Define stuff===========================================================================================
	initwindow(res[0],res[1]);
    int done = 0;
    int ca,ascii,pg;
    double co;
    bool pressed;
    enemy.pos.relx = 400;
    enemy.pos.rely = 700;
    enemy.tamanho = 5;
    player.pos.x = 500;
    player.pos.y = 500;
    player.alcance = 450;
    player.pos.angle = 270;
    bac.larg = 1920;
    bac.alt = 1080;
    bac.pos.x = player.pos.x-((int)bac.larg/2);
    bac.pos.y = player.pos.y-((int)bac.alt/2);
    player.pos.relx = player.pos.x - bac.pos.x;
    player.pos.rely = player.pos.y - bac.pos.y;
	int bg[10] = {0,0,1000,0,1000,1000,0,1000,0,0};
    int flash[8];
	char kb;
	//Play stuff===========================================================================================
	cleardevice();
    while(done == 0){
    	for(pg = 1; pg<=2;pg++){
    		setactivepage(pg);
    		cleardevice();
	    	//Move stuff=======================================================================================
	    	player.pos.relx = player.pos.x - bac.pos.x;
	    	player.pos.rely = player.pos.y - bac.pos.y;
	    	enemy.pos.x = player.pos.x+(enemy.pos.rely-player.pos.relx);
	    	enemy.pos.y = player.pos.y+(enemy.pos.rely-player.pos.rely);
	    	pressed = kbhit();
	       	if(pressed){
	        	kb = getch();
	        	ascii = (int)kb;
	        	if(ascii == 119 && player.pos.rely > 0){
	        		printf("UP\n");
	        		bac.pos.y+=5;
	        		printf("%d",player.pos.rely);
				}
				else if(ascii == 97 && player.pos.relx > 0){
	        		printf("LEFT\n");
	        		bac.pos.x+=5;
	        		printf("%d",player.pos.relx);
				}
	 			else if(ascii ==  115 &&  player.pos.rely < bac.alt){
	        		printf("DOWN\n");
	        		bac.pos.y-=5;
				}
				else if(ascii == 100 && player.pos.relx < bac.larg){
	        		printf("RIGHT\n");
	        		bac.pos.x-=5;
				}
				else if(ascii == 77){
	        		printf("Clockwise\n");
	        		player.pos.angle+=2;
				}
				else if(ascii == 75){
	        		printf("Counter-Clockwise\n");
	        		player.pos.angle-=2;
				}
				pressed = false;
				kb = NULL;
				ascii = NULL;
			}
			kb = NULL;
			ascii = NULL;
			if(player.pos.angle>359){
	            player.pos.angle = 0;
	        }
			if(player.pos.angle<0){
	            player.pos.angle = 359;
	        }
	        //Hit stuff=======================================================================================
	    	enemy.pos.angle = (int)(atan((float)(enemy.pos.y-player.pos.y)/(float)(enemy.pos.x-player.pos.x))*(180/3.14159265359));
			if(enemy.pos.angle < 0){
				enemy.pos.angle+=180;
			}	
			if(enemy.pos.y<player.pos.y){
				enemy.pos.angle+=180;
			}
			enemy.pos.hitangle = enemy.pos.angle-player.pos.angle;
			hitbox(enemy.pos.x,enemy.pos.y,enemy.pos.hitangle,player.pos.x,player.pos.y);
			enemy.pos.hit[0] = hitx;
			enemy.pos.hit[1] = hity;
			player.pos.hit[0] = player.pos.x+player.alcance;
			player.pos.hit[1] = player.pos.y;
			enemy.color = {0,255,0};
			if(enemy.pos.hit[0]>player.pos.x){
	   			ca = (enemy.pos.hit[0]-player.pos.x);
	   	    	co = ca*tan(15*conv);
	   	    	if (abs(enemy.pos.hit[1]-player.pos.y)<co && sqrt(((enemy.pos.x-player.pos.x)*(enemy.pos.x-player.pos.x))+((enemy.pos.y-player.pos.y)*(enemy.pos.y-player.pos.y)))<=player.alcance){
	   				enemy.color = {255,0,0};
	   			}
	        }
	        //Draw stuff=======================================================================================
	        readimagefile("Images/teste.bmp",bac.pos.x,bac.pos.y,bac.larg+bac.pos.x,bac.alt+bac.pos.y);
	        setcolor(RGB(255,255,0));
	        setfillstyle(1,RGB(255,255,0));
	        screenflashlight(player.pos.angle+15,player.pos.x,player.pos.y,player.alcance);
	        flash={player.pos.x,player.pos.y,xpos,ypos,posx,posy,player.pos.x,player.pos.y};
	        fillpoly(4,flash);
	        if(enemy.pos.x > -5 && enemy.pos.x < res[0]+5 && enemy.pos.y > -5 && enemy.pos.y < res[1]+5){
	        	setcolor(RGB(enemy.color[0],enemy.color[1],enemy.color[2]));
	        	setfillstyle(1,RGB(enemy.color[0],enemy.color[1],enemy.color[2]));
	        	fillellipse(enemy.pos.x,enemy.pos.y,enemy.tamanho,enemy.tamanho);
			}
	        setcolor(RGB(0,255,255));
	        setfillstyle(1,RGB(0,255,255));
	        fillellipse(player.pos.x,player.pos.y,10,10);
	        setvisualpage(pg);
	        delay(16);
		}
	}
	printf("\n");
	system("pause");
}



