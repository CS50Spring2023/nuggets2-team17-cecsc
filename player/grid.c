/*
deals with the grid module, which is in charge of modifying and returning strings to print the correct map to the terminal

The right representation of a grid can make your code vastly simpler than it would be if you choose other representations. Consider:

What information must the spectator keep about the state of the game?
- all the players and their positions
-all the gold and its positions
- the entire map string, nothing hidden

What information must the player keep about the state of the game?
- all the players and their positions
-all the gold and its positions
- what's hidden and what's not

What information must the server keep about the state of the game?
- - all the players and their positions
-all the gold and its positions
- what's hidden and what's not

What information does the string below record about the state of the game?
- a string will only put out the literal characters in the map
-need a separate array of gridCell items to keep track of what's hidden, etc.

What information does the string below not record about the state of the game?
-need a separate array of gridCell items to keep track of what's hidden, etc.

How much gold is in each pile... and does any player know until they step on a pile?
-random sized piles, so they won't know until they step on it

Do these questions give you ideas about how to store game state?
-have to create gridCell object
-should have
    char c - literal character
    int gold - amount of gold in pile if it is gold pile
    bool show - character hidden from view or not?

char* grid = ...;   // a string variable
puts(grid);         // produces the output below
    puts(grid) will loop through gridCell array, if show, then put c, if !show, put blank space
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "grid.h"
#include "file.h"
#include "mem.h"
#include "gridcell.h"

/**************** file-local global variables ****************/
/* none */

/**************** global types ****************/
typedef struct grid {
  gridcell_t** gridarray;       // array of gridcells in the grid
  char* map;
  int NR;                       // number of rows
  int NC;                       // number of columns
} grid_t;


//display consists of NR+1, NC (to fit text header)                 //note: do i need to allocate here or within gridcell_new?
grid_t* grid_new(int NR, int NC) {
  // Allocate memory for the grid structure
  grid_t* grid = mem_assert(malloc(sizeof(grid_t)), "grid memory error");
  
  // Allocate memory for the rows of the gridarray
  grid->gridarray = mem_assert(malloc(NR * NC * sizeof(gridcell_t*)), "gridarray memory error");

  // Return the initialized grid
  return grid;
}



//loads given file into grid
void grid_load(grid_t* grid, char* pathName) 
{
  // check arguments
  if (grid == NULL || pathName == NULL) {
      fprintf(stderr, "Null grid or Pathname");
      return;
  }

  // open file, check if worked
  FILE* fp;
  fp = fopen(pathName, "r");
  if (fp == NULL) {
    fprintf(stderr, "Failed to open file: %s\n", pathName);
    return;
  }
   
  // get number of columns
  char* lineOne = file_readLine(fp);
  int numCols = strlen(lineOne);
  grid->NC = numCols;
  rewind(fp);

  //get number of rows
  int numRows = file_numLines(fp);
  grid->NR = numRows;

  // for each character, concatenate to end of string with strncat
  char* map = malloc(numRows*numCols + 1); // string of all the characters in the map
  char* line;
  for (int i = 0; (line = file_readLine(fp)) != NULL; i++) {
    for (int j = 0; j < numCols; j++) {
      char c = line[j];
      strncat(map, &c, 1);
    }
  }
  map[numRows*numCols] = '\0'; // add null character to the end

  printf("%s\n", map);
  printf("%d\n%d\n", grid->NR, grid->NC);

  fclose(fp);


}

void grid_set(grid_t* grid, int x, int y, char c)
{
  if (grid == NULL || x < 0 || y < 0) {
    fprintf(stderr, "One or more grid_set args is null");
  }

  int idx = grid->NC * (y-1) + x - 1;

  gridcell_set(grid->gridarray[idx], c);
}

