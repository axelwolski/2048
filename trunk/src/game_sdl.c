#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#include <math.h>
#include <grid.h>

grid initialize();
static void print_wall();
static void display_game(grid g);
static void display_score(grid g);
static void event_play(grid g);
static void print_tile(grid g, int x, int y, int t);
static void print_game_over();
static void reset(grid g);

//pointeurs qui stocke les surface de l'écran
SDL_Surface *ecran, *grille, *restart, *top, *man, *go;

//nombre de case
int nb = (GRID_SIDE*99 + 4)/2;

static int display_wind(){

}

grid initialize(){
	grid g = new_grid();
	add_tile(g);
	add_tile(g);
	display_game(g);
	return g;
}

int main(int argc, char* argv[]){
	ecran = NULL;
	grille = NULL;
	restart = NULL;
	top = NULL;
	man = NULL;
	go = NULL;

	SDL_Event event;
	int continuer = 1;

	//initialise SDL et permet de vérifier qu'il n'y a pas d'erreur
	if(SDL_Init(SDL_INIT_VIDEO) == -1){ 
		printf("\n Erreur d'initialisation de la SDL \n");
		exit(EXIT_FAILURE);
	}
	//initialise TTF et permet de vérifier qu'il n'y a pas d'erreur
	if (TTF_Init() == -1){
		printf("\n Erreur d'initialisation de TTF_Init \n");
		exit(EXIT_FAILURE);
	}

	//Chargement de l'icone
	SDL_WM_SetIcon(IMG_Load("../SDLadd/icone_2048.png"), NULL);

	// ouverture d'une fenetre
	ecran = SDL_SetVideoMode(600, 700, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	// permet de nommé la fenetre
	SDL_WM_SetCaption("2048", NULL);
	//Coloration de la surface(fenetre) ecran de la couleur souhaité
	SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 253, 223, 162));

	print_wall();
	grid g = initialize();
	display_score(g);
	event_play(g);

	//libére la mémoire des surfaces créé
	SDL_FreeSurface(grille);
	SDL_FreeSurface(restart);
	SDL_FreeSurface(top);
	SDL_FreeSurface(man);
	SDL_FreeSurface(go);

	//suppresion de la grille pour évité les fuite de mémoire
	delete_grid(g);

	// arret du mode TTF
	TTF_Quit();

	// arret de la SDL
	SDL_Quit(); 
	
	//fermeture du programme
	return EXIT_SUCCESS; 
}

static void event_play(grid g){
	SDL_Event event;
	dir d = -1;
	int continuer = 1;
	do{
		//récupération de l'événements dans event
		SDL_WaitEvent(&event);
		if(game_over(g)){
			print_game_over();
		}
		switch(event.type){
			//quitter en cliquant sur la croix
			case SDL_QUIT:
				continuer = 0;
				break;
			//si on appui sur le clavier
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					// touche directionnel droite
					case SDLK_RIGHT:
						d = RIGHT;
						break;
					// touche directionnel haut
					case SDLK_UP:
						d = DOWN;
						break;
					// touche directionnel gauche
					case SDLK_LEFT:
						d = LEFT;
						break;
					// touche directionnel bas
					case SDLK_DOWN:
						d = UP;
						break;
					// touche fin
					case SDLK_END:
						continuer = 0;
						break;
				}
				/* si les conditions sont replies alors il peut y avoir déplacement */
    			if(d!=-1 && can_move(g,d)){
    				play(g, d);
    				/* afin de modifier l'affichage dans la grille */
    				display_game(g);
    				/* modifie le score après mouvement */
    				display_score(g);
    			}
    			break;
    		case SDL_MOUSEBUTTONDOWN:
				if(event.button.button == SDL_BUTTON_LEFT){
					// si le curseur ce trouve entre tel et tel coordonnées
					if(187.5 <= event.button.x && event.button.x <= 412.5 && (355 + nb) <= event.button.y && event.button.y <=(430 + nb)){
						reset(g);
						continuer = 0;
					}
				}
				break;
		}
	}while(continuer!=0);
}

static void print_wall(){
	//position du début de la grille
	int wG = 300 - nb;
	int hG = 350 - nb;
	//permet d'avoir la position à l'écran
	SDL_Rect positionG, positionRestart, positionTop, positionMan;

	//charge les image correspondante
	grille = IMG_Load("../SDLadd/case_grille.png");
	restart = IMG_Load("../SDLadd/button_restart.png");
	top = IMG_Load("../SDLadd/top.png");
	man = IMG_Load("../SDLadd/man.png");

	// les coordonnées des surfaces à ajouter
	for(int i =0; i<GRID_SIDE; i++){
		for(int j=0; j<GRID_SIDE; j++){
			positionG.x = wG + 99*i;
			positionG.y = hG + 99*j;
			SDL_BlitSurface(grille, NULL, ecran, &positionG);
		}
	}

	positionRestart.x = 187.5;
	positionRestart.y = 355 + nb;

	positionTop.x = 100;
	positionTop.y = hG - 85;

	positionMan.x = 0;
	positionMan.y = 451 + nb;

	//collage de la surface rectangle sur l'écran
	SDL_BlitSurface(restart, NULL, ecran, &positionRestart);
	SDL_BlitSurface(top, NULL, ecran, &positionTop);
	SDL_BlitSurface(man, NULL, ecran, &positionMan);
}

