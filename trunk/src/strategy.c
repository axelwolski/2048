#include "grid.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


/*typedef struct strategy_s
{
	void* mem;
	char* name;
	dir (*play_move) (strategy, grid);
	void* (free_strategy)(strategy);
} strategy;*/

int evaluate_grid(grid g)
{
	if (g == NULL)
		return 0;

	double value = 0;
	bool full_line = true;
	int max   = 0;
	
	for (int i = 0; i < GRID_SIDE; i++)
		for (int j = 0; j < GRID_SIDE; j++)
		{
			// Biggest tile.
			if (get_tile(g, i, j) >= max)
			{
				max = get_tile(g, i, j);
			}
			// 0-tiles
			if (get_tile(g, i, j) == 0)
				value += 2;
		}

	// Bottom line;
	for (int i = 0; i < GRID_SIDE-1; i++)
	{
		if (get_tile(g, i, 0) == get_tile(g, i+1, 0))
			value += 5 ;
		else
		{
			if (abs(get_tile(g, i, 0) - get_tile(g, i+1, 0) <= 3))
				value += 2;
			if (get_tile(g, i, 0) - get_tile(g, i, 1) >= 0 && get_tile(g, i, 0) - get_tile(g, i, 01) <= 1)
				value += 2;
		}
	
	
	}

	if (get_tile(g, GRID_SIDE-1, 0) == max)
	{
		value += 10;  
		if (get_tile(g, GRID_SIDE-2, 0) == get_tile(g, GRID_SIDE-1, 0))
			value += 6;
		if (get_tile(g, GRID_SIDE-1, 0) == get_tile(g, GRID_SIDE-1, 1))
			value += 2;
		if (get_tile(g, GRID_SIDE-1, 1) > 0)
			value += 1;
		if (get_tile(g, GRID_SIDE-2, 0) == 0)
			value += 3;
		
	}

	if (full_line)
		value++;

	return floor(value);
}

dir play_move(grid g)
{
  //Find the max
  int maxi = 0;
  for (int i = 0; i < GRID_SIDE; i++)
    for (int j = 0; j < GRID_SIDE; j++)
      	if (get_tile(g, i, j) > maxi)
	  maxi = get_tile(g, i, j);
  
  if (get_tile(g, GRID_SIDE-1, 0) == maxi)
    {
      for (int k = GRID_SIDE-1; k > 0; k--)
      {
	
	if (get_tile(g, k, 0) == get_tile(g, k, 1) || (get_tile(g, k, 0) == (get_tile(g,k, 1) + 1) && get_tile(g, k, 1) == get_tile(g, k, 2)))
	  {
	    if(can_move(g, DOWN))
	       return DOWN;
	  }
	else
	  if (get_tile(g, k, 0) == get_tile(g, k-1, 0) || (get_tile(g, k, 0) == (get_tile(g, k, 1) + 1) && (get_tile(g, k,1) == get_tile(g, k-1, 1) || get_tile(g, k, 0) == get_tile(g, k-1, 1))))
	    {
	      if (can_move(g, RIGHT))
		return RIGHT;
	    }
      }
    }      
	
  grid tab[] =  {new_grid(g), new_grid(g), new_grid(g)};
  bool up = true; 
  for (int i = 0; i < 3; i++)
	{
	  copy_grid(g, tab[i]);
	  if (can_move(tab[i], i+1))
	  {
		do_move(tab[i], i+1);
		up = false;
	  }
	  else
		tab[i] = NULL;
	}

 if (up)
   return 0;

 int dir = 0;
 int max  = 0;
 
 for (int i = 0; i < 3; i++)
   {
     int value = evaluate_grid(tab[i]);
     if (tab[i] != NULL)
       delete_grid(tab[i]);

     if (value > max)
     {
     	max = value;
     	dir = i+1;
     }

   }
 return dir;
}

void libA3_lugan_guessoum_wolski()
{
	grid g = new_grid();
	add_tile(g);
	add_tile(g);

	for (int i = 0; i <= 5; i++)
	{
		play(g, DOWN);
		play(g, RIGHT);
	}
	
	while (!game_over(g))
		play(g, play_move(g));

	printf("%lu \n", grid_score(g));
	delete_grid(g);
}

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

int main()
{
	grid g = new_grid();
	add_tile(g);
	add_tile(g);

	for (int i = 0; i <= 5; i++)
	{
		play(g, DOWN);
		play(g, RIGHT);
	}
	
	while (!game_over(g))	{
		print_grid(g);
		printf("\n");
		play(g, play_move(g));
	}

	printf("%lu \n", grid_score(g));
	delete_grid(g);
}
