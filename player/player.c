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
  bool* boolGrid;       // personal map of what they can see
  char c;               //what character they are
  char* name;           //what they say their name is
  int score;            //current score
  int x;                //location
  int y;                //location
  bool active;          //still in or has quit?
  const addr_t addr;    //address of client corresponding to player
} player_t;

player_t* player_new(char c, char* name, const addr_t addr, int NR, int NC) {                               //note: should they get a grid loaded in?

    player_t* player = mem_mallocc(sizeof(player_t));

    player->boolGrid = mem_malloc(sizeof(bool) * (NR * NC));  
    for (int i = 0; i < (NR * NC; i++)) {
    boolGrid[i] = false;
    }

    player->c = c;
    player->name = name;
    player->score = 0;
    player->x = 0;
    player->y = 0;
    player->active = true;                                                      //note should not start active yet?
    player->addr = addr;
}


void player_delete(player_t* player) {

  if (player->boolGrid != NULL) {
    mem_free(boolGrid);
  }
  if (player != NULL) {
    mem_free(player);
  }
}


/***** GETTER / SETTER FUNCTIONS *****/
addr_t player_get_addr(player_t* player) {

  if (player != NULL && player->addr != NULL) {
    return player->addr;
  } else {
    fprintf(stderr, "player or player address is null\n");
  }
}


grid_t* player_get_boolGrid(player_t* player, int index) {

  if (player != NULL && player->boolGrid != NULL) {
    return player->boolGrid[index];
  } else {
    fprintf(stderr, "player or player boolgrid is null\n");
  }
}

char player_get_c(player_t* player) {

  if (player != NULL && player->c != NULL) {
    return player->c;
  } else {
    fprintf(stderr, "player or player c is null\n");
  }
}

char* player_get_name(player_t* player) {

  if (player != NULL && player->name != NULL) {
    return player->name;
  } else {
    fprintf(stderr, "player or player name is null\n");
  }
}

int player_get_score(player_t* player) {

  if (player != NULL) {
    return player->score;
  } else {
    fprintf(stderr, "player or player score is null\n");
  }
}

int player_get_x(player_t* player) {

  if (player != NULL) {
    return player->x;
  } else {
    fprintf(stderr, "player or player x coord is null\n");
  }
}

int player_get_y(player_t* player) {

  if (player != NULL) {
    return player->y;
  } else {
    fprintf(stderr, "player or player y coord is null\n");
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

void player_set_boolGrid(player_t* player, int index, bool visible) {

  if (player != NULL) {
    player->boolGrid[index] = visible;
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