void grid_delete(grid_t* grid)
 {
  int arraySize = grid->NR * grid->NC;   // calculate size of gridarray

  for (int i = 0; i < arraySize; i++) {
    gridcell_delete(grid->gridarray[i]);
  }

  free(grid->gridarray);
  free(grid);
 }

 gridcell_t* grid_get_cell(grid_t* grid, int x, int y) {

  gridcell_t* gridcell = (grid->gridarray)[((grid->NC * y) + x)];

  if (gridcell == NULL) {
    fprintf(stderr, "gridcell is null for grid_get_cell\n");
  } 

  return gridcell;
 }

/******set of all grid_move help functions**********/

int grid_move_left(grid_t* grid, player_t* mover, int* goldLeft) {

  //get information about mover's current place
  int x = player_get_x(mover);
  int y = player_get_y(mover);
  gridcell_t* currCell = grid_get_cell(grid, x, y);
  char c = gridcell_getC(currCell);

  if (c == NULL) {
    fprintf(stderr, "something wrong with current character in grid_move\n");
  }

  //move left
  gridcell_t* leftCell = grid_get_cell(grid, x-1, y);
  char l = gridcell_getC(leftCell)
  //can't move left
  if (l == '|' || l == NULL || l == '_' || l == '+') {
    return 1;
  } else {                  //either a room or hallway space OR GOLD to the left, CAN move

    //gold?
    if (l == '*') {                         //NOTE: need getter and setter for gold in gridcell
      int g = gridcell_getGold(leftCell);
      if (g > 0) {
        int currScore = player_get_score(mover);
        player_set_score(mover, (currScore + g));
        gridcell_setGold(leftCell, 0);
        int newGold = *goldLeft - g;
        goldLeft = &newGold;
      } else {
        fprintf(stderr, "score of zero gold found where * is\n");
      }
    }
    gridcell_set(leftCell, player_get_c(mover));
    
    if (leftCell->room) {
      gridcell_set(currCell, '.');
    } else {
      gridcell_set(currCell, '#');
    }
    player_set_x(mover, (x-1));
    return 0;
  }
}



int grid_move_right(grid_t* grid, player_t* mover, int* goldLeft) {

  //get information about mover's current place
  int x = player_get_x(mover);
  int y = player_get_y(mover);
  gridcell_t* currCell = grid_get_cell(grid, x, y);
  char c = gridcell_getC(currCell);

  if (c == NULL) {
    fprintf(stderr, "something wrong with current character in grid_move\n");
  }

  //move right
  gridcell_t* rightCell = grid_get_cell(grid, x+1, y);
  char r = gridcell_getC(rightCell)
  //can't move right
  if (r == '|' || r == NULL || r == '_' || r == '+') {
    return 1;
  } else {                  //either a room or hallway space to the right, CAN move

    gridcell_set(rightCell, player_get_c(mover));
    if (currCell->room) {
      gridcell_set(currCell, '.');
    } else {
      gridcell_set(currCell, '#');
    }
    player_set_x(mover, (x+1));
    return 0;
  }
}

int grid_move_up(grid_t* grid, player_t* mover, int* goldLeft) {

  //get information about mover's current place
  int x = player_get_x(mover);
  int y = player_get_y(mover);
  gridcell_t* currCell = grid_get_cell(grid, x, y);
  char c = gridcell_getC(currCell);

  if (c == NULL) {
    fprintf(stderr, "something wrong with current character in grid_move\n");
  }

  //move up
  gridcell_t* upCell = grid_get_cell(grid, x, y-1);
  char u = gridcell_getC(upCell)
  //can't move right
  if (u == '|' || u == NULL || u == '_' || u == '+') {
    return 1;
  } else {                  //either a room or hallway space to the above, CAN move

    gridcell_set(upCell, player_get_c(mover));
    if (currCell->room) {
      gridcell_set(currCell, '.');
    } else {
      gridcell_set(currCell, '#');
    }
    player_set_y(mover, (y-1));
    return 0;
  }
    

}

