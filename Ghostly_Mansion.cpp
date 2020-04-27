#include <stdlib.h> 	//
#include <stdio.h>		//
#include <math.h>		//Bibliotecas utilizadas
#include <graphics.h>	//
#include <time.h>		//
static double conv = (3.14159265359/180); 	//Fator de conversão de graus para radianos
int res[2] = {1600,900}; 					//Tamanho da tela em pixeis
//Structfy stuff=======================================================
struct player{ 										//Struct para organizar as variaveis do jogador
	int tamanho,health,score;							//Variaveis para guardar o tamanho da imagem, a vida e a pontuação do jogador respectivamente
	bool key,moving;									//Variaveis para saber se o jogador já pegou a chave da fase e para verificar se o jogador está andando respectivamente
	char atlas[8][5][25];								//Array de três indices para guardar as "Strings" dos nomes das imagens que compoem a animação de caminhada do jogador
    struct pos{											//Struct para organizar as variaveis de posição do jogador
        int x,y,angle,relx,rely,direction;					//Variaveis para guardar as coordenadas x,y na tela, o angulo em que o jogador está olhando, as coordenadas x,y em relação ao canto superior esquerdo do mapa e o indice da direção que o jogador está olhando
    };
    struct lanterna{									//Struct para organizar as variaveis da lanterna do jogador
    	int angle,alcance,bateria;								//Variaveis para guardar o angulo formado pelo feixe de luz, a distancia e a quantidade de "vida" do jogador/lanterna
	};
    pos pos;											//Criar o objeto pos que referencia a struct pos
    lanterna lanterna;									//Criar o objeto lanterna que referencia a struct lanterna
};
struct enemy{										//Struct para organizar as variaveis dos inimigos
		int tamanho,health;								//Variaveis para guardar o tamanho da imagem e a vida do inimigo
		int color[3];
		float speed;									//Variavel para guardar a velocidade que o inimigo anda
		bool spawned;									//Variavel para saber se o inimigo já está na tela
		struct pos{										//Struct para organizar as variaveis de posição do inimigo
				int x,y,angle,hitangle,walkingangle,direction;		//Variaveis para guardar as coordenadas x,y na tela, o angulo em relacão ao eixo x, o anglo formado entre a posição do inimigo e a lanterna e a direção que o inimigo está andando
				int hit[2];									//Array para guardar as coordenadas x,y da hitbox do inimigo em relação a lanterna do jogador
				float relx,rely;							//Variaveis para guardar as coordenadas x,y em relação ao canto superior esquerdo do mapa
		};
	pos posi;											//Criar o objeto posi que referencia a struct posi
};
struct boss{
	int tamanho,health;
	float speed;
	int color[3];	
	struct pos{				
		int x,y,angle,hitangle,walkingangle,direction;
		int hit[2];
	};
	pos pos;											//Criar o objeto pos que referencia a struct pos
};
struct background{									//Struct para organizar as variaveis do background
		struct pos{										//Struct para organizar as coordenadas x,y do background
				int x,y;									//Variaveis para guardar as coordenadas x,y do background
		};
		struct parede{
			int alt,larg;
			char* imagem;		
		};
		struct chao{
			int alt,larg;
			char* imagem;		
		};
	pos pos;	
	parede parede;
	chao chao;										//Criar o objeto pos que referencia a struct pos
};
struct battery{										//Struct para organizar as variaveis das baterias
	bool spawned;										//Variavel para se se a bateria já está na tela
	int x,y,relx,rely,tamanho;							//Variaveis para guardar as coordenadas x,y na tela , as coordenadas x,y em relação ao canto superior esquerdo do mapa e o tamanho da imagem da bateria
};
struct key{											//Struct para organizar as variaveis da chave
	bool spawned;										//Variavel para se se a chave já está na tela
	int x,y,relx,rely,tamanho;							//Variaveis para guardar as coordenadas x,y na tela , as coordenadas x,y em relação ao canto superior esquerdo do mapa e o tamanho da imagem da chave
};
//Define Global Stuff===================================================================
int num;										//Variavel para guardar o numero maximo de inimigos na tela ao mesmo tempo
player player;									//Criar o objeto player que referencia a struct player
enemy *enmy;
battery battery[2];			//Array para guardar as baterias
key key;										//Criar o objeto key que referencia a struct key
background bac;										//Criar o objeto bac que referencia a struct background
boss boss;
int xpos = 0; 						//<=
int ypos = 0;						//<= Variaveis para guardar os vertices do
int posx = 0;						//<= polgono da lanterna
int posy = 0;						//<=
int hitx = 0;					//<= Variaveis para guardar momentaneamente
int hity = 0;					//<= a posição da hitbox do inimigo
int ca,pg,index;
double co;
unsigned int cicles = 0;
int flash[8];
int chance;
int fase = 0;
bool done,result;
char enmyatlas[8][5][25];
//Make stuff function =======================================================
void screenflashlight(int angle,int centerx,int centery,int radius,int flashangle){ //Função para calcular os vertices do feixe de luz da lanterna
	double nangle=angle*(conv); 														//<= Converter angulos de graus para radianos
	double mangle=(angle-flashangle)*(conv);											//<= (angle = angulo em que o jogador está olhando) (flashangle = angulo formado pelo feixe de luz da lanterna)
	xpos=(int)(centerx+cos(nangle)*radius);													//<= Calcular as coordenadas x,y
	ypos=(int)(centery+sin(nangle)*radius);													//<= do vertice 'A' da luz da lanterna
	posx=(int)(centerx+cos(mangle)*radius);												//<= Calcular as coordenadas x,y
	posy=(int)(centery+sin(mangle)*radius);												//<= do vertice 'B' da luz da lanterna
}
void hitbox(int x, int y, int angle, int centerx, int centery){						//Função para calcular a hitbox do inimigo
	double h = sqrt(((x-centerx)*(x-centerx))+((y-centery)*(y-centery)));				//Distancia em linha reta do inimigo até o jogador
	hitx = (int)(h*cos(angle*conv))+centerx;												//<= Calcular as coordenadas x,y
	hity = (int)(h*sin(angle*conv))+centery;												//<= da hitbox de um inimigo
}
void spawnenemies(int enem){														//Função para fazer inimigos aparecerem na tela
	int chance = rand();																//Escolhe um numero aleatório
	if (chance%100 == 0){																	//Cada inimigo tem 1% de chance de entrar em jogo a cada frame
		enmy[enem].posi.relx = rand();													//<= Escolhe uma coordenada x aleatoriamente
		enmy[enem].posi.relx = ((int)enmy[enem].posi.relx)%bac.chao.larg-enmy[enem].tamanho;//<= e o reposiciona para dentro do mapa
		if(enmy[enem].posi.relx < enmy[enem].tamanho ){									//Caso a posição escolhida fizesse o inimigo aparescer com uma parte fora do mapa
			enmy[enem].posi.relx+=enmy[enem].tamanho;											//Corrige a posição
		}
		enmy[enem].posi.rely = rand();
		enmy[enem].posi.rely = ((int)enmy[enem].posi.rely)%bac.chao.alt-enmy[enem].tamanho;
		if(enmy[enem].posi.rely < enmy[enem].tamanho ){
			enmy[enem].posi.rely+=enmy[enem].tamanho;
		}
		enmy[enem].posi.walkingangle = rand()%360;
    	enmy[enem].spawned = 1;
    	enmy[enem].health = 100;
    	//printf("enmy[%d] Spawnou\n",enem);
	}
	//printf("Inimigo %d: posicao x: %d, posicao y: %d, spawn: %d\n",enem,enmy[enem].posi.relx,enmy[enem].posi.rely,enmy[enem].spawned);
}
bool game(int num, int speed, int chaox, int chaoy,int parx, int pary, char* chao, char* parede){
	done = false;
	player.lanterna.alcance = 450;
	player.lanterna.angle = 30;
	player.pos.angle = 270;
	player.tamanho = 32;
	enmy = NULL;
	enmy = (enemy*)realloc(enmy,num*sizeof(enemy));
	printf("Jogar\n");
	for (index = 0;index < num; index++){
    	    enmy[index].tamanho = 32;
    	    enmy[index].spawned = 0;
    	    enmy[index].speed = speed;
	}
	for(index = 0; index < 2; index++){
		battery[index].spawned = 0;
		battery[index].tamanho = 5;
	}
	key.tamanho = 5;
	key.spawned = false;
    bac.chao.larg = chaox;//2046
    bac.chao.alt = chaoy;//1600
    bac.parede.alt = pary;//256
    bac.parede.larg = parx;//2048
    bac.chao.imagem = chao;
    bac.parede.imagem = parede;
	player.pos.x = (int)(res[0]/2);
	player.pos.y = (int)(res[1]/2);	
    player.key = false;
    bac.pos.x = player.pos.x-((int)bac.chao.larg/2);
    bac.pos.y = player.pos.y-((int)bac.chao.alt/2);
    player.pos.relx = player.pos.x - bac.pos.x;
    player.pos.rely = player.pos.y - bac.pos.y;
	printf("Primeira cutscene...\n");
	while(!done){
		for(pg = 1; pg<=3;pg++){
			setactivepage(pg);
			cleardevice();
			//Spawn stuff=======================================================================================
			if(player.key == false){
				for(index = 0; index <num ; index ++){
	    			 if(enmy[index].spawned == 0){
	    			 	spawnenemies(index);
					 }	
				}
			}
			for(index = 0; index <2 ; index ++){
				chance=rand()%100;
				if(battery[index].spawned == 0 && chance == 0){
					battery[index].spawned = 1;
					battery[index].relx = rand()%bac.chao.larg;
					battery[index].rely = rand()%bac.chao.alt;	
					battery[index].x = bac.pos.x+battery[index].relx;
	    			battery[index].y = bac.pos.y+battery[index].rely;
				}
			}
			if(player.score >= 30 && key.spawned == false && player.key == false){
				key.spawned = true;
				player.score = 0;
				key.relx = rand()%bac.chao.larg;
			 	key.rely = rand()%bac.chao.alt;
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
	    				enmy[index].spawned = false;
					}
			}
	    	//Move stuff=======================================================================================
	    	player.pos.relx = player.pos.x - bac.pos.x;
	    	player.pos.rely = player.pos.y - bac.pos.y;
	    	player.moving = false;
	    	for(index = 0;index < num; index++){
	    		enmy[index].posi.x = player.pos.x+(int)(enmy[index].posi.relx-player.pos.relx);
	    		enmy[index].posi.y = player.pos.y+(int)(enmy[index].posi.rely-player.pos.rely);
			}
	    	
	  		if( (GetAsyncKeyState('W') & 0x8000) && player.pos.rely-player.tamanho > 0){
	        	bac.pos.y+=5;
	        	player.moving = true;
			}
			if((GetAsyncKeyState('A') & 0x8000) && player.pos.relx-player.tamanho > 0){
	       		bac.pos.x+=5;
	       		player.moving = true;
			}
			if((GetAsyncKeyState('S') & 0x8000) && player.pos.rely+player.tamanho < bac.chao.alt){
	    		bac.pos.y-=5;
	    		player.moving = true;
			}
			if((GetAsyncKeyState('D') & 0x8000) && player.pos.relx+player.tamanho < bac.chao.larg){
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
				player.pos.angle = 180;
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
				if(enmy[index].spawned == 1){
					chance = rand()%5;
					if (enmy[index].posi.relx-enmy[index].tamanho < 0 || enmy[index].posi.rely-enmy[index].tamanho < 0 || enmy[index].posi.relx+enmy[index].tamanho > bac.chao.larg || enmy[index].posi.rely+enmy[index].tamanho > bac.chao.alt){
						enmy[index].posi.walkingangle += 180;
					}
					else if (cicles%60 == 0 && chance == 0){
						enmy[index].posi.walkingangle = rand()%360;
					}
					enmy[index].posi.relx+=cos(enmy[index].posi.walkingangle*conv)*(int)enmy[index].speed;
			       	enmy[index].posi.rely+=sin(enmy[index].posi.walkingangle*conv)*(int)enmy[index].speed;
			       	if(enmy[index].posi.angle > 69 && enmy[index].posi.angle <= 112){
						enmy[index].posi.direction = 4;
					}
					else if(enmy[index].posi.angle > 112 && enmy[index].posi.angle <= 157){
						enmy[index].posi.direction = 5;
					}
					else if(enmy[index].posi.angle > 157 && enmy[index].posi.angle <= 202){
						enmy[index].posi.direction = 6;
					}
					else if(enmy[index].posi.angle > 202 && enmy[index].posi.angle <= 247){
						enmy[index].posi.direction = 7;
					}
					else if(enmy[index].posi.angle > 247 && enmy[index].posi.angle <= 292){
						enmy[index].posi.direction = 0;
					}
					else if(enmy[index].posi.angle > 292 && enmy[index].posi.angle <= 337){
						enmy[index].posi.direction = 1;
					}			
					else if((enmy[index].posi.angle > 337 && enmy[index].posi.angle <= 360) || (enmy[index].posi.angle > 0 && enmy[index].posi.angle <= 23)){
						enmy[index].posi.direction = 2;
					}
					else if(enmy[index].posi.angle > 23 && enmy[index].posi.angle <= 68){
						enmy[index].posi.direction = 3;
					}
		       	//enmy[index].speed+=1;
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
		    	enmy[index].posi.angle = (int)(atan((float)(enmy[index].posi.y-player.pos.y)/(float)(enmy[index].posi.x-player.pos.x))*(180/3.14159265359));
				if(enmy[index].posi.angle < 0){
					enmy[index].posi.angle+=180;
				}	
				if(enmy[index].posi.y<player.pos.y){
					enmy[index].posi.angle+=180;
				}
				enmy[index].posi.hitangle = enmy[index].posi.angle-player.pos.angle;
				hitbox(enmy[index].posi.x,enmy[index].posi.y,enmy[index].posi.hitangle,player.pos.x,player.pos.y);
				enmy[index].posi.hit[0] = hitx;
				enmy[index].posi.hit[1] = hity;
				//player.pos.hit[0] = player.pos.x+player.lanterna.alcance;
				//player.pos.hit[1] = player.pos.y;
				enmy[index].color = {0,255,0};
				if(enmy[index].posi.hit[0]>player.pos.x && (enmy[index].posi.hitangle < 90 || enmy[index].posi.hitangle > 270) && enmy[index].spawned == 1){
		   			ca = (enmy[index].posi.hit[0]-player.pos.x);
		   	    	co = ca*tan((player.lanterna.angle/2)*conv);
		   	    	if (abs(enmy[index].posi.hit[1]-player.pos.y)<co && sqrt(((enmy[index].posi.x-player.pos.x)*(enmy[index].posi.x-player.pos.x))+((enmy[index].posi.y-player.pos.y)*(enmy[index].posi.y-player.pos.y)))<=player.lanterna.alcance){
		   				enmy[index].color = {255,0,0};
		   				enmy[index].health -= (int)((abs(100/co))+(abs(100/ca)));
		   			}
		        }
		        if(enmy[index].health < 0){
		        	enmy[index].spawned = false;
		        	enmy[index].health = NULL;
		        	player.score+=1;
		        	printf("Score: %d\n",player.score);
		        	
				}
				if((enmy[index].posi.x+enmy[index].tamanho>=player.pos.x-player.tamanho && enmy[index].posi.x-enmy[index].tamanho<=player.pos.x+player.tamanho)&& enmy[index].spawned){
					if(enmy[index].posi.y+enmy[index].tamanho>=player.pos.y-player.tamanho && enmy[index].posi.y-enmy[index].tamanho<=player.pos.y+player.tamanho){
						if(player.lanterna.alcance > 0){
								player.lanterna.alcance-=50;
						}
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
	        readimagefile(bac.chao.imagem,bac.pos.x,bac.pos.y,bac.chao.larg+bac.pos.x,bac.chao.alt+bac.pos.y);
	        setcolor(RGB(255,255,0));
	        setfillstyle(1,RGB(255,255,0));
	        screenflashlight(player.pos.angle+15,player.pos.x,player.pos.y,player.lanterna.alcance,player.lanterna.angle);
	        flash={player.pos.x,player.pos.y,xpos,ypos,posx,posy,player.pos.x,player.pos.y};
	        fillpoly(4,flash);
	  		for(index = 0;index < num; index++){
		        if(enmy[index].posi.x > - enmy[index].tamanho && enmy[index].posi.x < res[0]+enmy[index].tamanho && enmy[index].posi.y > -enmy[index].tamanho && enmy[index].posi.y < res[1]+enmy[index].tamanho && enmy[index].spawned == 1){
		        	readimagefile(enmyatlas[enmy[index].posi.direction][cicles%5],enmy[index].posi.x-enmy[index].tamanho,enmy[index].posi.y-enmy[index].tamanho,enmy[index].posi.x+enmy[index].tamanho,enmy[index].posi.y+enmy[index].tamanho);
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
				readimagefile(player.atlas[player.pos.direction][cicles%5],player.pos.x-(player.tamanho),player.pos.y-(player.tamanho),player.pos.x+(player.tamanho),player.pos.y+(player.tamanho));
			}
			else{
				readimagefile(player.atlas[player.pos.direction][0],player.pos.x-(player.tamanho),player.pos.y-(player.tamanho),player.pos.x+(player.tamanho),player.pos.y+(player.tamanho));
			}
			readimagefile(bac.parede.imagem,bac.pos.x,bac.pos.y-bac.parede.alt,bac.parede.larg+bac.pos.x,bac.pos.y);
	        //Win/Lose Stuff=========================================================================================
	        if(player.lanterna.alcance <= 0){
				done = 1;
				cleardevice();
				setvisualpage(pg);
				printf("GAME OVER!\n");
				done = true;
				result = false;
				break;
			}
			else if(player.key == true && player.pos.relx >= (bac.chao.larg/2)-15 && player.pos.relx <= (bac.chao.larg/2)+15 && player.pos.rely <= player.tamanho){
				cleardevice();
				setvisualpage(pg);
				printf("YOU WIN!\n");
				done = true;
				result = true;
				break;
			}
	        setvisualpage(pg);
	        cicles++;
	        if(cicles >= 65534){
	        	cicles = 0;
			}
		}
	}
	return result;
}
//Actual code stuff========================================================
int main(){
	srand((unsigned)time(NULL));
	bool playing = true;
	//Define stuff===========================================================================================
	initwindow(res[0],res[1]);
	player.atlas[0] = {"Images/Agatha/w1.bmp","Images/Agatha/w2.bmp","Images/Agatha/w3.bmp","Images/Agatha/w4.bmp","Images/Agatha/w5.bmp"};
	player.atlas[1] = {"Images/Agatha/wd1.bmp","Images/Agatha/wd2.bmp","Images/Agatha/wd3.bmp","Images/Agatha/wd4.bmp","Images/Agatha/wd5.bmp"};
	player.atlas[2] = {"Images/Agatha/d1.bmp","Images/Agatha/d2.bmp","Images/Agatha/d3.bmp","Images/Agatha/d4.bmp","Images/Agatha/d5.bmp"};
	player.atlas[3] = {"Images/Agatha/sd1.bmp","Images/Agatha/sd2.bmp","Images/Agatha/sd3.bmp","Images/Agatha/sd4.bmp","Images/Agatha/sd5.bmp"};
	player.atlas[4] = {"Images/Agatha/s1.bmp","Images/Agatha/s2.bmp","Images/Agatha/s3.bmp","Images/Agatha/s4.bmp","Images/Agatha/s5.bmp"};
	player.atlas[5] = {"Images/Agatha/sa1.bmp","Images/Agatha/sa2.bmp","Images/Agatha/sa3.bmp","Images/Agatha/sa4.bmp","Images/Agatha/sa5.bmp"};
	player.atlas[6] = {"Images/Agatha/a1.bmp","Images/Agatha/a2.bmp","Images/Agatha/a3.bmp","Images/Agatha/a4.bmp","Images/Agatha/a5.bmp"};
	player.atlas[7] = {"Images/Agatha/wa1.bmp","Images/Agatha/wa2.bmp","Images/Agatha/wa3.bmp","Images/Agatha/wa4.bmp","Images/Agatha/wa5.bmp"};
	enmyatlas[0] = {"Images/Fantasma/w1.bmp","Images/Fantasma/w2.bmp","Images/Fantasma/w3.bmp","Images/Fantasma/w4.bmp","Images/Fantasma/w5.bmp"};
	enmyatlas[1] = {"Images/Fantasma/wd1.bmp","Images/Fantasma/wd2.bmp","Images/Fantasma/wd3.bmp","Images/Fantasma/wd4.bmp","Images/Fantasma/wd5.bmp"};
	enmyatlas[2] = {"Images/Fantasma/d1.bmp","Images/Fantasma/d2.bmp","Images/Fantasma/d3.bmp","Images/Fantasma/d4.bmp","Images/Fantasma/d5.bmp"};
	enmyatlas[3] = {"Images/Fantasma/sd1.bmp","Images/Fantasma/sd2.bmp","Images/Fantasma/sd3.bmp","Images/Fantasma/sd4.bmp","Images/Fantasma/sd5.bmp"};
	enmyatlas[4] = {"Images/Fantasma/s1.bmp","Images/Fantasma/s2.bmp","Images/Fantasma/s3.bmp","Images/Fantasma/s4.bmp","Images/Fantasma/s5.bmp"};
	enmyatlas[5] = {"Images/Fantasma/sa1.bmp","Images/Fantasma/sa2.bmp","Images/Fantasma/sa3.bmp","Images/Fantasma/sa4.bmp","Images/Fantasma/sa5.bmp"};
	enmyatlas[6] = {"Images/Fantasma/a1.bmp","Images/Fantasma/a2.bmp","Images/Fantasma/a3.bmp","Images/Fantasma/a4.bmp","Images/Fantasma/a5.bmp"};
	enmyatlas[7] = {"Images/Fantasma/wa1.bmp","Images/Fantasma/wa2.bmp","Images/Fantasma/wa3.bmp","Images/Fantasma/wa4.bmp","Images/Fantasma/wa5.bmp"};
	//Play stuff===========================================================================================
	cleardevice();
	fase = 0;
    while(playing){
    	/*printf("Digite a fase desejada: ");
    	scanf("%d",&fase);*/
    	switch (fase){
	    	case 0:
	    		printf("Menu\n");
	    		delay(1000);
	    		printf("Precione a qualquer tecla para jogar...\n");
	    		getch();
	    		fase = 1;
	    		break;
	    	case 1:
	    		setactivepage(1);
	    		readimagefile("Images/Cutscenes/cut1.jpg",0,0,res[0],res[1]);
	    		setvisualpage(1);
	    		getch();
	    		fase = 5;
	    		break;
	    	case 2:
	    		printf("Check\n");
	    		cicles = 0;
				//int bg[10] = {0,0,1000,0,1000,1000,0,1000,0,0};
	    		game(10, 7, 2048, 1600, 2048, 256, "Images/Background/dungeon_chao.bmp", "Images/Background/dungeon_parede.bmp");	
				enmy = NULL;
				if (result == true){
					fase = 3;
				}
				else if (result == false){
					fase = 8;
					playing = false;
					break;
				}
				//free(enmy);	
			case 3:
				printf("Segunda Cutscene...\n");
				delay(5000);
				fase=4;
			case 4:
				cicles = 0;
				//int bg[10] = {0,0,1000,0,1000,1000,0,1000,0,0};
	    		game(20, 10, 2048, 1600, 2048, 256, "Images/Background/biblioteca_chao.bmp", "Images/Background/biblioteca_parede.bmp");	
				enmy = NULL;
				if (result == true){
					fase = 5;
				}
				else if(result == false){
					fase = 8;
					playing = false;
					break;
					
				}
			case 5:
				fase=6;
				break;
			case 6: //Chefão
			{
				char fireball[4][30]={"Images/Boss/Fireball/1.bmp","Images/Boss/Fireball/2.bmp","Images/Boss/Fireball/3.bmp","Images/Boss/Fireball/4.bmp"};
				char bossatlas[8][30]={"Images/Boss/w.bmp","Images/Boss/wd.bmp","Images/Boss/d.bmp","Images/Boss/sd.bmp","Images/Boss/s.bmp","Images/Boss/sa.bmp","Images/Boss/a.bmp","Images/Boss/wa.bmp"};
				printf("Check\n");
				int limite,hi;
				for(index = 0; index < 2; index++){
					battery[index].spawned = 0;
					battery[index].tamanho = 5;
				}

			    bac.chao.larg = res[1];
			    bac.chao.alt = res[1];
			    bac.chao.imagem = "Images/background/hall.bmp";
			    limite = res[1]-(320*bac.chao.alt/2048);
			    player.pos.x = (int)(res[0]/2);// Mudar
			   	player.pos.y = bac.chao.alt-((bac.chao.alt-limite)/2);// Mudar
			    player.lanterna.alcance = 450;
			    player.lanterna.angle = 30;
			    player.pos.angle = 270;
			    player.tamanho = 32;
			    bac.pos.x = (int)(res[0]-res[1])/2;
			    bac.pos.y = 0;
			    boss.tamanho = 64;
			    boss.pos.x = res[0]/2;
			    boss.pos.y = res[1]/2;
			    boss.speed = 5;
			    boss.pos.walkingangle = rand()%360;
			    boss.health = 5000;
				num = 5;
			    enmy = NULL;
			    enmy = (enemy*)realloc(enmy,num*sizeof(enemy));
				for(index = 0; index < num; index++){
					enmy[index].spawned = false;
					enmy[index].tamanho = 32;
				}
			    cicles = 0;
			    while(fase == 6){
			    	for(pg = 1 ; pg <= 2 ; pg ++){
			    		setactivepage(pg);
			    		cleardevice();
			    		//Spawn Stuff======================================================================================
			    		for(index = 0 ; index < num; index++){
			    			chance = rand()%1000;
			    			if(chance == 0 && enmy[index].spawned == false){
			    				enmy[index].posi.x = boss.pos.x + (boss.tamanho/2);
			    				enmy[index].posi.y = boss.pos.y + boss.tamanho;
			    				enmy[index].speed = rand()%7;
			    				enmy[index].speed+=2;
			    				ca = player.pos.x-enmy[index].posi.x;
			    				co = player.pos.y-enmy[index].posi.y;
			    				hi = (int)sqrt((co*co)+(ca*ca));
			    				enmy[index].posi.walkingangle = (int)(acos(ca/hi)*(180/3.14159265359));
			    				if(enmy[index].posi.walkingangle<0){
			    					enmy[index].posi.walkingangle+=180;
								}
			    				//enmy[index].posi.walkingangle+=180;
								
								enmy[index].spawned = true;
							}
						}
						for(index = 0; index <2 ; index ++){
							chance=rand()%100;
							if(battery[index].spawned == 0 && chance == 0){
								battery[index].spawned = 1;
								battery[index].x = 0;
								battery[index].y = 0;
								while(battery[index].x < bac.pos.x){
									battery[index].x = rand()%bac.chao.larg;
								}
								
								while(battery[index].y <= limite){
									printf("aqui\n");
									battery[index].y = rand()%bac.chao.alt;	
								}
							}
						}
			    		//Move Stuff=====================================================================================
			    		player.moving = false;
			    		if(kbhit()){
				    		if( (GetAsyncKeyState('W') & 0x8000) && player.pos.y-player.tamanho > limite){
					        	player.pos.y-=5;
					        	player.moving = true;
							}
							if((GetAsyncKeyState('A') & 0x8000) && player.pos.x-player.tamanho > bac.pos.x){
					       		player.pos.x-=5;
					       		player.moving = true;
							}
				 			if((GetAsyncKeyState('S') & 0x8000) && player.pos.y+player.tamanho < bac.chao.alt){
				        		player.pos.y+=5;
				        		player.moving = true;
							}
							if((GetAsyncKeyState('D') & 0x8000) && player.pos.x+player.tamanho < bac.chao.larg+bac.pos.x){
				        		player.pos.x+=5;
				        		player.moving = true;
							}
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
							player.pos.angle = 180;
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
						if (boss.pos.x-boss.tamanho < bac.pos.x ){
							boss.pos.walkingangle = 1;
						}
						else if(boss.pos.x+boss.tamanho > bac.pos.x+res[1] ){
							boss.pos.walkingangle = 181;
						}
						if(boss.pos.y-boss.tamanho <128){
							if(boss.pos.walkingangle >= 270){
								boss.pos.walkingangle = 45;
							}
							else if (boss.pos.walkingangle >= 180 && boss.pos.walkingangle < 270){
								boss.pos.walkingangle = 135;
							}
						} 
						if(boss.pos.y > limite){
							if(boss.pos.walkingangle <= 90){
								boss.pos.walkingangle = 315;
							}
							else if (boss.pos.walkingangle > 90  && boss.pos.walkingangle < 180){
								boss.pos.walkingangle = 225;
							}
						}
						chance = rand()%3;
						if (cicles%60 == 0 && chance == 0){
							boss.pos.walkingangle = rand()%360;
						}
						boss.pos.x+=cos(boss.pos.walkingangle*conv)*(int)boss.speed;
				       	boss.pos.y+=sin(boss.pos.walkingangle*conv)*(int)boss.speed;
				       	if(boss.pos.walkingangle > 69 && boss.pos.walkingangle <= 112){
							boss.pos.direction = 4;
						}
						else if(boss.pos.walkingangle > 112 && boss.pos.walkingangle <= 157){
							boss.pos.direction = 5;
						}
						else if(boss.pos.walkingangle > 157 && boss.pos.walkingangle <= 202){
							boss.pos.direction = 6;
						}
						else if(boss.pos.walkingangle > 202 && boss.pos.walkingangle <= 247){
							boss.pos.direction = 7;
						}
						else if(boss.pos.walkingangle > 247 && boss.pos.walkingangle <= 292){
							boss.pos.direction = 0;
						}
						else if(boss.pos.walkingangle > 292 && boss.pos.walkingangle <= 337){
							boss.pos.direction = 1;
						}			
						else if((boss.pos.walkingangle > 337 && boss.pos.walkingangle <= 360) || (boss.pos.walkingangle > 0 && boss.pos.walkingangle <= 23)){
							boss.pos.direction = 2;
						}
						else if(boss.pos.walkingangle > 23 && boss.pos.walkingangle <= 68){
							boss.pos.direction = 3;
						}
				       	for(index = 0; index < num;index++){
				       		if(enmy[index].spawned == true){
				       			enmy[index].posi.x+=cos(enmy[index].posi.walkingangle*conv)*enmy[index].speed;	
				       			enmy[index].posi.y+=sin(enmy[index].posi.walkingangle*conv)*enmy[index].speed;
							}
						}
						//Hit Stuff=====================================================================================
				    	boss.pos.angle = (int)(atan((float)(boss.pos.y-player.pos.y)/(float)(boss.pos.x-player.pos.x))*(180/3.14159265359));
						if(boss.pos.angle < 0){
							boss.pos.angle+=180;
						}	
						if(boss.pos.y<player.pos.y){
							boss.pos.angle+=180;
						}
						boss.pos.hitangle = boss.pos.angle-player.pos.angle;
						hitbox(boss.pos.x,boss.pos.y,boss.pos.hitangle,player.pos.x,player.pos.y);
						boss.pos.hit[0] = hitx;
						boss.pos.hit[1] = hity;
						boss.color = {0,255,0};
						if(boss.pos.hit[0]>player.pos.x && (boss.pos.hitangle < 90 || boss.pos.hitangle > 270)){
				   			ca = (boss.pos.hit[0]-player.pos.x);
				   	    	co = ca*tan((player.lanterna.angle/2)*conv);
				   	    	if ((abs(boss.pos.hit[1]-player.pos.y)<co+boss.tamanho || abs(boss.pos.hit[1]-player.pos.y)<co-boss.tamanho) && sqrt(((boss.pos.x-player.pos.x)*(boss.pos.x-player.pos.x))+((boss.pos.y-player.pos.y)*(boss.pos.y-player.pos.y)))<=player.lanterna.alcance+boss.tamanho){
				   				boss.color = {255,0,0};
				   				boss.health -= (int)((abs(1000/co))+(abs(1000/ca)));
				   				printf("Vida do Boss: %d\n",boss.health);
				   			}
				        }
				        if(player.pos.x-player.tamanho<=boss.pos.x+boss.tamanho && player.pos.x+player.tamanho >= boss.pos.x-boss.tamanho){
				        	if(player.pos.y-player.tamanho<=boss.pos.y+boss.tamanho && player.pos.y+player.tamanho >= boss.pos.y-boss.tamanho){
				        		player.lanterna.alcance-=5;
							}
						}
						for (index = 0;index < num;index++){
							if(enmy[index].spawned){
								if(enmy[index].posi.y >= res[1]){
									enmy[index].spawned = false;
								}
								if(enmy[index].posi.x+enmy[index].tamanho>=player.pos.x-player.tamanho && enmy[index].posi.x-enmy[index].tamanho<=player.pos.x+player.tamanho ){
									if(enmy[index].posi.y+enmy[index].tamanho>=player.pos.y-player.tamanho && enmy[index].posi.y-enmy[index].tamanho<=player.pos.y+player.tamanho){
										enmy[index].spawned = false;
										player.lanterna.alcance-=10;
									}
								}
							}	
						}
						for(index=0;index<2;index++){
							if(battery[index].spawned == true && battery[index].x >= player.pos.x-player.tamanho && battery[index].x <= player.pos.x+player.tamanho && battery[index].y >= player.pos.y-player.tamanho && battery[index].y <= player.pos.y+player.tamanho){
								battery[index].spawned = false;
								battery[index].x = 0;
								battery[index].y = 0;	
				    			player.lanterna.alcance+=50;
				    			if(player.lanterna.alcance > 600){
				    				player.lanterna.alcance = 600;
								}
				    			printf("Spawn bateria %d: %d\n",index,battery[index].spawned);
							}
						}
						
			    		//Drawstuff=====================================================================================
			    		readimagefile(bac.chao.imagem,bac.pos.x,bac.pos.y,bac.chao.larg+bac.pos.x,bac.chao.alt+bac.pos.y);
			    		setcolor(RGB(255,255,0));
				        setfillstyle(1,RGB(255,255,0));
				        screenflashlight(player.pos.angle+15,player.pos.x,player.pos.y,player.lanterna.alcance,player.lanterna.angle);
				        flash={player.pos.x,player.pos.y,xpos,ypos,posx,posy,player.pos.x,player.pos.y};
				        fillpoly(4,flash);
				        if(player.moving){
				        	readimagefile(player.atlas[player.pos.direction][cicles%5],player.pos.x-player.tamanho,player.pos.y-player.tamanho,player.pos.x+player.tamanho,player.pos.y+player.tamanho);
						}
			    		else{
			    			readimagefile(player.atlas[player.pos.direction][0],player.pos.x-player.tamanho,player.pos.y-player.tamanho,player.pos.x+player.tamanho,player.pos.y+player.tamanho);
						}
			    		setfillstyle(1,RGB(boss.color[0],boss.color[1],boss.color[2]));
			    		readimagefile(bossatlas[boss.pos.direction],boss.pos.x-boss.tamanho,boss.pos.y-boss.tamanho,boss.pos.x+boss.tamanho,boss.pos.y+boss.tamanho);
			    		setfillstyle(1,RGB(0,0,255));
			    		for(index = 0; index < num; index++){
			    			if(enmy[index].spawned == true){
			    				readimagefile(fireball[cicles%4],enmy[index].posi.x-enmy[index].tamanho,enmy[index].posi.y-enmy[index].tamanho,enmy[index].posi.x+enmy[index].tamanho,enmy[index].posi.y+enmy[index].tamanho);
							}
						}
						setfillstyle(1,RGB(255,0,255));
						for(index = 0; index < 2; index++){
							if(battery[index].spawned){
								fillellipse(battery[index].x,battery[index].y,battery[index].tamanho,battery[index].tamanho);
							}
						}
						//Win/lose====================================================================================================
						if(boss.health <= 0){
							fase = 7;
							break;
						}
						else if (player.lanterna.alcance <= 0){
							fase = 8;
							playing = false;
							break;
						}
			    		setvisualpage(pg);
			    		cicles++;
			    		//delay(50);
			    		if(cicles >= 65534){
				        	cicles = 0;
						}
					}
				}
				enmy = NULL;
				free(enmy);
				delete enmy;
				free(enmyatlas);
				delete enmyatlas;
				break;
				}
			case 7:
				printf("Ultima cutscene...\n");
				delay(1000);
				playing = false;
				cleardevice();
				setvisualpage(pg);
				printf("YOU WIN!\n");
				break;
			case 8:
				delay(1000);
				playing = false;
				cleardevice();
				setvisualpage(pg);
				pg = 3;
				printf("GAME OVER\n");
				break;
		}	
		if(fase == 8){
			break;
		}
	}
	cleardevice();
	printf("\n");
	system("pause");
}
