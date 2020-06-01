#include <stdlib.h> 	//
#include <stdio.h>		//
#include <math.h>		//Bibliotecas utilizadas
#include <graphics.h>	//
#include <time.h>		//
static double conv = (3.14159265359/180); 	//Fator de conversão de graus para radianos
int res[2] = {getmaxwidth(),getmaxheight()}; 					//Tamanho da tela em pixeis
//Structfy stuff=======================================================
struct player{ 										//Struct para organizar as variaveis do jogador
	int tamanho,health,score;							//Variaveis para guardar o tamanho da imagem, a vida e a pontuação do jogador respectivamente
	bool key,moving,loss;									//Variaveis para saber se o jogador já pegou a chave da fase e para verificar se o jogador está andando respectivamente
	void *atlas[8][5];	
	void *mask[8][5];							//Array de três indices para guardar as "Strings" dos nomes das imagens que compoem a animação de caminhada do jogador
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
		int tamanho,speed;								//Variaveis para guardar o tamanho da imagem e a vida do inimigo
		float health;									//Variavel para guardar a velocidade que o inimigo anda
		bool spawned;
		struct pos{										//Struct para organizar as variaveis de posição do inimigo
			int x,y,angle,hitangle,walkingangle,direction;		//Variaveis para guardar as coordenadas x,y na tela, o angulo em relacão ao eixo x, o anglo formado entre a posição do inimigo e a lanterna e a direção que o inimigo está andando
			int hit[2];									//Array para guardar as coordenadas x,y da hitbox do inimigo em relação a lanterna do jogador
			float relx,rely;							//Variaveis para guardar as coordenadas x,y em relação ao canto superior esquerdo do mapa
		};
	pos posi;											//Criar o objeto posi que referencia a struct posi
};
struct boss{
	int tamanho,health,aggro;
	float speed;
	void *img[8][4];
	void *mask[8][4];	
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
	void *img;
	void *mask;
};
//Define Global Stuff===================================================================
int num,ca,pg,index;
int xpos = 0;
int ypos = 0;
int posx = 0;
int posy = 0;
int hitx = 0;
int hity = 0;
double co;
unsigned long long int cicles = 0;
void *enmyatlas[8][5];
void *enmymask[8][5];
void *btrimg;
void *btrmask;
void *fbimg[5];
void *fbmask[5];
void *back[5];
int flash[8];
int chance;
int fase = 0;
bool done,result;
clock_t start;
player player;
enemy *enmy;
battery battery[2];
key key;
background bac;
boss boss;
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
			enmy[enem].posi.relx+=2*enmy[enem].tamanho;											//Corrige a posição
		}
		enmy[enem].posi.rely = rand();
		enmy[enem].posi.rely = ((int)enmy[enem].posi.rely)%bac.chao.alt-enmy[enem].tamanho;
		if(enmy[enem].posi.rely < enmy[enem].tamanho){
			enmy[enem].posi.rely+=2*enmy[enem].tamanho;
		}
		enmy[enem].posi.walkingangle = rand()%360;
    	enmy[enem].spawned = 1;
    	enmy[enem].health = 50;
    	//printf("enmy[%d] Spawnou\n",enem);
	}
	//printf("Inimigo %d: posicao x: %d, posicao y: %d, spawn: %d\n",enem,enmy[enem].posi.relx,enmy[enem].posi.rely,enmy[enem].spawned);
}
void mask(int left, int top, int right, int bottom){
	int points[10];
	setfillstyle(1,RGB(0,0,0));
	setcolor(RGB(0,0,0));
	points = {0,0,0,res[1],left,res[1],left,0,0,0};
	fillpoly(5,points);
	points = {0,0,0,top,res[0],top,res[0],0,0,0};
	fillpoly(5,points);
	points = {0,bottom,0,res[1],res[0],res[1],res[0],bottom,0,bottom};
	fillpoly(5,points);
	points = {right,0,right,res[1],res[0],res[1],res[0],0,0,0};
	fillpoly(5,points);
}
void load_img(){
	initwindow(2048,2048);
	int i,j;
	int tam = imagesize(0,0,player.tamanho*2,player.tamanho*2);
	setvisualpage(1);
	setactivepage(2);
	readimagefile("Images/Agatha/bitmap.bmp",0,0,player.tamanho*10,player.tamanho*16);
	for(i=0;i<8;i++){
		for(j=0;j<5;j++){
			player.atlas[i][j] = malloc(tam);
			getimage(j*player.tamanho*2,i*player.tamanho*2,(j+1)*player.tamanho*2,(i+1)*player.tamanho*2,player.atlas[i][j]);
		}
	}
	cleardevice();
	readimagefile("Images/Agatha/bitmap_mask.bmp",0,0,player.tamanho*10,player.tamanho*16);
	for(i=0;i<8;i++){
		for(j=0;j<5;j++){
			player.mask[i][j] = malloc(tam);
			getimage(j*player.tamanho*2,i*player.tamanho*2,(j+1)*player.tamanho*2,(i+1)*player.tamanho*2,player.mask[i][j]);
		}
	}
	cleardevice();
	tam = imagesize(0,0,96,96);
	readimagefile("Images/Fantasma/bitmap.bmp",0,0,480,768);
	for(i=0;i<8;i++){
		for(j=0;j<5;j++){
			enmyatlas[i][j] = malloc(tam);
			getimage(j*96,i*96,(j+1)*96,(i+1)*96,enmyatlas[i][j]);
			
		}
	}
	cleardevice();
	readimagefile("Images/Fantasma/bitmap_mask.bmp",0,0,480,768);
	for(i=0;i<8;i++){
		for(j=0;j<5;j++){
			enmymask[i][j] = malloc(tam);
			getimage(j*96,i*96,(j+1)*96,(i+1)*96,enmymask[i][j]);
		}
	}
	cleardevice();
	tam = imagesize(0,0,96,96);
	readimagefile("Images/Objetos/Bateria.bmp",0,0,48,48);
	btrimg= malloc(tam);
	getimage(0,0,48,48,btrimg);
	cleardevice();
	readimagefile("Images/Objetos/Bateria_mask.bmp",0,0,48,48);
	btrmask= malloc(tam);
	getimage(0,0,48,48,btrmask);
	cleardevice();
	tam = imagesize(0,0,key.tamanho,key.tamanho);
	readimagefile("Images/Objetos/Chave.bmp",0,0,key.tamanho,key.tamanho);
	key.img= malloc(tam);
	getimage(0,0,key.tamanho,key.tamanho,key.img);
	cleardevice();
	readimagefile("Images/Objetos/Chave_mask.bmp",0,0,key.tamanho,key.tamanho);
	key.mask= malloc(tam);
	getimage(0,0,key.tamanho,key.tamanho,key.mask);
	cleardevice();
	tam = imagesize(0,0,boss.tamanho*2,boss.tamanho*2);
	readimagefile("Images/Boss/bitmap.bmp",0,0,512,1024);
	for(i=0;i<8;i++){
		for(j=0;j<4;j++){
			boss.img[i][j] = malloc(tam);
			getimage(j*boss.tamanho*2,i*boss.tamanho*2,(j+1)*boss.tamanho*2,(i+1)*boss.tamanho*2,boss.img[i][j]);
		}
	}
	cleardevice();
	readimagefile("Images/Boss/bitmap_mask.bmp",0,0,512,1024);
	for(i=0;i<8;i++){
		for(j=0;j<4;j++){
			boss.mask[i][j] = malloc(tam);
			getimage(j*boss.tamanho*2,i*boss.tamanho*2,(j+1)*boss.tamanho*2,(i+1)*boss.tamanho*2,boss.mask[i][j]);
		}
	}
	cleardevice();
	tam = imagesize(0,0,64,64);
	readimagefile("Images/Boss/Fireball/bitmap.bmp",0,0,320,64);
	for(i=0;i<5;i++){
		fbimg[i] = malloc(tam);
		getimage(i*64,0,(i+1)*64,64,fbimg[i]);
	}
	cleardevice();
	readimagefile("Images/Boss/Fireball/bitmap_mask.bmp",0,0,320,64);
	for(i=0;i<5;i++){
		fbmask[i] = malloc(tam);
		getimage(i*64,0,(i+1)*64,64,fbmask[i]);
	}
	cleardevice();
	tam = imagesize(0,0,2048,256);
	readimagefile("Images/Background/dungeon_parede.bmp",0,0,2048,256);
	back[0] = malloc(tam);
	getimage(0,0,2048,256,back[0]);
	cleardevice();
	tam = imagesize(0,0,2048,800);
	readimagefile("Images/Background/dungeon_chao.bmp",0,0,2048,800);
	back[1] = malloc(tam);
	getimage(0,0,2048,800,back[1]);
	cleardevice();
	tam = imagesize(0,0,2048,256);
	readimagefile("Images/Background/biblioteca_parade.bmp",0,0,2048,256);
	back[2] = malloc(tam);
	getimage(0,0,2048,256,back[2]);
	cleardevice();
	tam = imagesize(0,0,2048,1600);
	readimagefile("Images/Background/biblioteca_chao.bmp",0,0,2048,1600);
	back[3] = malloc(tam);
	getimage(0,0,2048,1600,back[3]);
	cleardevice();
	tam = imagesize(0,0,res[1],res[1]);
	readimagefile("Images/Background/hall.bmp",0,0,res[1],res[1]);
	back[4] = malloc(tam);
	getimage(0,0,res[1],res[1],back[4]);
	closegraph();
}

