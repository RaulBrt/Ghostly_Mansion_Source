#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <graphics.h>
#include <time.h>
static double conv = (3.14159265359/180);
int res[2] = {1920,1080};

//Classify stuff=======================================================

struct player{
	int alcance,tamanho,health;
    struct pos{
        int x;
        int y;
        int angle;
        int hit[2];
        int relx;
		int rely;
    };
    pos pos;
};
struct enemy{
		int tamanho,health;
		int color[3];
		float speed;
		bool spawned;
		struct pos{
				int x,y,angle,hitangle,walkingangle;
				int hit[2];
				float relx,rely;
		};
	pos pos;
};
struct background{
		int larg;
		int alt;
		struct pos{
				int x;
				int y;
		};
	pos pos;
};
int num = 20;
player player;
enemy enemy1;
enemy enemy2;
enemy enemy3;
enemy enemy4;
enemy enemy5;
enemy enemy6;
enemy enemy7;
enemy enemy8;
enemy enemy9;
enemy enemy10;
enemy enemy11;
enemy enemy12;
enemy enemy13;
enemy enemy14;
enemy enemy15;
enemy enemy16;
enemy enemy17;
enemy enemy18;
enemy enemy19;
enemy enemy20;
enemy enemy[20] = {enemy1,enemy2,enemy3,enemy4,enemy5,enemy6,enemy7,enemy8,enemy9,enemy10,enemy11,enemy12,enemy13,enemy14,enemy15,enemy16,enemy17,enemy18,enemy19,enemy20};
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
void spawnenemies(int enem){	
	int chance = rand();
	if (chance%100 == 0){
		enemy[enem].pos.relx = rand();
		enemy[enem].pos.relx = ((int)enemy[enem].pos.relx)%bac.larg-5;
		enemy[enem].pos.rely = rand();
		enemy[enem].pos.rely = ((int)enemy[enem].pos.rely)%bac.alt-5;
		enemy[enem].pos.walkingangle = rand()%360;
    	enemy[enem].spawned = 1;
    	enemy[enem].health = 100;
    	printf("Enemy[%d] Spawnou\n",enem);
	}
	//printf("Inimigo %d: posicao x: %d, posicao y: %d, spawn: %d\n",enem,enemy[enem].pos.relx,enemy[enem].pos.rely,enemy[enem].spawned);
	
}
//========================================================
int main(){
	srand((unsigned)time(NULL));
	//Define stuff===========================================================================================
	initwindow(res[0],res[1]);
    int done = 0;
    int ca,pg,index,cicles;
    double co;
    for (index = 0;index < num; index++){
    	    enemy[index].tamanho = 5;
    	    enemy[index].spawned = 0;
    	    enemy[index].speed = 0;
	}
    bac.larg = 920;
    bac.alt = 540;
    player.pos.x = (int)(res[0]/2);
    player.pos.y = (int)(res[1]/2);
    player.alcance = 450;
    player.pos.angle = 270;
    player.tamanho = 10;
    bac.pos.x = player.pos.x-((int)bac.larg/2);
    bac.pos.y = player.pos.y-((int)bac.alt/2);
    player.pos.relx = player.pos.x - bac.pos.x;
    player.pos.rely = player.pos.y - bac.pos.y;
    player.health = 100;
	int bg[10] = {0,0,1000,0,1000,1000,0,1000,0,0};
    int flash[8];
    int chance;
	cicles = 0;
	//Play stuff===========================================================================================
	cleardevice();
	getch();
    while(done == 0){
    	for(pg = 1; pg<=2;pg++){
    		setactivepage(pg);
    		cleardevice();
    		//Spawn stuff=======================================================================================
    		for(index = 0; index <num ; index ++){
    			 if(enemy[index].spawned == 0){
    			 	spawnenemies(index);
				 }	
			}
			
	    	//Move stuff=======================================================================================
	    	player.pos.relx = player.pos.x - bac.pos.x;
	    	player.pos.rely = player.pos.y - bac.pos.y;
	    	for(index = 0;index < num; index++){
	    		enemy[index].pos.x = player.pos.x+(int)(enemy[index].pos.relx-player.pos.relx);
	    		enemy[index].pos.y = player.pos.y+(int)(enemy[index].pos.rely-player.pos.rely);
			}
	    	
      		if( (GetAsyncKeyState('W') & 0x8000) && player.pos.rely-player.tamanho > 0){
	        	bac.pos.y+=5;
			}
			if((GetAsyncKeyState('A') & 0x8000) && player.pos.relx-player.tamanho > 0){
	       		bac.pos.x+=5;
			}
 			if((GetAsyncKeyState('S') & 0x8000) && player.pos.rely+player.tamanho < bac.alt){
        		bac.pos.y-=5;
			}
			if((GetAsyncKeyState('D') & 0x8000) && player.pos.relx+player.tamanho < bac.larg){
        		bac.pos.x-=5;
			}
			if(GetAsyncKeyState(VK_RIGHT) & 0x8000){
        		player.pos.angle+=5;
			}
			if(GetAsyncKeyState(VK_LEFT) & 0x8000){
        		player.pos.angle-=5;
			}
			if(player.pos.angle>359){
	            player.pos.angle = 0;
	        }
			if(player.pos.angle<0){
	            player.pos.angle = 359;
	        }
			for(index = 0; index < num; index ++){
				if(enemy[index].spawned == 1){
					chance = rand()%10;
					if (enemy[index].pos.relx-enemy[index].tamanho < 0 || enemy[index].pos.rely-enemy[index].tamanho < 0 || enemy[index].pos.relx+enemy[index].tamanho > bac.larg || enemy[index].pos.rely+enemy[index].tamanho > bac.alt){
						enemy[index].pos.walkingangle += 180;
					}
					else if (cicles >= 60 && chance == 0){
						enemy[index].pos.walkingangle = rand()%360;
					}
				enemy[index].pos.relx+=cos(enemy[index].pos.walkingangle*conv)*(int)enemy[index].speed;
		       	enemy[index].pos.rely+=sin(enemy[index].pos.walkingangle*conv)*(int)enemy[index].speed;
		       	enemy[index].speed+=1;
				}				
			}
	        //Hit stuff=======================================================================================
	        for(index = 0;index < num; index++){
		    	enemy[index].pos.angle = (int)(atan((float)(enemy[index].pos.y-player.pos.y)/(float)(enemy[index].pos.x-player.pos.x))*(180/3.14159265359));
				if(enemy[index].pos.angle < 0){
					enemy[index].pos.angle+=180;
				}	
				if(enemy[index].pos.y<player.pos.y){
					enemy[index].pos.angle+=180;
				}
				enemy[index].pos.hitangle = enemy[index].pos.angle-player.pos.angle;
				hitbox(enemy[index].pos.x,enemy[index].pos.y,enemy[index].pos.hitangle,player.pos.x,player.pos.y);
				enemy[index].pos.hit[0] = hitx;
				enemy[index].pos.hit[1] = hity;
				player.pos.hit[0] = player.pos.x+player.alcance;
				player.pos.hit[1] = player.pos.y;
				enemy[index].color = {0,255,0};
				if(enemy[index].pos.hit[0]>player.pos.x && (enemy[index].pos.hitangle < 90 || enemy[index].pos.hitangle > 270) && enemy[index].spawned == 1){
		   			ca = (enemy[index].pos.hit[0]-player.pos.x);
		   	    	co = ca*tan(15*conv);
		   	    	if (abs(enemy[index].pos.hit[1]-player.pos.y)<co && sqrt(((enemy[index].pos.x-player.pos.x)*(enemy[index].pos.x-player.pos.x))+((enemy[index].pos.y-player.pos.y)*(enemy[index].pos.y-player.pos.y)))<=player.alcance){
		   				enemy[index].color = {255,0,0};
		   				enemy[index].health -= (int)((abs(100/co))+(abs(100/ca)));
		   			}
		        }
		        if(enemy[index].health < 0){
		        	enemy[index].spawned = false;
		        	enemy[index].health = NULL;
		        	printf("Enemy[%d] morreu\n",index);
		        	
				}
				if(enemy[index].pos.x+enemy[index].tamanho>=player.pos.x-player.tamanho && enemy[index].pos.x-enemy[index].tamanho<=player.pos.x+player.tamanho){
					if(enemy[index].pos.y+enemy[index].tamanho>=player.pos.y-player.tamanho && enemy[index].pos.y-enemy[index].tamanho<=player.pos.y+player.tamanho){
						player.health-=1;
						printf("Vida: %d\n",player.health);
					}
				}
			}
			if(player.health<=0){
				done = 1;
				cleardevice();
				setvisualpage(pg);
				break;
			}
	        //Draw stuff=======================================================================================
	        readimagefile("Images/teste.bmp",bac.pos.x,bac.pos.y,bac.larg+bac.pos.x,bac.alt+bac.pos.y);
	        setcolor(RGB(255,255,0));
	        setfillstyle(1,RGB(255,255,0));
	        screenflashlight(player.pos.angle+15,player.pos.x,player.pos.y,player.alcance);
	        flash={player.pos.x,player.pos.y,xpos,ypos,posx,posy,player.pos.x,player.pos.y};
	        fillpoly(4,flash);
	  		for(index = 0;index < num; index++){
		        if(enemy[index].pos.x > -5 && enemy[index].pos.x < res[0]+5 && enemy[index].pos.y > -5 && enemy[index].pos.y < res[1]+5 && enemy[index].spawned == 1){
		        	setcolor(RGB(enemy[index].color[0],enemy[index].color[1],enemy[index].color[2]));
		        	setfillstyle(1,RGB(enemy[index].color[0],enemy[index].color[1],enemy[index].color[2]));
		        	fillellipse(enemy[index].pos.x,enemy[index].pos.y,enemy[index].tamanho,enemy[index].tamanho);
				}
			}
	        setcolor(RGB(0,255,255));
	        setfillstyle(1,RGB(0,255,255));
	        fillellipse(player.pos.x,player.pos.y,10,10);
	        setvisualpage(pg);
	        cicles++;
		}
	}
	cleardevice();
	printf("GAME OVER!");
	printf("\n");
	system("pause");
}



