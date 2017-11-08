#include <assert.h>
#include <grid.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct coordinate
{
	unsigned int x;
	unsigned int y;
} coordinate;

typedef tile tiles_array[GRID_SIDE];

struct grid_s
{
	tiles_array *g;             // The actual grid. 
	coordinate origin;         // Will contain the coordinates of the grid's coordinates that can_move will set for do_move to pick up from. 
	unsigned int nbv;         // Number of the grid's vacant spots, useful to keep the game_over function to a minimum of operations. 
	unsigned long int score; // The grid's score. 
};

grid new_grid()
{
	grid gd    = malloc(sizeof(struct grid_s));
	assert(gd != NULL);
	gd->g      = malloc(GRID_SIDE*sizeof(tiles_array));
	assert(gd->g != NULL);
	gd->score     = 0;
	gd->nbv       = GRID_SIDE*GRID_SIDE;
	for (unsigned int i = 0; i<GRID_SIDE*GRID_SIDE; i++)
		gd->g[i/GRID_SIDE][i%GRID_SIDE] = 0;
	return gd;
}

void delete_grid(grid g)
{
	free(g->g);
	free(g);
}

void copy_grid(grid src, grid dst)
{
	dst->score = src->score;
	dst->nbv   = src->nbv;
	memcpy(dst->g, src->g, GRID_SIDE*sizeof(tiles_array));
}

unsigned long int grid_score(grid g)
{
	return g->score;
}

tile get_tile(grid g, int x, int y)
{
	x = abs(x);
	y = abs(y);
	assert(x < GRID_SIDE && y < GRID_SIDE);
	return g->g[x][y];
}

void set_tile(grid g, int x, int y, tile t)
{
	x = abs(x);
	y = abs(y);
	assert(x < GRID_SIDE && y < GRID_SIDE);
	g->g[x][y] = t;
}

bool can_move(grid g, dir d)
{
	// Setting the origin's coordiantes as well the directions according to the requested move test.	
	unsigned int x = d == DOWN || d == LEFT  ? 0 : GRID_SIDE-1;
	unsigned int y = d == DOWN || d == RIGHT ? 0 : GRID_SIDE-1;	
	int dir_x      = x == 0 ? 1 : -1;
	int dir_y      = y == 0 ? 1 : -1;	
		
	if (dir_x == dir_y)
	{
		unsigned int tmp_y = y;
		unsigned int thr_y = y == 0 ? GRID_SIDE-1 : 0;
		for (; x >= 0 && x < GRID_SIDE; x += dir_x)
			for (y = tmp_y; y != thr_y; y += dir_y)	
				if (get_tile(g, x, y+dir_y) != 0 && (get_tile(g, x, y) == get_tile(g, x, y+dir_y) || get_tile(g, x, y) == 0))
				{	
					g->origin.x = x;
					return true;
				}
	}					
	else
	{
		unsigned int tmp_x = x;
		unsigned int thr_x = x == 0 ? GRID_SIDE-1 : 0;
		for (; y >= 0 && y < GRID_SIDE; y += dir_y)
			for (x = tmp_x; x != thr_x; x += dir_x)	
				if (get_tile(g, x+dir_x, y) != 0 && (get_tile(g, x, y) == get_tile(g, x+dir_x, y) || get_tile(g, x, y) == 0))
				{	
					g->origin.y = y;
					return true;
				}
	}
	return false;
}

