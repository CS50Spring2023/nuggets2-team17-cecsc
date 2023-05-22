/* 
 * spectator.c - CS50 'spectator' module
 *
 * see spectator.h for more information.
 *
 * CecsC, 2023
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"

/**************** global types ****************/
typedef struct spectator {
  grid_t* grid;       // personal map of what they can see
  bool active;          //still in or has quit?
} spectator_t;

spectator_t* spectator_new() {                               //note: should they get a grid loaded in?

    spectator_t* spectator = mem_mallocc(sizeof(spectator_t));
    active = true;                                                      //note should not start active yet?
    
}

void spectator_load_map(spectator_t* spectator, char* map) {

  spectator->grid = grid_new(NR, NC)                                     //note: should these get passed in or dimensions inferred?
  grid_load(map);

}

void spectator_deactivate(spectator_t* spectator) {

  if (spectator->active == true) {
    active = false;
    return;
  }
}

void spectator_delete(spectator_t* spectator) {

  if (spectator->grid != NULL) {
    grid_delete(spectator->grid);
  }
  if (spectator != NULL) {
    mem_free(spectator);
  }
}

void spectator_move(spectator_t* spectator, player_t* mover, char keystroke) {

  //if valid keystroke {
      grid_move(spectator->grid, player_t* mover, char keystroke)
  //}

}