int grid_move_down(grid_t* grid, player_t* mover, int* goldLeft) {

  //get information about mover's current place
  int x = player_get_x(mover);
  int y = player_get_y(mover);
  gridcell_t* currCell = grid_get_cell(grid, x, y);
  char c = gridcell_getC(currCell);

  if (c == NULL) {
    fprintf(stderr, "something wrong with current character in grid_move\n");
  }

  //move down
  gridcell_t* downCell = grid_get_cell(grid, x, y+1);
  char d = gridcell_getC(downCell)
  //can't move right
  if (d == '|' || d == NULL || d == '_' || d == '+') {
    return 1;
  } else {                  //either a room or hallway space to the below, CAN move

    gridcell_set(downCell, player_get_c(mover));
    if (currCell->room) {
      gridcell_set(currCell, '.');
    } else {
      gridcell_set(currCell, '#');
    }
    player_set_y(mover, (y+1));
    return 0;
  }
}

int grid_move_ul(grid_t* grid, player_t* mover, int* goldLeft) {

  //get information about mover's current place
  int x = player_get_x(mover);
  int y = player_get_y(mover);
  gridcell_t* currCell = grid_get_cell(grid, x, y);
  char c = gridcell_getC(currCell);

  if (c == NULL) {
    fprintf(stderr, "something wrong with current character in grid_move\n");
  }

  gridcell_t* ulCell = grid_get_cell(grid, x-1, y-1);
  char ul = gridcell_getC(ulCell)
  //can't move right
  if (ul == '|' || ul == NULL || ul == '_' || ul == '+') {
    return 1;
  } else {                  //either a room or hallway space to the above, CAN move

    gridcell_set(ulCell, player_get_c(mover));
    if (currCell->room) {
      gridcell_set(currCell, '.');
    } else {
      gridcell_set(currCell, '#');
    }
    player_set_x(mover, (x-1));
    player_set_y(mover, (y-1));
    return 0;
  }
}


int grid_move_ur(grid_t* grid, player_t* mover, int* goldLeft) {

  //get information about mover's current place
  int x = player_get_x(mover);
  int y = player_get_y(mover);
  gridcell_t* currCell = grid_get_cell(grid, x, y);
  char c = gridcell_getC(currCell);

  if (c == NULL) {
    fprintf(stderr, "something wrong with current character in grid_move\n");
  }

  gridcell_t* urCell = grid_get_cell(grid, x+1, y-1);
  char ur = gridcell_getC(urCell)
  //can't move right
  if (ur == '|' || ur == NULL || ur == '_' || ur == '+') {
    return 1;
  } else {                  //either a room or hallway space to the above, CAN move

    gridcell_set(urCell, player_get_c(mover));
    if (currCell->room) {
      gridcell_set(currCell, '.');
    } else {
      gridcell_set(currCell, '#');
    }
    player_set_x(mover, (x+1));
    player_set_y(mover, (y-1));
    return 0;
  }
}


int grid_move_dl(grid_t* grid, player_t* mover, int* goldLeft) {

  //get information about mover's current place
  int x = player_get_x(mover);
  int y = player_get_y(mover);
  gridcell_t* currCell = grid_get_cell(grid, x, y);
  char c = gridcell_getC(currCell);

  if (c == NULL) {
    fprintf(stderr, "something wrong with current character in grid_move\n");
  }

  gridcell_t* dlCell = grid_get_cell(grid, x-1, y+1);
  char dl = gridcell_getC(dlCell)
  //can't move right
  if (dl == '|' || dl == NULL || dl == '_' || dl == '+') {
    return 1;
  } else {                  //either a room or hallway space to the above, CAN move

    gridcell_set(dlCell, player_get_c(mover));
    if (currCell->room) {
      gridcell_set(currCell, '.');
    } else {
      gridcell_set(currCell, '#');
    }
    player_set_x(mover, (x-1));
    player_set_y(mover, (y+1));
    return 0;
  }
}

