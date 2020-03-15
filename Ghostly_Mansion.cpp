#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <graphics.h>
#include <time.h>
static double conv = (3.14159265359/180);
int res[2] = {1920,1080};
//Classify stuff=======================================================
struct player{
	int tamanho,health,score;
	bool key,moving;
	char atlas[8][8][15];
    struct pos{
        int x,y,angle,relx,rely,direction;
        int hit[2];
    };
    struct lanterna{
    	int angle,alcance,bateria;
	};
    pos pos;
    lanterna lanterna;
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
struct battery{
	bool spawned;
	int tamanho;
	int x,y,relx,rely;
};
struct key{
	bool spawned;
	int tamanho;
	int x,y,relx,rely;
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
battery battery1;
battery battery2;
battery battery[2] = {battery1,battery2};
key key;
background bac;
int xpos=0;
int ypos=0;
int posx = 0;
int posy = 0;
int hitx = 0;
int hity = 0;
//=======================================================
void screenflashlight(int angle,int centerx,int centery,int radius,int flashangle){
	double nangle=angle*(conv);
	double mangle=(angle-flashangle)*(conv);
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
		enemy[enem].pos.relx = ((int)enemy[enem].pos.relx)%bac.larg-enemy[enem].tamanho;
		if(enemy[enem].pos.relx < enemy[enem].tamanho ){
			enemy[enem].pos.relx+=enemy[enem].tamanho;
		}
		enemy[enem].pos.rely = rand();
		enemy[enem].pos.rely = ((int)enemy[enem].pos.rely)%bac.alt-enemy[enem].tamanho;
		if(enemy[enem].pos.rely < enemy[enem].tamanho ){
			enemy[enem].pos.rely+=enemy[enem].tamanho;
		}
		enemy[enem].pos.walkingangle = rand()%360;
    	enemy[enem].spawned = 1;
    	enemy[enem].health = 100;
    	//printf("Enemy[%d] Spawnou\n",enem);
	}
	//printf("Inimigo %d: posicao x: %d, posicao y: %d, spawn: %d\n",enem,enemy[enem].pos.relx,enemy[enem].pos.rely,enemy[enem].spawned);
}
//========================================================
int main(){
	srand((unsigned)time(NULL));
	//Define stuff===========================================================================================
	initwindow(res[0],res[1]);
    int done = 0;
    int ca,pg,index;
    double co;
    for (index = 0;index < num; index++){
    	    enemy[index].tamanho = 10;
    	    enemy[index].spawned = 0;
    	    enemy[index].speed = 7;
	}
	for(index = 0; index < 2; index++){
		battery[index].spawned = 0;
		battery[index].tamanho = 5;
	}
	key.tamanho = 5;
	key.spawned = false;
    bac.larg = 1920;
    bac.alt = 1080;
    player.pos.x = (int)(res[0]/2);
    player.pos.y = (int)(res[1]/2);
    player.lanterna.alcance = 450;
    player.lanterna.angle = 30;
    player.pos.angle = 270;
    player.tamanho = 64;
    player.key = false;
    bac.pos.x = player.pos.x-((int)bac.larg/2);
    bac.pos.y = player.pos.y-((int)bac.alt/2);
    player.pos.relx = player.pos.x - bac.pos.x;
    player.pos.rely = player.pos.y - bac.pos.y;
    player.health = 100;
    player.lanterna.bateria = 15;
	int bg[10] = {0,0,1000,0,1000,1000,0,1000,0,0};
    int flash[8];
    int chance;
	unsigned int cicles = 0;
	player.atlas[0] = {"Images/w1.bmp","Images/w2.bmp","Images/w3.bmp","Images/w4.bmp","Images/w5.bmp","Images/w6.bmp","Images/w7.bmp","Images/w8.bmp"};
	player.atlas[1] = {"Images/wd1.bmp","Images/wd2.bmp","Images/wd3.bmp","Images/wd4.bmp","Images/wd5.bmp","Images/wd6.bmp","Images/wd7.bmp","Images/wd8.bmp"};
	player.atlas[2] = {"Images/d1.bmp","Images/d2.bmp","Images/d3.bmp","Images/d4.bmp","Images/d5.bmp","Images/d6.bmp","Images/d7.bmp","Images/d8.bmp"};
	player.atlas[3] = {"Images/sd1.bmp","Images/sd2.bmp","Images/sd3.bmp","Images/sd4.bmp","Images/sd5.bmp","Images/sd6.bmp","Images/sd7.bmp","Images/sd8.bmp"};
	player.atlas[4] = {"Images/s1.bmp","Images/s2.bmp","Images/s3.bmp","Images/s4.bmp","Images/s5.bmp","Images/s6.bmp","Images/s7.bmp","Images/s8.bmp"};
	player.atlas[5] = {"Images/sa1.bmp","Images/sa2.bmp","Images/sa3.bmp","Images/sa4.bmp","Images/sa5.bmp","Images/sa6.bmp","Images/sa7.bmp","Images/sa8.bmp"};
	player.atlas[6] = {"Images/a1.bmp","Images/a2.bmp","Images/a3.bmp","Images/a4.bmp","Images/a5.bmp","Images/a6.bmp","Images/a7.bmp","Images/a8.bmp"};
	player.atlas[7] = {"Images/wa1.bmp","Images/wa2.bmp","Images/wa3.bmp","Images/wa4.bmp","Images/wa5.bmp","Images/wa6.bmp","Images/wa7.bmp","Images/wa8.bmp"};
	//Play stuff===========================================================================================
	cleardevice();
	getch();
    while(done == 0){
    	for(pg = 1; pg<=2;pg++){
    		setactivepage(pg);
    		cleardevice();
    		//Spawn stuff=======================================================================================
    		if(player.key == false){
    			for(index = 0; index <num ; index ++){
	    			 if(enemy[index].spawned == 0){
	    			 	spawnenemies(index);
					 }	
				}
			}
			for(index = 0; index <2 ; index ++){
				chance=rand()%100;
				if(battery[index].spawned == 0 && chance == 0){
					battery[index].spawned = 1;
					battery[index].relx = rand()%bac.larg;
					battery[index].rely = rand()%bac.alt;	
					battery[index].x = bac.pos.x+battery[index].relx;
	    			battery[index].y = bac.pos.y+battery[index].rely;
				}
			}
			if(player.score >= 30 && key.spawned == false){
				key.spawned = true;
				player.score = 0;
				key.relx = rand()%bac.larg;
			 	key.rely = rand()%bac.alt;
				key.x = bac.pos.x+key.relx;
    			key.y = bac.pos.y+key.rely;
				}	
			//Mechanics Stuff==================================================================================
			if(cicles%5== 0){
				if(player.lanterna.alcance>=0){
					player.lanterna.alcance-=1;
				}
			}
			if (player.key == true){
				for (index = 0; index < num; index++){
	    				enemy[index].spawned = false;
					}
			}
	    	//Move stuff=======================================================================================
	    	player.pos.relx = player.pos.x - bac.pos.x;
	    	player.pos.rely = player.pos.y - bac.pos.y;
	    	player.moving = false;
	    	for(index = 0;index < num; index++){
	    		enemy[index].pos.x = player.pos.x+(int)(enemy[index].pos.relx-player.pos.relx);
	    		enemy[index].pos.y = player.pos.y+(int)(enemy[index].pos.rely-player.pos.rely);
			}
	    	
      		if( (GetAsyncKeyState('W') & 0x8000) && player.pos.rely-player.tamanho > 0){
	        	bac.pos.y+=5;
	        	player.moving = true;
			}
			if((GetAsyncKeyState('A') & 0x8000) && player.pos.relx-player.tamanho > 0){
	       		bac.pos.x+=5;
	       		player.moving = true;
			}
 			if((GetAsyncKeyState('S') & 0x8000) && player.pos.rely+player.tamanho < bac.alt){
        		bac.pos.y-=5;
        		player.moving = true;
			}
			if((GetAsyncKeyState('D') & 0x8000) && player.pos.relx+player.tamanho < bac.larg){
        		bac.pos.x-=5;
        		player.moving = true;
			}
			ca = mousex()-player.pos.x;
			co = mousey()-player.pos.y;
			player.pos.angle = (int)(atan(co/ca)*(180/3.14159265359));
			if (player.pos.angle<0){
				player.pos.angle+=180;
			}
			if(mousey()<player.pos.y){
				player.pos.angle+=180;
			}
			if(mousex()<player.pos.x && mousey() == player.pos.y){
				player.pos.angle == 180;
			}
			if(player.pos.angle>359){
	            player.pos.angle = 0;
	        }
			if(player.pos.angle<0){
	            player.pos.angle = 359;
	        }
	        if(player.pos.angle > 69 && player.pos.angle <= 112){
				player.pos.direction = 4;
			}
			else if(player.pos.angle > 112 && player.pos.angle <= 157){
				player.pos.direction = 5;
			}
			else if(player.pos.angle > 157 && player.pos.angle <= 202){
				player.pos.direction = 6;
			}
			else if(player.pos.angle > 202 && player.pos.angle <= 247){
				player.pos.direction = 7;
			}
			else if(player.pos.angle > 247 && player.pos.angle <= 292){
				player.pos.direction = 0;
			}
			else if(player.pos.angle > 292 && player.pos.angle <= 337){
				player.pos.direction = 1;
			}			
			else if((player.pos.angle > 337 && player.pos.angle <= 360) || (player.pos.angle > 0 && player.pos.angle <= 23)){
				player.pos.direction = 2;
			}
			else if(player.pos.angle > 23 && player.pos.angle <= 68){
				player.pos.direction = 3;
			}
			for(index = 0; index < num; index ++){
				if(enemy[index].spawned == 1){
					chance = rand()%5;
					if (enemy[index].pos.relx-enemy[index].tamanho < 0 || enemy[index].pos.rely-enemy[index].tamanho < 0 || enemy[index].pos.relx+enemy[index].tamanho > bac.larg || enemy[index].pos.rely+enemy[index].tamanho > bac.alt){
						enemy[index].pos.walkingangle += 180;
					}
					else if (cicles%60 == 0 && chance == 0){
						enemy[index].pos.walkingangle = rand()%360;
					}
				enemy[index].pos.relx+=cos(enemy[index].pos.walkingangle*conv)*(int)enemy[index].speed;
		       	enemy[index].pos.rely+=sin(enemy[index].pos.walkingangle*conv)*(int)enemy[index].speed;
		       	//enemy[index].speed+=1;
				}				
			}
			for(index=0;index<2;index++){
				if(battery[index].spawned == true){
					battery[index].x = bac.pos.x+battery[index].relx;
	    			battery[index].y = bac.pos.y+battery[index].rely;
				}
			}
			if(key.spawned == true){
					key.x = bac.pos.x+key.relx;
	    			key.y = bac.pos.y+key.rely;
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
				player.pos.hit[0] = player.pos.x+player.lanterna.alcance;
				player.pos.hit[1] = player.pos.y;
				enemy[index].color = {0,255,0};
				if(enemy[index].pos.hit[0]>player.pos.x && (enemy[index].pos.hitangle < 90 || enemy[index].pos.hitangle > 270) && enemy[index].spawned == 1){
		   			ca = (enemy[index].pos.hit[0]-player.pos.x);
		   	    	co = ca*tan((player.lanterna.angle/2)*conv);
		   	    	if (abs(enemy[index].pos.hit[1]-player.pos.y)<co && sqrt(((enemy[index].pos.x-player.pos.x)*(enemy[index].pos.x-player.pos.x))+((enemy[index].pos.y-player.pos.y)*(enemy[index].pos.y-player.pos.y)))<=player.lanterna.alcance){
		   				enemy[index].color = {255,0,0};
		   				enemy[index].health -= (int)((abs(100/co))+(abs(100/ca)));
		   			}
		        }
		        if(enemy[index].health < 0){
		        	enemy[index].spawned = false;
		        	enemy[index].health = NULL;
		        	player.score+=1;
		        	printf("Score: %d\n",player.score);
		        	
				}
				if(enemy[index].pos.x+enemy[index].tamanho>=player.pos.x-player.tamanho && enemy[index].pos.x-enemy[index].tamanho<=player.pos.x+player.tamanho){
					if(enemy[index].pos.y+enemy[index].tamanho>=player.pos.y-player.tamanho && enemy[index].pos.y-enemy[index].tamanho<=player.pos.y+player.tamanho){
						player.health-=1;
						//printf("Vida: %d\n",player.health);
					}
				}
			}
			for(index=0;index<2;index++){
				if(battery[index].spawned == true && battery[index].relx >= player.pos.relx-player.tamanho && battery[index].relx <= player.pos.relx+player.tamanho && battery[index].rely >= player.pos.rely-player.tamanho && battery[index].rely <= player.pos.rely+player.tamanho){
					battery[index].spawned = false;
					battery[index].relx = 0;
					battery[index].rely = 0;	
					battery[index].x = 0;
	    			battery[index].y = 0;
	    			player.lanterna.alcance+=50;
	    			printf("Spawn bateria %d: %d\n",index,battery[index].spawned);
				}
			}
			if(player.lanterna.alcance>450){
	    		player.lanterna.alcance=450;
			}
			if(key.spawned == true && key.relx >= player.pos.relx-player.tamanho && key.relx <= player.pos.relx+player.tamanho && key.rely >= player.pos.rely-player.tamanho && key.rely <= player.pos.rely+player.tamanho){
					key.spawned = false;
					key.relx = 0;
					key.rely = 0;	
					key.x = 0;
	    			key.y = 0;
	    			player.key = true;
				}
	        //Draw stuff=======================================================================================
	        readimagefile("Images/teste.bmp",bac.pos.x,bac.pos.y,bac.larg+bac.pos.x,bac.alt+bac.pos.y);
	        setcolor(RGB(255,255,0));
	        setfillstyle(1,RGB(255,255,0));
	        screenflashlight(player.pos.angle+15,player.pos.x,player.pos.y,player.lanterna.alcance,player.lanterna.angle);
	        flash={player.pos.x,player.pos.y,xpos,ypos,posx,posy,player.pos.x,player.pos.y};
	        fillpoly(4,flash);
	  		for(index = 0;index < num; index++){
		        if(enemy[index].pos.x > - enemy[index].tamanho && enemy[index].pos.x < res[0]+enemy[index].tamanho && enemy[index].pos.y > -enemy[index].tamanho && enemy[index].pos.y < res[1]+enemy[index].tamanho && enemy[index].spawned == 1){
		        	setcolor(RGB(enemy[index].color[0],enemy[index].color[1],enemy[index].color[2]));
		        	setfillstyle(1,RGB(enemy[index].color[0],enemy[index].color[1],enemy[index].color[2]));
		        	fillellipse(enemy[index].pos.x,enemy[index].pos.y,enemy[index].tamanho,enemy[index].tamanho);
				}
			}
			setcolor(RGB(255,0,255));
	        setfillstyle(1,RGB(255,0,255));
			for(index=0;index < 2; index++){
				if(battery[index].spawned == true){
					fillellipse(battery[index].x,battery[index].y,battery[index].tamanho,battery[index].tamanho);	
				}
			}
			setcolor(RGB(0,0,255));
	        setfillstyle(1,RGB(0,0,255));
			if(key.spawned == true){
				fillellipse(key.x,key.y,key.tamanho,key.tamanho);	
			}
			if(player.moving){
				readimagefile(player.atlas[player.pos.direction][cicles%8],player.pos.x-(player.tamanho/2),player.pos.y-(player.tamanho/2),player.pos.x+(player.tamanho/2),player.pos.y+(player.tamanho/2));
			}
			else{
				readimagefile(player.atlas[player.pos.direction][0],player.pos.x-(player.tamanho/2),player.pos.y-(player.tamanho/2),player.pos.x+(player.tamanho/2),player.pos.y+(player.tamanho/2));
			}
	        //Win/Lose Stuff=========================================================================================
	        if(player.health<=0){
				done = 1;
				cleardevice();
				setvisualpage(pg);
				printf("GAME OVER!\n");
				break;
			}
			else if(player.key == true && player.pos.relx >= (bac.larg/2)-15 && player.pos.relx <= (bac.larg/2)+15 && player.pos.rely <= player.tamanho){
				done = 1;
				cleardevice();
				setvisualpage(pg);
				printf("YOU WIN!\n");
				break;
			}
	        setvisualpage(pg);
	        cicles++;
	        if(cicles >= 65534){
	        	cicles = 0;
			}
		}
	}
	cleardevice();
	printf("\n");
	system("pause");
}