bool game(int win, int num, int speed, int chaox, int chaoy,int parx, int pary, int level){
	done = false;
	player.lanterna.alcance = 450;
	player.pos.angle = 270;
	enmy = NULL;
	enmy = (enemy*)realloc(enmy,num*sizeof(enemy));
	printf("Jogar\n");
	for (index = 0;index < num; index++){
    	    enmy[index].spawned = 0;
    	    enmy[index].speed = speed;
    	    enmy[index].tamanho = 48;
	}
	for(index = 0; index < 2; index++){
		battery[index].spawned = 0;
		battery[index].tamanho = 48;
	}
	key.spawned = false;
    bac.chao.larg = chaox;//2046
    bac.chao.alt = chaoy;//1600
    bac.parede.alt = pary;//256
    bac.parede.larg = parx;//2048
	player.pos.x = (int)(res[0]/2);
	player.pos.y = (int)(res[1]/2);	
    player.key = false;
    bac.pos.x = player.pos.x-((int)bac.chao.larg/2);
    bac.pos.y = player.pos.y-((int)bac.chao.alt/2);
    player.pos.relx = player.pos.x - bac.pos.x;
    player.pos.rely = player.pos.y - bac.pos.y;
	printf("Primeira cutscene...\n");
	while(!done){
		for(pg = 1; pg<=2;pg++){
			start = clock();
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
			if(player.score >= win && key.spawned == false && player.key == false){
				key.spawned = true;
				player.score = 0;
				key.relx = rand()%bac.chao.larg;
			 	key.rely = rand()%bac.chao.alt;
				key.x = bac.pos.x+key.relx;
				key.y = bac.pos.y+key.rely;
			}	
			//Mechanics Stuff==================================================================================
			if(cicles%5== 0){
				if(player.lanterna.alcance>=0 && player.loss){
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
					chance = rand()%3;
					if (enmy[index].posi.relx-enmy[index].tamanho < 0 || enmy[index].posi.rely-enmy[index].tamanho < 0 || enmy[index].posi.relx+enmy[index].tamanho > bac.chao.larg || enmy[index].posi.rely+enmy[index].tamanho > bac.chao.alt){
						enmy[index].posi.walkingangle += 180;
					}
					else if (cicles%30 == 0 && chance == 0){
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
				if(enmy[index].posi.hit[0]>player.pos.x && (enmy[index].posi.hitangle < 90 || enmy[index].posi.hitangle > 270) && enmy[index].spawned == 1){
		   			ca = (enmy[index].posi.hit[0]-player.pos.x);
		   	    	co = ca*tan((player.lanterna.angle/2)*conv);
		   	    	if (abs(enmy[index].posi.hit[1]-player.pos.y)<co && sqrt(((enmy[index].posi.x-player.pos.x)*(enmy[index].posi.x-player.pos.x))+((enmy[index].posi.y-player.pos.y)*(enmy[index].posi.y-player.pos.y)))<=player.lanterna.alcance){
		   				enmy[index].health -= (int)((abs(100/co))+(abs(100/ca)));
		   			}
		        }
		        if(enmy[index].health < 0){
		        	enmy[index].spawned = false;
		        	enmy[index].health = NULL;
		        	player.score+=1;
		        	printf("Score: %d\n",player.score);
		        	
				}
				if((enmy[index].posi.x+enmy[index].tamanho>=player.pos.x-player.tamanho && enmy[index].posi.x-enmy[index].tamanho<=player.pos.x+player.tamanho)&& enmy[index].spawned && player.loss){
					if(enmy[index].posi.y+enmy[index].tamanho>=player.pos.y-player.tamanho && enmy[index].posi.y-enmy[index].tamanho<=player.pos.y+player.tamanho){
						if(player.lanterna.alcance > 0){
								player.lanterna.alcance-=15;
						}
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
			if(player.lanterna.alcance>500){
	    		player.lanterna.alcance=500;
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
	        putimage(bac.pos.x,bac.pos.y,back[level+1],0);
	        setcolor(RGB(255,255,0));
	        setfillstyle(1,RGB(255,255,0));
	        screenflashlight(player.pos.angle+15,player.pos.x,player.pos.y,player.lanterna.alcance,player.lanterna.angle);
	        flash={player.pos.x,player.pos.y,xpos,ypos,posx,posy,player.pos.x,player.pos.y};
	        fillpoly(4,flash);
	  		for(index = 0;index < num; index++){
		        if(enmy[index].posi.x > - enmy[index].tamanho && enmy[index].posi.x < res[0]+enmy[index].tamanho && enmy[index].posi.y > -enmy[index].tamanho && enmy[index].posi.y < res[1]+enmy[index].tamanho && enmy[index].spawned == 1){
					putimage(enmy[index].posi.x-enmy[index].tamanho,enmy[index].posi.y-enmy[index].tamanho,enmymask[enmy[index].posi.direction][cicles%5],AND_PUT);
					putimage(enmy[index].posi.x-enmy[index].tamanho,enmy[index].posi.y-enmy[index].tamanho,enmyatlas[enmy[index].posi.direction][cicles%5],OR_PUT);
				}
			}
			for(index=0;index < 2; index++){
				if(battery[index].spawned == true){
					putimage(battery[index].x-(battery[index].tamanho/2),battery[index].y-(battery[index].tamanho/2),btrmask,AND_PUT);
					putimage(battery[index].x-(battery[index].tamanho/2),battery[index].y-(battery[index].tamanho/2),btrimg,OR_PUT);
						
				}
			}
			if(key.spawned == true){
				putimage(key.x-(key.tamanho/2),key.y-(key.tamanho/2),key.mask,AND_PUT);
				putimage(key.x-(key.tamanho/2),key.y-(key.tamanho/2),key.img,OR_PUT);
			}
			if(player.moving){
				putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho*1.3),player.mask[player.pos.direction][cicles%5],AND_PUT);
				putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho*1.3),player.atlas[player.pos.direction][cicles%5],OR_PUT);
			}
			else{
				putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho*1.3),player.mask[player.pos.direction][0],AND_PUT);
				putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho*1.3),player.atlas[player.pos.direction][0],OR_PUT);
			}
			putimage(bac.pos.x,bac.pos.y-bac.parede.alt,back[level],0);
			mask(bac.pos.x,bac.pos.y-bac.parede.alt,bac.pos.x+bac.parede.larg,bac.pos.y+bac.chao.alt);
	        //Win/Lose Stuff=========================================================================================
	        if(player.lanterna.alcance <= 0){
				cleardevice();
				setvisualpage(pg);
				printf("GAME OVER!\n");
				done = true;
				result = false;
				break;
			}
			else if(player.key == true){
			 	if((num == 20 && player.pos.relx >= (bac.chao.larg/2)-15 && player.pos.relx <= (bac.chao.larg/2)+15 && player.pos.rely <= player.tamanho) || (num == 10 && player.pos.relx - player.tamanho <= 0)){
					cleardevice();
					setvisualpage(pg);
					printf("YOU WIN!\n");
					done = true;
					result = true;
					break;
				}
			}
	        setvisualpage(pg);
	        cicles++;
	        if(cicles >= 65534){
	        	cicles = 0;
			}
			while((double)(clock()-start)<34){
			}
		}
	}
}
//Actual code stuff========================================================
int main(){	
	//Define stuff===========================================================================================
	player.lanterna.angle = 30;
	player.pos.angle = 270;
	player.tamanho = 48;
	player.loss = true;
	key.tamanho = 64;
	boss.tamanho = 64;
	srand((unsigned)time(NULL));
	int gdriver = DETECT, gmode;
	initgraph(&gdriver, &gmode, "");
	load_img();
	initwindow(res[0],res[1],"Ghostly Mansion");
	//Play stuff===========================================================================================
	cleardevice();
	fase = 0;
	bool playing = true;
	int wn = 30;
	int k,l;
    while(playing){
    	printf("Estou aqui agora\n");
    	/*printf("Digite a fase desejada: ");
    	scanf("%d",&fase);*/
    	switch (fase){
	    	case 0:{
	    		printf("Menu\n");
	    		char menu[2][20]={"Images/menu.bmp","Images/menu_2.bmp"};
	    		int imagem = 0;
	    		while(imagem < 2){
					for(pg = 1;pg<=2;pg++){
						char cheat[10] = {};
						setactivepage(pg);
						cleardevice();
						if(imagem == 0){
							if (GetAsyncKeyState(VK_LBUTTON) && 0x8000){
								if(mousey()>=(650*res[1])/1080 && mousey()<=(770*res[1])/1080){
									if(mousex()>=(int)(190*res[0])/1920 && mousex()<=(int)(440*res[0])/1920){	
										fase = 1;
										imagem = 3000;
										break;
									}
								}
								else if(mousex()>=(int)(190*res[0])/1920 && mousex()<=(int)(720*res[0])/1920){	
									if(mousey()>=(800*res[1])/1080 && mousey()<=(890*res[1])/1080){
										imagem = 1;
									}
								}
							}
						}
						else if(imagem == 1){
							if (GetAsyncKeyState(VK_LBUTTON) && 0x8000){
								if(mousey()>=(970*res[1])/1080 && mousey()<=(1050*res[1])/1080){
									if(mousex()>=(int)(1650*res[0])/1920 && mousex()<=(int)(1890*res[0])/1920){	
										imagem = 0;
									}
								}
							}
						}
						readimagefile(menu[imagem],0,0,res[0],res[1]);
			    		setvisualpage(pg);
			    		if (GetAsyncKeyState('A') && GetAsyncKeyState('L') && 0x8000){
			    			printf("Digite a trapaca: ");
			    			scanf("%s",&cheat);
			    		}
			    		if(strcmp(cheat, "Ap0cryph4") == 0) {
			    			cleardevice();
			    			fase = 3;
			    			imagem = 3000;
			    			break;
						}
						else if (strcmp(cheat, "Sp3ctr3") == 0){
							cleardevice();
							fase = 5;
							imagem = 3000;
							break;
						}
						else if (strcmp(cheat, "V1ct0ry") == 0){
							printf("Digite a pontuacao para ganhar: ");
			    			scanf("%d",&wn);
							break;
						}
						else if (strcmp(cheat, "4ch1ll3s") == 0){
							printf("Invulnerabilidade\n ");
			    			player.loss = false;
							break;
						}
						else {
							cheat = {};
						}
					}
				}
	    		break;
	    	}
	    	case 1:
	    		setactivepage(1);
	    		readimagefile("Images/Cutscenes/cut1.bmp",0,0,res[0],res[1]);
	    		setvisualpage(1);
	    		getch();
	    		fase = 2;
	    		break;
	    	case 2:{
	    		printf("Check\n");
	    		cicles = 0;
	    		game(wn ,10, 4, 2048, 800, 2048, 256, 0);	
				enmy = NULL;
				if (result == true){
					fase = 3;
				}
				if (result == false){
					fase = 8;
					printf("%d\n",fase);					
				}
				break;
			}
				//free(enmy);	
			case 3:{
	    		setactivepage(1);
	    		readimagefile("Images/Cutscenes/cut2.bmp",0,0,res[0],res[1]);
	    		setvisualpage(1);
	    		delay(5000);   		
				getch();
	    		fase = 4;
	    		break;
			}

			case 4:
				cicles = 0;
	    		game(wn ,20, 6, 2048, 1600, 2048, 256,2);	
				enmy = NULL;
				if (result == true){
					fase = 5;
					break;
				}
				else if(result == false){
					fase = 8;
					break;
					
				}
			case 5:
				setactivepage(1);
	    		readimagefile("Images/Cutscenes/cut3.bmp",0,0,res[0],res[1]);
	    		setvisualpage(1);
	    		delay(5000);
	    		getch();
	    		fase = 6;
	    		break;
			case 6: //Chefão
			{
				int limite,hi;
				for(index = 0; index < 2; index++){
					battery[index].spawned = 0;
					battery[index].tamanho = 48;
				}
				enmy = (enemy*)realloc(enmy,15*sizeof(enemy));
			    bac.chao.larg = res[1];
			    bac.chao.alt = res[1];
			    limite = res[1]-(320*bac.chao.alt/2048);
			    player.pos.x = (int)(res[0]/2);// Mudar
			   	player.pos.y = bac.chao.alt-((bac.chao.alt-limite)/2);// Mudar
			    player.lanterna.alcance = 450;
			    player.lanterna.angle = 30;
			    player.pos.angle = 270;
			    player.tamanho = 48;
			    bac.pos.x = (int)(res[0]-res[1])/2;
			    bac.pos.y = 0;
			    boss.pos.x = res[0]/2;
			    boss.pos.y = res[1]/2;
			    boss.speed = 5;
			    boss.pos.walkingangle = rand()%360;
			    boss.health = 15000;
			    boss.aggro = 500;
				num = 7;
			    enmy = NULL;
			    enmy = (enemy*)realloc(enmy,num*sizeof(enemy));
				for(index = 0; index < 20; index++){
					enmy[index].spawned = false;
					enmy[index].tamanho = 32;
				}
			    cicles = 0;
			    while(fase == 6){
			    	for(pg = 1 ; pg <= 2 ; pg ++){
			    		start = clock();
			    		setactivepage(pg);
			    		cleardevice();
			    		//Mechanics Stuff======================================================================================
			    		if(cicles%5== 0){
							if(player.lanterna.alcance>=0 && player.loss){
								player.lanterna.alcance-=1;
							}
						}
			    		//Spawn Stuff======================================================================================
			    		for(index = 0 ; index < num; index++){
			    			chance = rand()%500;
			    			if(chance == 0 && enmy[index].spawned == false){
			    				enmy[index].posi.x = boss.pos.x + (boss.tamanho/2);
			    				enmy[index].posi.y = boss.pos.y + boss.tamanho;
			    				enmy[index].speed = rand()%7;
			    				enmy[index].speed+=2;
			    				ca = player.pos.x-enmy[index].posi.x;
			    				co = player.pos.y-enmy[index].posi.y;
			    				hi = (int)sqrt((co*co)+(ca*ca));
			    				enmy[index].posi.walkingangle = (int)(acos((double)ca/(double)hi)*((double)180/(double)3.14159265359));
			    				if(enmy[index].posi.walkingangle<0){
			    					enmy[index].posi.walkingangle+=180;
								}
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
						chance = rand()%2;
						if (cicles%30 == 0 && chance == 0){
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
						if(boss.pos.hit[0]>player.pos.x && (boss.pos.hitangle < 90 || boss.pos.hitangle > 270)){
				   			ca = (boss.pos.hit[0]-player.pos.x);
				   	    	co = ca*tan((player.lanterna.angle/2)*conv);
				   	    	if ((abs(boss.pos.hit[1]-player.pos.y)<co+boss.tamanho || abs(boss.pos.hit[1]-player.pos.y)<co-boss.tamanho) && sqrt(((boss.pos.x-player.pos.x)*(boss.pos.x-player.pos.x))+((boss.pos.y-player.pos.y)*(boss.pos.y-player.pos.y)))<=player.lanterna.alcance+boss.tamanho){
				   				boss.health -= (int)((abs(1000/co))+(abs(1000/ca)));
				   				printf("Vida do Boss: %d\n",boss.health);
				   			}
				        }
				        if(boss.health <= 10000 && boss.health > 5000){
				        	boss.aggro = 300;
				        	num = 10;
				        	boss.speed = 7;
						}
						else if (boss.health <= 5000){
							boss.aggro = 100;
				        	num = 15;
				        	boss.speed = 10;
						}
				        if(player.pos.x-player.tamanho<=boss.pos.x+boss.tamanho && player.pos.x+player.tamanho >= boss.pos.x-boss.tamanho && player.loss){
				        	if(player.pos.y-player.tamanho<=boss.pos.y+boss.tamanho && player.pos.y+player.tamanho >= boss.pos.y-boss.tamanho){
				        		player.lanterna.alcance-=7;
							}
						}
						for (index = 0;index < num;index++){
							if(enmy[index].spawned){
								if(enmy[index].posi.y >= res[1]){
									enmy[index].spawned = false;
								}
								if(enmy[index].posi.x+enmy[index].tamanho>=player.pos.x-player.tamanho && enmy[index].posi.x-enmy[index].tamanho<=player.pos.x+player.tamanho && player.loss){
									if(enmy[index].posi.y+enmy[index].tamanho>=player.pos.y-player.tamanho && enmy[index].posi.y-enmy[index].tamanho<=player.pos.y+player.tamanho){
										enmy[index].spawned = false;
										player.lanterna.alcance-=30;
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
			    		putimage(bac.pos.x,bac.pos.y,back[4],0);
			    		setcolor(RGB(255,255,0));
				        setfillstyle(1,RGB(255,255,0));
				        screenflashlight(player.pos.angle+15,player.pos.x,player.pos.y,player.lanterna.alcance,player.lanterna.angle);
				        flash={player.pos.x,player.pos.y,xpos,ypos,posx,posy,player.pos.x,player.pos.y};
				        fillpoly(4,flash);
				        if(player.moving){
				        	putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho),player.mask[player.pos.direction][cicles%5],AND_PUT);
							putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho),player.atlas[player.pos.direction][cicles%5],OR_PUT);
						}
						else{
							putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho),player.mask[player.pos.direction][0],AND_PUT);
							putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho),player.atlas[player.pos.direction][0],OR_PUT);
						}
			    		putimage(boss.pos.x-boss.tamanho,boss.pos.y-boss.tamanho,boss.mask[boss.pos.direction][cicles%4],AND_PUT);
			    		putimage(boss.pos.x-boss.tamanho,boss.pos.y-boss.tamanho,boss.img[boss.pos.direction][cicles%4],OR_PUT);
			    		setfillstyle(1,RGB(0,0,255));
			    		for(index = 0; index < num; index++){
			    			if(enmy[index].spawned == true){
			    				putimage(enmy[index].posi.x-enmy[index].tamanho,enmy[index].posi.y-enmy[index].tamanho,fbmask[(cicles+index)%5],AND_PUT);
			    				putimage(enmy[index].posi.x-enmy[index].tamanho,enmy[index].posi.y-enmy[index].tamanho,fbimg[(cicles+index)%5],OR_PUT);
							}
						}
						for(index=0;index < 2; index++){
							if(battery[index].spawned == true){
								putimage(battery[index].x-(battery[index].tamanho/2),battery[index].y-(battery[index].tamanho/2),btrmask,AND_PUT);
								putimage(battery[index].x-(battery[index].tamanho/2),battery[index].y-(battery[index].tamanho/2),btrimg,OR_PUT);
							}
						}
						mask(bac.pos.x,bac.pos.y,bac.pos.x+bac.chao.larg,bac.pos.y+bac.chao.alt);
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
						while((double)(clock()-start)<34){
						}
					}
				}
				enmy = NULL;
				free(enmy);
				delete enmy;
				break;
				}
			case 7:
				setactivepage(1);
	    		readimagefile("Images/Cutscenes/cut4.bmp",0,0,res[0],res[1]);
	    		setvisualpage(1);
	    		delay(1000);
	    		setactivepage(2);
	    		readimagefile("Images/Cutscenes/cut5.bmp",0,0,res[0],res[1]);
	    		setvisualpage(2);
	    		delay(1000);
	    		getch();
	    		fase = 0;
	    		wn = 30;
	    		player.loss = true;
	    		break;
			case 8:{
				//playing = false;
				cleardevice();
				setactivepage(1);
				readimagefile("Images/Cutscenes/over.bmp",0,0,res[0],res[1]);
				setvisualpage(1);
				delay(5000);
				getch();
				fase = 0;
				wn = 30;
				player.loss = true;
				break;
			}	
		}
	}
	cleardevice();
	for(k=0;k<8;k++){
		for(l=0;l<5;l++){
			free(player.atlas[k][l]);
			free(player.mask[k][l]);
			free(enmymask[k][l]);
			free(enmyatlas[k][l]);
		}
	}
	for(k=0;k<8;k++){
		for(l=0;l<4;l++){
			free(boss.img[k][l]);
			free(boss.mask[k][l]);
		}
	}
	for(l=0;l<4;l++){
		free(fbimg[l]);
		free(fbmask[l]);
	}
	for(l=0;l<4;l++){
		free(back[l]);
	}
	free(btrimg);
	free(btrmask);
	free(key.img);
	free(key.mask);
	printf("\n");
	system("pause");
}