//moves character down and right
int grid_move_dr(grid_t* grid, player_t* mover, int* goldLeft) {

  //get information about mover's current place
  int x = player_get_x(mover);
  int y = player_get_y(mover);
  gridcell_t* currCell = grid_get_cell(grid, x, y);
  char c = gridcell_getC(currCell);

  if (c == NULL) {
    fprintf(stderr, "something wrong with current character in grid_move\n");
  }

  gridcell_t* drCell = grid_get_cell(grid, x+1, y+1);
  char dr = gridcell_getC(drCell)
  //can't move right
  if (dr == '|' || dr == NULL || dr == '_' || dr == '+') {
    return 1;
  } else {                  //either a room or hallway space to the above, CAN move

    gridcell_set(drCell, player_get_c(mover));
    if (currCell->room) {
      gridcell_set(currCell, '.');
    } else {
      gridcell_set(currCell, '#');
    }
    player_set_x(mover, (x+1));
    player_set_y(mover, (y+1));
    return 0;
  }
}


/********main function that moves characters on grid************/
 void grid_move(grid_t* grid, player_t* mover, char key, int* goldLeft) {

  //move left                 NOTE: do we have to check for x boundary or will it always be wthin wall
  if (key == 'h') {
    int l = grid_move_left(grid, mover, goldLeft);
  }

  //move right                
  if (key == 'l') {
    int r = grid_move_right(grid, mover, goldLeft);
  }


  //move up                 
  if (key == 'k') {
    int u = grid_move_up(grid, mover, goldLeft);
  }


  //move down                 
  if (key == 'j') {
    int d = grid_move_down(grid, mover);
  }

  //move up and left               
  if (key == 'y') {
    int ul = grid_move_ul(grid, mover, goldLeft);
  }


  //move up and right               
  if (key == 'u') {
    int ur = grid_move_ur(grid, mover, goldLeft);
  }



  //move down and left               
  if (key == 'b') {
    int dl = grid_move_dl(grid, mover, goldLeft);
  }


  //move down and right               
  if (key == 'n') {
    int dr = grid_move_dr(grid, mover, goldLeft);
  }

  //skip left
  if (key == 'H') {
    int L = 0;
    while (L < 1) {
      L = grid_move_left(grid, mover, goldLeft);
    }
  }
  //skip right                
  if (key == 'L') {
    int R = 0;
    while (R < 1) {
      R = grid_move_right(grid, mover, goldLeft);
    }
  }


  //move up                 
  if (key == 'K') {
    int U = 0;
    while (U < 1) {
      U = grid_move_up(grid, mover, goldLeft);
    }  
  }


  //skip down                 
  if (key == 'J') {
    int D = 0;
    while (D < 1) {
      D = grid_move_down(grid, mover, goldLeft);
    }  
  }

  //skip up and left               
  if (key == 'Y') {
    int UL = 0;
    while (UL < 1) {
      UL = grid_move_ul(grid, mover, goldLeft);
    }  
  }


  //skip up and right               
  if (key == 'U') {
    int UR = 0;
    while (UR < 1) {
      UR = grid_move_ur(grid, mover, goldLeft);
    }  
  }



  //skip down and left               
  if (key == 'B') {
    int DL = 0;
    while (DL < 1) {
      DL = grid_move_dl(grid, mover, goldLeft);
    }  
  }


  //skip down and right               
  if (key == 'N') {
    int DR = 0;
    while (DR < 1) {
      DR = grid_move_dr(grid, mover, goldLeft);
    }  
  }
  
 }

//gives the map string to print given a grid
void grid_update_map(grid_t* grid) {
  int totalCells = (grid->NC) * (grid->NR);
  mem_free(grid->map);
  grid->map = (char*) mem_malloc(sizeof(char) * (totalCells + NR));
  int index = 0;
  for (int i = 0; i < totalCells; i++) {

    gridcell_t* cell = grid->gridarray[i];
    char c = gridcell_getC(cell);
    grid->map[index] = name;
    index++;

    //at end of row?
    if (((i+1) % NC) == 0) {
      grid->map[index] = '\n';
      index++;
    }

  }
}


