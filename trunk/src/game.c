#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ncurses.h>
#include <math.h>
#include <time.h>
#include "grid.h"

/* déclarations des fonctions afin de ne pas faire de fichier game.h */
WINDOW *create_newwin(int height, int width, int starty, int startx);
void display_game();
void display_score();
void destroy_FGrille();
void destroy_Status();
grid initialize();
void init_score();


/* initialisation des cases de la grille qui sont des fenetres sous ncurses */
WINDOW *FGrille[GRID_SIDE][GRID_SIDE];
/*initialisation de la fenetre pour écrire le score */
WINDOW *Status;

grid initialize(){
  	int startx, starty, width, height, i, j;
  	/* hauteur et largeur d'une fenetre */
  	height = 5;
  	width = 8;
  	/* positionnement de la grille dans la fenetre de terminal */
  	starty = (LINES - GRID_SIDE * height) / 2;
  	startx = (COLS  - GRID_SIDE * width)  / 2;
  	/* création de la grille */
	grid g = new_grid();
	/* ajout des valeurs alléatoire dans la grille */
	add_tile(g);
	add_tile(g);
	/* création de chaque fenetres correspondant à chaque cases de la grille*/
  	for(i=0; i<GRID_SIDE; i++){
    	for(j=(GRID_SIDE-1); j>=0; j--){
    		/* la fenetre de coordonnées i et j sera de hauteur height et de largeur width et sont angle superieur gauche sera au coordonnées starty + j * (height - 1) et startx + i * (width - 1) */
      		FGrille[i][j] = create_newwin(height, width, starty + j * (height - 1), startx + i * (width - 1));
    	}
  	}
	display_game(g);
	return g;
}


void init_score(){
	int startx, starty, width, height;
	/* hauteur et largeur de la fenetre */
  	height = 5;
  	width = 8;
  	/* positionnement de la grille dans la fenetre de terminal */
  	starty = (LINES + (GRID_SIDE-1) * height) / 2;
  	startx = (COLS  - 4 - GRID_SIDE * width) / 2;
  	/* création de la fenetre dans laquel sera écris le code*/
  	Status = create_newwin(height, GRID_SIDE * width +1, starty, startx);
}

int main(){
	/* initialise une fenetre src de la me taille que le terminal */
	initscr();
  	cbreak();
	/* équivalent au input */
	raw();
	/* pour éviter d'afficher le input */
	noecho();
	/* permet que le curseur ne clignote pas */
	curs_set(FALSE);
	/* récupéré les touche spécial (direction) */
	keypad(stdscr, TRUE);

  	printw("Press Fin to exit / Press R to reset / Press B to back last move \n");

  	refresh();

  	int key;
  	dir d;
  	/* initialisation et création de la grille */
  	grid g = initialize();

	/* création d'une nouvelle grille qui servira clone de la grille g afin de pour faire un back */
  	grid g2 = new_grid();
  	/* initialisation de la fentre pour le score */
  	init_score();
  	/* affichage du score */
  	display_score(g);

  	/* loop est ici afin de pouvoir revenir à se point lors de l'appelle de goto loop */
  	loop: do{
  		d = -1;
  		/* key sera égual à la touche appuyé */
    	key = getch();
    	switch (key){

    		/* déplacement à gauche */
    		case KEY_LEFT:
      			d = LEFT;
	  			break;

	  		/* déplacement en bas */
    		case KEY_UP:
      			d = DOWN;
	  			break;

	  		/* déplacement à droite */
    		case KEY_RIGHT:
      			d = RIGHT;
	  			break;

	  		/* déplacement en haut */
   	 		case KEY_DOWN:
    	 		d = UP;
	  			break;

	  		/* reset de la partie */
    		case 'r':
    			/* détruis la grille */
    			delete_grid(g);
    			/* détruis la copie de la grille */
    			delete_grid(g2);
    			/* nétoie de tout affichage */
    			destroy_FGrille();
    			/* réinitialise la grille */
    			g = initialize();
    			/* recréé une grille vide qui servira de copie */
    			g2 = new_grid();
    			/* détruit la fenetre de score */
    			destroy_Status();
    			/* réinitialise la fenetre de score */
    			init_score();
    			break;

    		/* retour à la grille avant le dernier mouvement */
    		case 'b':
    			/* copie la grille g2 dans g */
    			copy_grid(g2, g);
    			/* permet de récupéré les nouvelles infos de la grille et du score */
    			display_game(g);
    			display_score(g);
    			break;

    		/* quitter le jeu */
        	case KEY_END:
          		d = KEY_END;
          		break;

    	}

    	/* copie de la grille g dans la grille g après chaque mouvement */
    	copy_grid(g, g2);

    	/* si les conditions sont replies alors il peut y avoir déplacement */
    	if(d!=KEY_END && d!=-1 && can_move(g,d)){
    		play(g, d);
    		/* afin de modifier l'affichage dans la grille */
    		display_game(g);
    		/* modifie le score après mouvement */
    		display_score(g);
    	}
    /* on sort de la boucle si il y a game over ou que le joueur clic sur F5 */
  	}while(!game_over(g) && d!=KEY_END);

  	/*supprime la fenetre de score */
    destroy_Status();
    /* réinitialise la même fenetre que celle pour le score */
    init_score();
    /* mais cette fois si on écris GAME OVER à l'intérieur */
    mvwprintw(Status, 2, 2, "	GAME OVER 	");
    /* raffraichi l'affichage de la fenetre Status */
    wrefresh(Status);

    
    /* le programme est mis en pause et atend que le joueur clique sur une touche */
    key = getch();

    /* si la touche cliqué est R alors on relance le jeu et on retourne au début de la boucle grace a goto loop */
    if (key == 'r'){
    	/* détruit les 2 grille */
    	delete_grid(g);
    	delete_grid(g2);
    	/* supprime les fenetres créé avec ncurses */
    	destroy_FGrille();
    	destroy_Status();
    	/* réinitialise la grille g et g2 */
    	g = initialize();
    	g2 = new_grid();
    	/*réinitialise la fenetre de score */
    	init_score();
    	/* permet de venir à la boucle do ... while afin de pouvoir rejouer malgré la sortie de la boucle car GAME OVER */
    	goto loop;
    }
    
    /* si n'importe quel autre touche est cliquer alors on sort du jeu, toute les fenetres sont supprimer et on retourne au terminal */
    else{
    	/* détruit les 2 grille */
    	delete_grid(g);
    	delete_grid(g2);
    	/* supprime les fenetres créé avec ncurses */
    	destroy_FGrille();
    	destroy_Status();
    	/* permet de sortir de mode ncurses */
		endwin();
		return EXIT_SUCCESS;
	}
}

