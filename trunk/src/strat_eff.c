#include <strategy.h>
#include <grid.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <ncurses.h>
#include <math.h>


typedef struct memory_s *memory;

dir play_move(strategy, grid);
/* Fonction qui fait la decisionn de la direction*/
void free_strategy(strategy);
/* Libere les memoires des strats*/
memory create_memory();
/* Creation de la memoire  */
void save(memory, grid, dir);
/* Sauvegarde la direction choisi et la grille actuelle */
void free_memory(memory);
/* Libere la memoire*/
strategy A3_Lugan_Guessoum_Wolski_effic();

void print_grid(grid g)
{
  
  for (int i = 3; i >= 0; i--)
    {
      for (int j=0; j< GRID_SIDE; j++){
	int m=pow(2,get_tile(g,j,i));
	printf("%d | ",m);
      }
      printf("\n");
    }
}
	   

typedef struct tour_s *tour;

struct tour_s
{
  grid g; 
  dir decision; // Direction prise 
};

struct memory_s
{
  int nb;
  int max;
  tour* stock;
};
  

strategy A3_Lugan_Guessoum_Wolski_effic()
{
  srand(time(NULL));
  strategy test = malloc(sizeof(struct strategy_s));
  test->mem = create_memory();
  test->name = "Strategy Effi";
  test->play_move = &(play_move);
  test->free_strategy = &(free_strategy);
  return test;
}

dir play_move(strategy s, grid g)
{
  if(can_move(g, DOWN))
    {
      save(s->mem, g, DOWN);
      print_grid(g);
      return DOWN;
    }
  if(can_move(g, LEFT))
    {
      save(s->mem, g, LEFT);
      print_grid(g);
      return LEFT;
    }
  if(can_move(g, RIGHT))
    {
      save(s->mem, g, RIGHT);
      print_grid(g);
      return RIGHT;
    }
  if(can_move(g, UP))
    {
      save(s->mem, g, UP);
      print_grid(g);
      return UP;
    }
    }


void free_strategy(strategy s)
{
  free(s);
}

memory create_memory()
{
  memory new = malloc(sizeof(struct memory_s));
  tour* stock = malloc(sizeof(struct tour_s) * 100000);
  if(stock != NULL) // Verification du malloc
    {
      new->stock = stock;
      new->max = 100000;
      new->nb = 0;
    }
}
 
void save(memory mem, grid g, dir d)
{
  tour newStage = malloc(sizeof(struct tour_s));
  if(newStage != NULL) // verification du malloc
    {
      grid gridToSave = new_grid();
      copy_grid(g, gridToSave);
      newStage->g = gridToSave;
      newStage->decision = d;
      if(mem->nb < mem->max)
	{
	  (mem->stock)[mem->nb] = newStage;
	  (mem->nb)++;
	}
    }
}

void free_memory(memory mem)
{
  int i;
  for(i = 0; i < mem->nb; i++)
    {
      delete_grid(((mem->stock)[i])->g);
      free((mem->stock)[i]);
    }
  for(i = mem->nb; i < mem->max; i++)
    {
      free((mem->stock)[i]);
    }
  free(mem);
}


int main ()
{
  grid g=new_grid();
  add_tile(g);
  add_tile(g);
  strategy s = A3_Lugan_Guessoum_Wolski_effic();
 
  
  while (!game_over(g))
    {
      play(g, play_move(s,g));
      printf("\n");
		
		
    }
  if (game_over(g)){
    printf("game over \n");
      }
  
  printf("%lu \n", grid_score(g));
 

}