static void display_game(grid g){
	
	for (int i=0; i<GRID_SIDE; i++){
		for(int j=(GRID_SIDE-1); j>=0; j--){
			if(get_tile(g,i,j)!=0){
				int m = pow(2, get_tile(g,i,j));
				print_tile(g,i,j,m);
			}
			else{
				print_tile(g,i,j,0);
			}
		}
	}

	//on actualise l'écran
	SDL_Flip(ecran);
}

static void display_score(grid g){
	int hG = 350 - nb;
	SDL_Surface *text;
	// déclaration de la possition de score
	SDL_Rect positionScore;
	// déclaration du pointeur pour la police de caractère
	TTF_Font *style = NULL;
	// la couleur du score
	SDL_Color col = {255,247,153}, orange = {245, 88, 12};
	// déclaration d'un tableau de caractére qui contiendra chaque chiffre du score
	char tabtext[21];

	//on associe le style voulu pour le score
	style = TTF_OpenFont("../SDLadd/Sketch_Block.ttf", 20);
	
	// on place le score dans le tableau
	sprintf(tabtext, "%ld", grid_score(g));
	
	text = TTF_RenderText_Shaded(style,tabtext,col, orange);
	
	//coordonnées du score dans la fenetre
	positionScore.x = 395;
	positionScore.y = hG - 44;
	
	//on blitte (colle) le score sur la fenetre
	SDL_BlitSurface(text, NULL, ecran, &positionScore);
	//on actualise l'écran
	SDL_Flip(ecran);
	TTF_CloseFont(style);
	SDL_FreeSurface(text);
}

static void print_tile(grid g, int x, int y, int t){
	//position du début de la grille
	int wG = 300 - nb;
	int hG = 350 - nb;

	SDL_Surface *tuile[13] = {NULL};
	
	SDL_Rect positionTuile;

	positionTuile.x = x*99 + wG+4;
	positionTuile.y = y*99 + hG+4;
  	switch(t){
  		case 2:
  			tuile[0] = IMG_Load("../SDLadd/tuille/2.png");
    		SDL_BlitSurface(tuile[0], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
  		case 4:
  			tuile[1] = IMG_Load("../SDLadd/tuille/4.png");
    		SDL_BlitSurface(tuile[1], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
  		case 8:
  			tuile[2] = IMG_Load("../SDLadd/tuille/8.png");
    		SDL_BlitSurface(tuile[2], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
  		case 16:
  			tuile[3] = IMG_Load("../SDLadd/tuille/16.png");
    		SDL_BlitSurface(tuile[3], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
  		case 32:
  			tuile[4] = IMG_Load("../SDLadd/tuille/32.png");
    		SDL_BlitSurface(tuile[4], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
  		case 64:
  			tuile[5] = IMG_Load("../SDLadd/tuille/64.png");
    		SDL_BlitSurface(tuile[5], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
  		case 128:
  			tuile[6] = IMG_Load("../SDLadd/tuille/128.png");
    		SDL_BlitSurface(tuile[6], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
  		case 256:
  			tuile[7] = IMG_Load("../SDLadd/tuille/256.png");
    		SDL_BlitSurface(tuile[7], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
  		case 512:
  			tuile[8] = IMG_Load("../SDLadd/tuille/512.png");
    		SDL_BlitSurface(tuile[8], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
  		case 1024:
  			tuile[9] = IMG_Load("../SDLadd/tuille/1024.png");
    		SDL_BlitSurface(tuile[9], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
  		case 2048:
  			tuile[10] = IMG_Load("../SDLadd/tuille/2048t.png");
    		SDL_BlitSurface(tuile[10], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
    	case 4096:
    		tuile[11] = IMG_Load("../SDLadd/tuille/4096.png");
    		SDL_BlitSurface(tuile[11], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
    		break;
    	case 0:
    		tuile[12] = IMG_Load("../SDLadd/tuille/0.png");
    		SDL_BlitSurface(tuile[12], NULL, ecran, &positionTuile);
    		SDL_Flip(ecran);
  			SDL_FreeSurface(*tuile);
  	}
}

static void print_game_over(){

	SDL_Rect positionGO;
	go = IMG_Load("../SDLadd/GAME_OVER.png");
	positionGO.x = 150;
	positionGO.y = 250;
	SDL_BlitSurface(go, NULL, ecran, &positionGO);
	SDL_Flip(ecran);

}

static void reset(grid g){
	delete_grid(g);
	print_wall();
	g = initialize();
	display_score(g);
	event_play(g);
}