static void vertical_move(grid g, dir d)
{
	/* y = 0 or 3.
		The possible cases divide into two, one of which also divides into two, as follows:
		I/ grid[x][y] > 0:
			a) 	grid[x][y + dir_y] > 0: If grid[x][y + dir_y] == grid[x][y], then grid[x][y] += 1,
				set grid[x][y + dir_y] to 0 and y += dir_y. If not, y += dir_y;
			b) grid[x][y + dir_y] == 0: We look through the yth column for the first non-zero element. 
				If it exists, then we move it to grid[x][y + dir_y] and y += dir_y. If not, break (move to the next column);
		II/ grid[x][y] == 0: We look through the yth column for the first non-zero element. 
			If it exists, then we move it to grid[x][y + dir_y]. If not, break (move to the next column); */

	// Setting the origin's coordiantes as well the directions according to the requested move.
	unsigned int y;	
	unsigned int tmp_y = d == DOWN || d == RIGHT ? 0 : GRID_SIDE-1;
	unsigned int thr_y = tmp_y == 0 ? GRID_SIDE-1 : 0;
	int dir_x = d == DOWN || d == LEFT  ? 1 : -1;
	int dir_y = d == DOWN || d == RIGHT ? 1 : -1;
	
	
	for (unsigned int x = g->origin.x; x >= 0 && x < GRID_SIDE; x += dir_x)
	{
		y = tmp_y;
		while (y != thr_y)
			if (get_tile(g, x, y) > 0)
				if (get_tile(g, x, y+dir_y) > 0)
				{
					if (get_tile(g, x, y) == get_tile(g, x, y+dir_y))
					{
						g->score += pow(2, get_tile(g, x, y)+1);
						set_tile(g, x, y, get_tile(g, x, y)+1);
						set_tile(g, x, y+dir_y, 0);
					}
					y += dir_y;
				}
				else
				{
					unsigned int i = y+2*dir_y;
					while (i >= 0 && i < GRID_SIDE && get_tile(g, x, i) == 0)
						i += dir_y;
					if (i >= 0 && i < GRID_SIDE)
					{
						set_tile(g, x, y+dir_y, get_tile(g, x, i));
						set_tile(g, x, i, 0);
					}
					else
						break;
				}
			else
			{
				unsigned int i = y+dir_y;
				while (i >= 0 && i < GRID_SIDE && get_tile(g, x, i) == 0)
					i += dir_y;
				if (i >= 0 && i < GRID_SIDE)
				{
					set_tile(g, x, y, get_tile(g, x, i));
					set_tile(g, x, i, 0);
				}
				else
					break;
			}
	}		
}

static void horizontal_move(grid g, dir d)
{
	unsigned int x;	
	unsigned int tmp_x = d == DOWN || d == LEFT ? 0 : GRID_SIDE-1;
	unsigned int thr_x = tmp_x ? 0 : GRID_SIDE-1;
	int dir_x = d == DOWN || d == LEFT  ? 1 : -1;
	int dir_y = d == DOWN || d == RIGHT ? 1 : -1;

	for (unsigned int y = g->origin.y; y >= 0 && y < GRID_SIDE; y += dir_y)
	{
		x = tmp_x;
		while (x != thr_x)
			if (get_tile(g, x, y) > 0)
				if (get_tile(g, x+dir_x, y) > 0)
				{
					if (get_tile(g, x, y) == get_tile(g, x+dir_x, y))
					{
						g->score += pow(2, get_tile(g, x, y)+1); 
						set_tile(g, x, y, get_tile(g, x, y)+1);
						set_tile(g, x+dir_x, y, 0);
					}
					x += dir_x;
				}
				else
				{
					unsigned int i = x+2*dir_x;
					while (i >= 0 && i < GRID_SIDE && get_tile(g, i, y) == 0)
						i += dir_x;
					if (i >= 0 && i < GRID_SIDE)
					{
						set_tile(g, x+dir_x, y, get_tile(g, i, y));
						set_tile(g, i, y, 0);
					}
					else	
						break;
				}					
			else
			{
				unsigned int i = x+dir_x;
				while (i >= 0 && i < GRID_SIDE && get_tile(g, i, y) == 0)
					i += dir_x;
				if (i >= 0 && i < GRID_SIDE)
				{
					set_tile(g, x, y, get_tile(g, i, y));
					set_tile(g, i, y, 0);
				}
				else
					break;
			}
	}		
			
}

void do_move(grid g, dir d)
{
	d == UP || d == DOWN ? vertical_move(g, d) : horizontal_move(g, d);
}

void add_tile(grid g)
{
	static bool init_srand = false;
	coordinate v_spots[GRID_SIDE*GRID_SIDE]; // Array that will contain of all the vacant spots' coordinates, thus the candidates to host the new tile.
	g->nbv = 0;

	for (unsigned int i = 0; i < GRID_SIDE; i++)
		for (unsigned int j = 0; j < GRID_SIDE; j++)
			if (get_tile(g, i, j) == 0)
			{
				v_spots[g->nbv].x   = i;
				v_spots[g->nbv++].y = j;
			}
	if (!init_srand)
	{
		srand(time(NULL));
		init_srand = true;
	}
	if(g->nbv==0)
		return;
	unsigned int tmp = rand() % g->nbv--;
	set_tile(g, v_spots[tmp].x, v_spots[tmp].y, (rand()%10 > 0 ? 1 : 2));
}

bool game_over(grid g)
{
	return g->nbv == 0 && !(can_move(g, LEFT) || can_move(g, DOWN) || can_move(g, RIGHT) || can_move(g, UP)); // Takes advantage of C's lazy evaluation.
}

void play(grid g, dir d)
{
  if (can_move(g, d))
    {
	do_move(g, d);
	add_tile(g);
    }
}