WINDOW *create_newwin(int height, int width, int starty, int startx){ 	
	WINDOW *local_win;
	/* créé un fenetre de hauteur height, de largeur width, et donc l'angle superieur gauche est au coordonné starty et startx */
  	local_win = newwin(height, width, starty, startx);
	/* paramètre les bords de chaque fenetre ncurses */
  	wborder(local_win, '|', '|', '-', '-', '+', '+', '+', '+');
  	/* affiche la dernière version de la fenetre */
  	wrefresh(local_win);
  	return local_win;
}

void display_game(grid g){
  	int i, j;
  	for(i=0; i<GRID_SIDE; i++){
    	for(j=(GRID_SIDE-1); j>=0; j--){
      		if(get_tile(g,i,j) == 0){ 
      			/* si la case de la grille contient un zero on affichera du vide */
        		mvwprintw(FGrille[i][j], 2, 2, "    " );
      		}
      		else { 
      			/* permet d'afficher la valeur dans la fenetre contenue dans la case  correspondante dans la grille */
        		int m = pow(2, get_tile(g, i, j));
        		mvwprintw(FGrille[i][j], 2, 2, "%4d", m); 
      		}
      	/* affiche la derniere version des fenetre */
      	wrefresh(FGrille[i][j]);
    	}
  	}
}

/* permet l'affichage du score dans la fenetre aproprié */
void display_score(grid g){
	/* écris la string dans la fenetre Status en laissant un espace de 2 char au début et à la fin */
  	mvwprintw(Status, 2, 2, "Score of the game : %ld pts", grid_score(g));
  	/* rafraichie l'affichage de Status */
  	wrefresh(Status);
}

/* permet de néttoyer et détruire les fenetres représentant la grille */
void destroy_FGrille(){
  	int i, j;
  	for(i=0; i<GRID_SIDE; i++){
    	for(j=(GRID_SIDE-1); j>=0; j--){
    		/* nétoie tous les contours de la grille */
   			wborder(FGrille[i][j], ' ', ' ', ' ',' ',' ',' ',' ',' ');
   			/* rafraichis l'affichage de FGrille */
      		wrefresh(FGrille[i][j]);
      		/* supprime toutes les fenetres */
      		delwin(FGrille[i][j]);
    	}
  	}
}

/* permet de néttoyer et détruire les fenetres représentant la grille */
void destroy_Status(){
	wborder(Status, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(Status);
	delwin(Status);
}
