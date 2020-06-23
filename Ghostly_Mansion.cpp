#include <stdlib.h> 	//
#include <stdio.h>		//
#include <math.h>		//Bibliotecas utilizadas
#include <graphics.h>	//
#include <time.h>		//
#include <windows.h>
//Define Global Stuff===================================================================
static double conv = (3.14159265359/180); 	//Fator de conversão de graus para radianos
int res[2] = {getmaxwidth(),getmaxheight()}; 					//Tamanho da tela em pixeis
int num,ca,pg,index;
int xpos = 0;
int ypos = 0;
int posx = 0;
int posy = 0;
int hitx = 0;
int hity = 0;
int FPS = 30;
double co;
unsigned long long int start, cicles = 0;
void *enmyatlas[8][5];
void *enmymask[8][5];
void *btrimg;
void *btrmask;
void *fbimg[5];
void *fbmask[5];
void *back[5];
void *seta[4];
void *death_screen;
int flash[8];
int chance;
int fase = 0;
char *walk[8];
char *load_sound;
bool done,pausa;
//Structfy stuff=======================================================
struct player{ 										//Struct para organizar as variaveis do jogador
	int tamanho,health,score,life;							//Variaveis para guardar o tamanho da imagem, a vida e a pontuação do jogador respectivamente
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
	pos posi;											//Criar o objeto posi que referencia a struct pos
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
struct background{	
		bool playing;							//Struct para organizar as variaveis do background
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
	}
}
void mask(int left, int top, int right, int bottom){
	int points[10];
	setfillstyle(1,RGB(0,0,0));
	setcolor(RGB(0,0,0));
	points[0] = 0;
	points[1] = 0;
	points[2] = 0;
	points[3] = res[1];
	points[4] = left;
	points[5] = res[1];
	points[6] = left;
	points[7] = 0;
	points[8] = 0;
	points[9] = 0;
	fillpoly(5,points);
	points[0] = 0;
	points[1] = 0;
	points[2] = 0;
	points[3] = top;
	points[4] = res[0];
	points[5] = top;
	points[6] = res[0];
	points[7] = 0;
	points[8] = 0;
	points[9] = 0;
	fillpoly(5,points);
	points[0] = 0;
	points[1] = bottom;
	points[2] = 0;
	points[3] = res[1];
	points[4] = res[0];
	points[5] = res[1];
	points[6] = res[0];
	points[7] = bottom;
	points[8] = 0;
	points[9] = bottom;
	fillpoly(5,points);
	points[0] = right;
	points[1] = 0;
	points[2] = right;
	points[3] = res[1];
	points[4] = res[0];
	points[5] = res[1];
	points[6] = res[0];
	points[7] = 0;
	points[8] = 0;
	points[9] = 0;
	fillpoly(5,points);
}
void load_img(){
	initwindow(2048,2048);
	int i,j,k;
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
	tam = imagesize(0,0,128,128);
	readimagefile("Images/Fantasma/bitmap.bmp",0,0,640,1024);
	for(i=0;i<8;i++){
		for(j=0;j<5;j++){
			enmyatlas[i][j] = malloc(tam);
			getimage(j*128,i*128,(j+1)*128,(i+1)*128,enmyatlas[i][j]);
			
		}
	}
	cleardevice();
	readimagefile("Images/Fantasma/bitmap_mask.bmp",0,0,640,1024);
	for(i=0;i<8;i++){
		for(j=0;j<5;j++){
			enmymask[i][j] = malloc(tam);
			getimage(j*128,i*128,(j+1)*128,(i+1)*128,enmymask[i][j]);
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
	readimagefile("Images/Background/biblioteca_parede.bmp",0,0,2048,256);
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
	cleardevice();
	tam = imagesize(0,0,128,128);
	readimagefile("Images/Menus/Seta.bmp",0,0,256,256);
	k = 0;
	for(i = 0; i < 2;i++){
		for(j = 0; j < 2; j++){
			seta[k] = malloc(tam);
			getimage(j*128,i*128,(j+1)*128,(i+1)*128,seta[k]);
			k++;
		}
	}
	cleardevice();
	tam = imagesize(0,0,res[0],res[1]);
	death_screen = malloc(tam);
	readimagefile("Images/Menus/Died.bmp",0,0,res[0],res[1]);
	getimage(0,0,res[0],res[1],death_screen);
	closegraph();
}
void text(int color,int tam, int x, int y, char* txt){
	setcolor(color);
	setfillstyle(1,color);
	settextstyle(0,0,tam);
	outtextxy(x,y,txt);
}
void draw_death_screen(int vida){
	char buffer[1];
	cleardevice();
	putimage(0,0,death_screen,COPY_PUT);
	setcolor(RGB(255,0,0));
	setfillstyle(1,RGB(0,0,0));
	rectangle(0,(760*res[1])/1080,res[0],res[1]);
	floodfill(res[0]-1,res[1]-1,RGB(255,0,0));
	setcolor(RGB(0,0,0));
	setfillstyle(1,RGB(0,0,0));
	rectangle(0,(760*res[1])/1080,res[0],res[1]);
	readimagefile("Images/Menus/Icon.bmp",(790*res[0])/1920,(835*res[1])/1080,(884*res[0])/1920,(929*res[1])/1080);
	text(RGB(255,255,255),60,(890*res[0])/1920, (882*res[1])/1080, (char*)"X");
	text(RGB(255,255,255),64,(950*res[0])/1920, (850*res[1])/1080,(char*)itoa(vida+1,buffer,10));
}
int check_direction(int angle){
	int direction;
	//printf("Angle = %d\n",angle);
    if(angle > 68 && angle <= 112){
		direction = 4;
	}
	else if(angle > 112 && angle <= 157){
		direction = 5;
	}
	else if(angle > 157 && angle <= 202){
		direction = 6;
	}
	else if(angle > 202 && angle <= 247){
		direction = 7;
	}
	else if(angle > 247 && angle <= 292){
		direction = 0;
	}
	else if(angle > 292 && angle <= 337){
		direction = 1;
	}			
	else if((angle > 337 && angle <= 360) || (angle >= 0 && angle <= 23)){
		direction = 2;
	}
	else if(angle > 23 && angle <= 68){
		direction = 3;
	}
	return(direction);
}
bool check_button(int left, int top, int right, int bottom){
	bool click = false;
	if ((GetAsyncKeyState(VK_LBUTTON)) && 0x8000){
		if(mousex() >= left && mousex() <= right && mousey() >= top && mousey() <= bottom){
			printf("Click\n");
			click = true;
		}
	}
	return(click);
}
bool game(int win, int num, int speed, int chaox, int chaoy,int parx, int pary, int level){
	bool result,on_screen;
	pausa = false;
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
    bac.chao.larg = chaox;
    bac.chao.alt = chaoy;
    bac.parede.alt = pary;
    bac.parede.larg = parx;
	player.pos.x = (int)(res[0]/2);
	player.pos.y = (int)(res[1]/2);	
    player.key = false;
    bac.pos.x = player.pos.x-((int)bac.chao.larg/2);
    bac.pos.y = player.pos.y-((int)bac.chao.alt/2);
    player.pos.relx = player.pos.x - bac.pos.x;
    player.pos.rely = player.pos.y - bac.pos.y;
    long long unsigned int count = 0;
    if(!bac.playing){
    	mciSendString("play fase from 0 repeat",NULL,0,0);
    	bac.playing = true;
	}
	while(!done){
		for(pg = 1; pg<=2;pg++){
			while(pausa){
				setactivepage(pg);
				text(RGB(255,255,255),96,(800*res[0])/1920,(450*res[1])/1080,(char*)"Pause");
				setvisualpage(pg);
				if (GetAsyncKeyState(VK_ESCAPE) && 0x8000){
					pausa = !pausa;
					delay(250);
				}
			}
			start = GetTickCount();
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
				chance=rand()%75;
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
			if(cicles%30 == 0){
				on_screen = !on_screen;
			}
	    	//Move stuff=======================================================================================
	    	player.pos.relx = player.pos.x - bac.pos.x;
	    	player.pos.rely = player.pos.y - bac.pos.y;
	    	player.moving = false;
	    	for(index = 0;index < num; index++){
	    		enmy[index].posi.x = player.pos.x+(int)(enmy[index].posi.relx-player.pos.relx);
	    		enmy[index].posi.y = player.pos.y+(int)(enmy[index].posi.rely-player.pos.rely);
			}
	    	if (GetAsyncKeyState(VK_ESCAPE) && 0x8000){
					pausa = !pausa;
					delay(250);
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
		    printf("player.pos.direction =");
		    player.pos.direction = check_direction(player.pos.angle);
			for(index = 0; index < num; index ++){
				if(enmy[index].spawned == 1){
					chance = rand()%3;
					if (enmy[index].posi.relx-enmy[index].tamanho < 0 || enmy[index].posi.rely-enmy[index].tamanho < 0 || enmy[index].posi.relx+enmy[index].tamanho > bac.chao.larg || enmy[index].posi.rely+enmy[index].tamanho > bac.chao.alt){
						enmy[index].posi.walkingangle += 180;
						if(enmy[index].posi.walkingangle>360){
							enmy[index].posi.walkingangle-=360;
						}
					}
					else if (cicles%30 == 0 && chance == 0){
						enmy[index].posi.walkingangle = rand()%360;
					}
					enmy[index].posi.relx+=cos(enmy[index].posi.walkingangle*conv)*(int)enmy[index].speed;
			       	enmy[index].posi.rely+=sin(enmy[index].posi.walkingangle*conv)*(int)enmy[index].speed;
			       	enmy[index].posi.direction = check_direction(enmy[index].posi.walkingangle);
			       	printf("enmy.posi.direction = %d\n",enmy[index].posi.direction);
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
				}
				if((enmy[index].posi.x+enmy[index].tamanho>=player.pos.x-player.tamanho && enmy[index].posi.x-enmy[index].tamanho<=player.pos.x+player.tamanho)&& enmy[index].spawned && player.loss){
					if(enmy[index].posi.y+enmy[index].tamanho>=player.pos.y-player.tamanho && enmy[index].posi.y-enmy[index].tamanho<=player.pos.y+player.tamanho){
						if(player.lanterna.alcance > 0){
								player.lanterna.alcance-=10;
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
	    			player.lanterna.alcance+=75;
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
	        setcolor(RGB(64,64,0));
	        setfillstyle(1,RGB(64,64,0));
	        screenflashlight(player.pos.angle+15,player.pos.x,player.pos.y,player.lanterna.alcance,player.lanterna.angle);
	        flash[0] = player.pos.x;
			flash[1] = player.pos.y;
			flash[2] = xpos;
			flash[3] = ypos;
			flash[4] = posx;
			flash[5] = posy;
			flash[6] = player.pos.x;
			flash[7] = player.pos.y;
	        fillpoly(4,flash);
			putimage(bac.pos.x,bac.pos.y,back[level+1],OR_PUT);
			for(index=0;index < 2; index++){
				if(battery[index].spawned == true){
					putimage(battery[index].x-(battery[index].tamanho/2),battery[index].y-(battery[index].tamanho/2),btrmask,AND_PUT);
					putimage(battery[index].x-(battery[index].tamanho/2),battery[index].y-(battery[index].tamanho/2),btrimg,OR_PUT);
				}
			}
	  		for(index = 0;index < num; index++){
		        if(enmy[index].posi.x > - enmy[index].tamanho && enmy[index].posi.x < res[0]+enmy[index].tamanho && enmy[index].posi.y > -enmy[index].tamanho && enmy[index].posi.y < res[1]+enmy[index].tamanho && enmy[index].spawned == 1){
					putimage(enmy[index].posi.x-enmy[index].tamanho,enmy[index].posi.y-enmy[index].tamanho,enmymask[enmy[index].posi.direction][cicles%5],AND_PUT);
					putimage(enmy[index].posi.x-enmy[index].tamanho,enmy[index].posi.y-enmy[index].tamanho,enmyatlas[enmy[index].posi.direction][cicles%5],OR_PUT);
				}
			}
			if(key.spawned == true){
				putimage(key.x-(key.tamanho/2),key.y-(key.tamanho/2),key.mask,AND_PUT);
				putimage(key.x-(key.tamanho/2),key.y-(key.tamanho/2),key.img,OR_PUT);
				
			}
			if(player.moving){
				putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho*1.3),player.mask[player.pos.direction][(cicles/2)%5],AND_PUT);
				putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho*1.3),player.atlas[player.pos.direction][(cicles/2)%5],OR_PUT);
				if(cicles%6 == 0){
					chance = rand()%8;
					mciSendString(walk[chance],NULL,0,0);
				}
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
				if(on_screen){
					if(num == 10){
						putimage(bac.pos.x+10,bac.pos.y+(bac.chao.alt/4),seta[0],AND_PUT);
						putimage(bac.pos.x+10,bac.pos.y+(bac.chao.alt/4),seta[1],OR_PUT);
					}
					else if(num == 20){
						putimage(bac.pos.x+(bac.chao.larg/2)-64,bac.pos.y+10,seta[2],AND_PUT);
						putimage(bac.pos.x+(bac.chao.larg/2)-64,bac.pos.y+10,seta[3],OR_PUT);
					}
				}
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
			while((double)(GetTickCount()-start)<(1000/FPS)){
			}
		}
	}
	return(result);
}
//Actual code stuff========================================================
int main(){	
 	waveOutSetVolume(0,0xFFFFFFFF);
	for(index=0;index<8;index++){
		walk[index] = (char*)malloc(20*sizeof(char));
	}
	//Define stuff===========================================================================================
	player.lanterna.angle = 30;
	player.pos.angle = 270;
	player.tamanho = 48;
	player.loss = true;
	key.tamanho = 64;
	boss.tamanho = 64;
	srand((unsigned)time(NULL));
	int gdriver = DETECT, gmode;
	initgraph(&gdriver, &gmode, (char*)"");
	load_img();
	initwindow(res[0],res[1],"Ghostly Mansion");
	//Play stuff===========================================================================================
	cleardevice();
	fase = 0;
	bool playing = true;
	bool resultado,again;
	int wn = 30;
	int k,l;
	setbkcolor(RGB(0,0,0));
    while(playing){
    	switch (fase){
	    	case 0:{
	    		printf("Menu\n");
	    		char menu[2][26]={"Images/Menus/menu.bmp","Images/Menus/menu_2.bmp"};
	    		int imagem = 0;
	    		player.life = 2;
	    		while(imagem < 2){
					for(pg = 1;pg<=2;pg++){
						char cheat[10] = {};
						setactivepage(pg);
						cleardevice();
						readimagefile(menu[imagem],0,0,res[0],res[1]);
						if(imagem == 0){
							if (check_button((190*res[0])/1920,(650*res[1])/1080,(440*res[0])/1920,(770*res[1])/1080)){
								fase = 1;
								imagem = 3000;
								break;
							}
							else if(check_button((190*res[0])/1920,(800*res[1])/1080,(720*res[0])/1920,(890*res[1])/1080)){	
									imagem = 1;
								}
							else if (GetAsyncKeyState(VK_ESCAPE) && 0x8000){
								playing = false;
								imagem = 3000;
								break;
							}
						}
						else if(imagem == 1){
							if (check_button((1650*res[0])/1920,(970*res[1])/1080,(1890*res[0])/1920,(1050*res[1])/1080) || GetAsyncKeyState(VK_ESCAPE) && 0x8000){
								imagem = 0;
							}
						}
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
							for(k = 0; k<10; k++){
								cheat[k] = '\0';
							}
						}
					}
				}
	    		break;
	    	}
	    	case 1:
	    		setactivepage(1);
	    		readimagefile("Images/Cutscenes/cut1.bmp",0,0,res[0],res[1]);
	    		setvisualpage(1);
	    		load_sound = (char*)malloc(100*sizeof(char));
				for(index = 1;index <=8 ; index++){
					sprintf(load_sound,"open .\\SFX\\Walking\\Stone%d.mp3 type MPEGVideo alias stone%d",index,index);
					sprintf(walk[index-1],"play stone%d from 0",index);
					mciSendString((char*)load_sound,NULL,0,0);
				}
				mciSendString("open .\\SFX\\Background\\Fase.mp3 type MPEGVideo alias fase",NULL,0,0);
	    		delay(1000);
	    		setactivepage(1);
	    		text(RGB(255,255,255),32,(1650*res[0])/1920,res[1]-32,"Aperte Enter");
	    		setvisualpage(1);
	    		do{
				}while(!((GetAsyncKeyState(VK_RETURN) && 0x8000)));
	    		fase = 2;
	    		break;
	    	case 2:{
	    		printf("Check\n");
	    		cicles = 0;
	    		enmy = NULL;
	    		resultado = game(15 ,10, 4, 2048, 800, 2048, 256, 0);	
				if (resultado == true){
					fase = 3;
				}
				if (resultado == false){
					if(player.life>=1){
						player.score = 0;
						player.key = false;
						key.spawned = false;
						setactivepage(1);
						draw_death_screen(player.life);
						player.life--;
						setvisualpage(1);
						delay(1000);
						setactivepage(2);
						draw_death_screen(player.life);
						setvisualpage(2);
						delay(2000);
						setactivepage(1);
						cleardevice();
						pg=2;
						do{
							if(pg==1){
								pg = 2;
							}
							else{
								pg = 1;
							}
							setactivepage(pg);
							cleardevice();
							setcolor(RGB(255,255,255));
							setfillstyle(1,RGB(255,255,255));
							rectangle((580*res[0])/1920,(130*res[1])/1080,(1340*res[0])/1920,(750*res[1])/1080);
							floodfill((581*res[0])/1920,(131*res[1])/1080,RGB(255,255,255));
							rectangle((295*res[0])/1920,(820*res[1])/1080,(1690*res[0])/1920,(950*res[1])/1080);
							if(mousex() >= (295*res[0])/1920 && mousex() <= (1690*res[0])/1920 && mousey() >= (820*res[1])/1080 && mousey() <= (950*res[1])/1080){
								setfillstyle(1,RGB(255,255,0));
							}
							floodfill((296*res[0])/1920,(821*res[1])/1080,RGB(255,255,255));
							putimage(0,0,death_screen,AND_PUT);
							if(GetAsyncKeyState(VK_ESCAPE) && 0x8000){
								fase=8;
								break;
							}
							setvisualpage(pg);
						}while(!check_button((295*res[0])/1920,(820*res[1])/1080,(1690*res[0])/1920,(950*res[1])/1080));
					}			
					else if(player.life<1){
						fase = 8;
						printf("%d\n",fase);	
					}		
				}
				break;
			}	
			case 3:{
	    		setactivepage(1);
	    		readimagefile("Images/Cutscenes/cut2.bmp",0,0,res[0],res[1]);
	    		setvisualpage(1);
	    		load_sound = (char*)malloc(100*sizeof(char));
				for(index = 1;index <=8 ; index++){
					sprintf(load_sound,"close stone%d",index);
					mciSendString((char*)load_sound,NULL,0,0);
					sprintf(load_sound,"open .\\SFX\\Walking\\Wood%d.mp3 type MPEGVideo alias wood%d",index,index);
					sprintf(walk[index-1],"play wood%d from 0",index);
					mciSendString((char*)load_sound,NULL,0,0);
				}
	    		delay(1000);   		
	    		text(RGB(255,255,255),32,(1650*res[0])/1920,res[1]-32,"Aperte Enter");
				do{
				}while(!((GetAsyncKeyState(VK_RETURN) && 0x8000)));
	    		fase = 4;
	    		break;
			}

			case 4:
				cicles = 0;
				enmy = NULL;
	    		resultado = game(25 ,20, 6, 2048, 1600, 2048, 256,2);	
				if (resultado == true){
					fase = 5;
					mciSendString("close fase",NULL,0,0);
					break;
				}
				if (resultado == false){
					if(player.life>=1){
						player.score = 0;
						player.key = false;
						key.spawned = false;
						setactivepage(1);
						draw_death_screen(player.life);
						player.life--;
						setvisualpage(1);
						delay(1000);
						setactivepage(2);
						draw_death_screen(player.life);
						setvisualpage(2);
						delay(2000);
						setactivepage(1);
						cleardevice();
						readimagefile("Images/Menus/Died.bmp",0,0,res[0],res[1]);
						setvisualpage(1);
						while(!check_button((295*res[0])/1920,(820*res[1])/1080,(1690*res[0])/1920,(950*res[1])/1080)){
							if(GetAsyncKeyState(VK_ESCAPE) && 0x8000){
								fase = 8;
								break;
							}
						}
					}			
					else if(player.life<1){
						fase = 8;
						printf("%d\n",fase);
						mciSendString("close fase",NULL,0,0);	
					}		
				}
				free(enmy);
				break;
			case 5:
				setactivepage(1);
	    		readimagefile("Images/Cutscenes/cut3.bmp",0,0,res[0],res[1]);
	    		setvisualpage(1);	    		
				mciSendString("open .\\SFX\\Background\\Boss.mp3 type MPEGVideo alias fase",NULL,0,0);
	    		delay(1000);
	    		text(RGB(255,255,255),32,(1630*res[0])/1920,res[1]-32,"Aperte Enter");
	    		do{
				}while(!((GetAsyncKeyState(VK_RETURN) && 0x8000)));
	    		fase = 6;
	    		//break;
			case 6:{
				printf("0");
				cleardevice();
				int limite,hi;
				double tg;
				bool chefao = true;
				for(index = 0; index < 2; index++){
					battery[index].spawned = 0;
					battery[index].tamanho = 48;
				}
				enmy = NULL;
				enmy = (enemy*)realloc(enmy,15*sizeof(enemy));
			    bac.chao.larg = res[1];
			    bac.chao.alt = res[1];
			    limite = res[1]-(320*bac.chao.alt/2048);
			    player.pos.x = (int)(res[0]/2);
			   	player.pos.y = bac.chao.alt-((bac.chao.alt-limite)/2);
			    player.lanterna.alcance = 450;
			    player.lanterna.angle = 30;
			    player.pos.angle = 270;
			    player.tamanho = 48;
			    bac.pos.x = (int)(res[0]-res[1])/2;
			    bac.pos.y = 0;
			    boss.pos.x = res[0]/2;
			    boss.pos.y = boss.tamanho+20;
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
				pausa = false;
			    cicles = 0;
			    if(!bac.playing){
			    	mciSendString("play fase from 0 repeat",NULL,0,0);
			    	bac.playing = true;
				}
			    while(chefao){
			    	for(pg = 1 ; pg <= 2 ; pg ++){
				    	while(pausa){
							if (GetAsyncKeyState(VK_ESCAPE) && 0x8000){
								pausa = !pausa;
								delay(250);
							}
						}
			    		start = GetTickCount();
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
			    			chance = rand()%300;
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
							chance=rand()%500;
							if(battery[index].spawned == 0 && chance == 0){
								battery[index].spawned = 1;
								battery[index].x = 0;
								battery[index].y = 0;
								while(battery[index].x < bac.pos.x){
									battery[index].x = rand()%bac.chao.larg;
								}
								
								while(battery[index].y <= limite){
									battery[index].y = rand()%bac.chao.alt;	
								}
							}
						}
			    		//Move Stuff=====================================================================================
			    		player.moving = false;
			    			if (GetAsyncKeyState(VK_ESCAPE) && 0x8000){
								pausa = !pausa;
								delay(250);
							}
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
						ca = mousex()-player.pos.x;
						if(ca < 1 && ca > -1){
							ca = 1;
						} 
						co = mousey()-player.pos.y;
						player.pos.angle = (int)(atan(co/ca)*(180/3.14159265359));
						if (player.pos.angle == 0){
							player.pos.angle +=1;
						}
						if (player.pos.angle<0){
							player.pos.angle+=180;
						}
						if(mousey()<player.pos.y){
							player.pos.angle+=180;
						}
						if(mousex()<player.pos.x && mousey() == player.pos.y){
							player.pos.angle = 181;
						}
						if(player.pos.angle>359){
				            player.pos.angle = 1;
				        }
						if(player.pos.angle<0){
				            player.pos.angle = 359;
				        }
				        player.pos.direction = check_direction(player.pos.angle);
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
				       	boss.pos.direction = check_direction(boss.pos.walkingangle);
				       	for(index = 0; index < num;index++){
				       		if(enmy[index].spawned == true){
				       			enmy[index].posi.x+=cos(enmy[index].posi.walkingangle*conv)*enmy[index].speed;	
				       			enmy[index].posi.y+=sin(enmy[index].posi.walkingangle*conv)*enmy[index].speed;
							}
						}
						//Hit Stuff=====================================================================================
						printf("5");
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
				        		player.lanterna.alcance-=20;
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
										player.lanterna.alcance-=50;
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
							}
						}
						
			    		//Drawstuff=====================================================================================
			    		setcolor(RGB(128,128,0));
				        setfillstyle(1,RGB(128,128,0));
				        screenflashlight(player.pos.angle+15,player.pos.x,player.pos.y,player.lanterna.alcance,player.lanterna.angle);
				        flash[0] = player.pos.x;
						flash[1] = player.pos.y;
						flash[2] = xpos;
						flash[3] = ypos;
						flash[4] = posx;
						flash[5] = posy;
						flash[6] = player.pos.x;
						flash[7] = player.pos.y;
				        fillpoly(4,flash);
			    		putimage(bac.pos.x,bac.pos.y,back[4],OR_PUT);
			    		for(index=0;index < 2; index++){
							if(battery[index].spawned == true){
								putimage(battery[index].x-(battery[index].tamanho/2),battery[index].y-(battery[index].tamanho/2),btrmask,AND_PUT);
								putimage(battery[index].x-(battery[index].tamanho/2),battery[index].y-(battery[index].tamanho/2),btrimg,OR_PUT);
							}
						}
				        if(player.moving){
				        	putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho),player.mask[player.pos.direction][(cicles/2)%5],AND_PUT);
							putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho),player.atlas[player.pos.direction][(cicles/2)%5],OR_PUT);
							if(cicles%6 == 0){
								chance = rand()%8;
								mciSendString(walk[chance],NULL,0,0);
							}
						}
						else{
							putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho),player.mask[player.pos.direction][0],AND_PUT);
							putimage(player.pos.x-(player.tamanho),player.pos.y-(player.tamanho),player.atlas[player.pos.direction][0],OR_PUT);
						}
			    		putimage(boss.pos.x-boss.tamanho,boss.pos.y-boss.tamanho,boss.mask[boss.pos.direction][cicles%4],AND_PUT);
			    		putimage(boss.pos.x-boss.tamanho,boss.pos.y-boss.tamanho,boss.img[boss.pos.direction][cicles%4],OR_PUT);
			    		for(index = 0; index < num; index++){
			    			if(enmy[index].spawned == true){
			    				putimage(enmy[index].posi.x-enmy[index].tamanho,enmy[index].posi.y-enmy[index].tamanho,fbmask[(cicles+index)%5],AND_PUT);
			    				putimage(enmy[index].posi.x-enmy[index].tamanho,enmy[index].posi.y-enmy[index].tamanho,fbimg[(cicles+index)%5],OR_PUT);
							}
						}
						mask(bac.pos.x,bac.pos.y,bac.pos.x+bac.chao.larg,bac.pos.y+bac.chao.alt);
						//Win/lose====================================================================================================
						if(boss.health <= 0){
							fase = 7;
							break;
						}
						else if (player.lanterna.alcance <= 0){
							if(player.life>=1){
								mciSendString("stop fase",NULL,0,0);
								bac.playing = false;
								setactivepage(1);
								draw_death_screen(player.life);
								player.life--;
								setvisualpage(1);
								delay(1000);
								setactivepage(2);
								draw_death_screen(player.life);
								setvisualpage(2);
								delay(2000);
								setactivepage(1);
								cleardevice();
								readimagefile("Images/Menus/Died.bmp",0,0,res[0],res[1]);
								setvisualpage(1);
								while(!check_button((295*res[0])/1920,(820*res[1])/1080,(1690*res[0])/1920,(950*res[1])/1080)){
									if(GetAsyncKeyState(VK_ESCAPE) && 0x8000){
										fase = 8;
										break;
									}
								}
								chefao = false;
								break;
							}			
							else if(player.life<1){
								fase = 8;
								chefao = false;
								break;	
							}		
						}
			    		setvisualpage(pg);
			    		cicles++;
			    		if(cicles >= 65534){
				        	cicles = 0;
						}
						while((double)(GetTickCount()-start)<(1000/FPS)){
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
	    		getch();
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
				cleardevice();
				setactivepage(1);
				readimagefile("Images/Cutscenes/over.bmp",0,0,res[0],res[1]);
				setvisualpage(1);
				delay(1000);
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
