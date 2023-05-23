/* 
 * player.c - CS50 'player' module
 *
 * see player.h for more information.
 *
 * CecsC, 2023
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"

/**************** global types ****************/
typedef struct player {
  grid_t* grid;       // personal map of what they can see
  char c;               //what character they are
  char* name;           //what they say their name is
  int score;            //current score
  int x;                //location
  int y;                //location
  bool active;          //still in or has quit?
} player_t;

player_t* player_new(char c, char* name) {                               //note: should they get a grid loaded in?

    player_t* player = mem_mallocc(sizeof(player_t));
    player->c = c;
    player->name = name;
    player->score = 0;
    player->x = 0;
    player->y = 0;
    player->active = true;                                                      //note should not start active yet?
    
}

void player_set_map(player_t* player, char* map) {

  player->grid = grid_new(NR, NC)                                     //note: should these get passed in or dimensions inferred?
  grid_load(map);

}


void player_delete(player_t* player) {

  if (player->grid != NULL) {
    grid_delete(player->grid);
  }
  if (player != NULL) {
    mem_free(player);
  }
}

void player_move(player_t* player, player_t* mover, char keystroke) {

  //if valid keystroke {

    if (player->c == mover->c) {

      //move 'c' character based on game logic + walls
      grid_move(player->grid, player_t* mover, char keystroke)                //note: not written yet
      grid_update_visibility(player->grid, player_t* player)                  //note: not written yet

    } else {
      grid_move(player->grid, player_t* mover, char keystroke)                //note: not written yet

      
    }
    

  //}

}

/*

if valid keystroke
  if player != mover
      modify where mover character is on player's grid based on keystroke
  if player == mover
	  modify where '@' symbol is on player's grid based on keystroke
	update visibility based on movement
  
*/

/***** GETTER / SETTER FUNCTIONS *****/
grid_t* grid;       // personal map of what they can see
char c;               //what character they are
char* name;           //what they say their name is
int score;            //current score
int x;                //location
int y;                //location
bool active;          //still in or has quit?

grid_t* player_get_grid(player_t* player) {

  if (player != NULL) {
    return player->grid;
  }
}

char player_get_c(player_t* player) {

  if (player != NULL) {
    return player->c;
  }
}

char* player_get_name(player_t* player) {

  if (player != NULL) {
    return player->name;
  }
}

int player_get_score(player_t* player) {

  if (player != NULL) {
    return player->score;
  }
}

int player_get_x(player_t* player) {

  if (player != NULL) {
    return player->x;
  }
}

int player_get_y(player_t* player) {

  if (player != NULL) {
    return player->y;
  }
}

bool player_is_active(player_t* player) {

  if (player != NULL) {
    return player->active;
  }
}




void player_set_c(player_t* player, char c) {

  if (player != NULL && c != NULL) {
    player->c = c;
  }
}

void player_set_name(player_t* player, char* name) {

  if (player != NULL && name != NULL) {
    player->name = name;
  }
}


void player_set_score(player_t* player, int score) {

  if (player != NULL) {
    player->score = score;
  }
}


void player_set_x(player_t* player, int x) {

  if (player != NULL) {
    player->x = x;
  }
}


void player_set_y(player_t* player, int y) {

  if (player != NULL) {
    player->y = y;
  }
}


void player_deactivate(player_t* player) {

  if (player->active == true) {
    active = false;
    return;
  }
}