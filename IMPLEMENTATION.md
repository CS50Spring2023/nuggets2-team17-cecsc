# CS50 Nuggets
## Implementation Spec
### CecsC (Team 17), Spring, 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our implementation also includes `player`, `grid`, and `gridcell` modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

We have 4 group members, Nikhil, Andy, Kyrylo, and Logan. 

Nikhil is responsible for the grid and gridcell modules, focusing on the visibility aspect of the game. Logan is responsible for the player module, and to assist Nikhil with the grid module. Kyrylo will be responsible for the client, and Andy will be in charge of designing the server.

Nikhil, Andy, and Kyrylo will be testing while Logan works on the support and documentation.

## Player

### Data structures

bool* boolGrid
boolGrid is a 1d array of booleans aligning with the main map, where each boolean is true if the corresponding character on the map has been seen by the player, false otherwise

struct grid_t
Grid_t contains a 1d array of gridcells, each representing a character on the map that the players move on. 

### Definition of function prototypes

A function to create a new player

```c
player_new()
```

A function to update a player's visibility grid (boolGrid) based on the main grid after each player moves. 

```c
void player_playerVisibility(player_t* player, grid_t* grid);
```

A function to take the current grid and print out the string map that the specific player sees theirselves.

```c
char* player_get_string(player_t* player, grid_t* grid);
```

A function to delete a player

```c
player_delete()
```

### Detailed pseudo code

#### `player_playerVisibility`:

	for each gridcell in the main map grid
		checks if each cell is visable from the player's current position
		if yes, mark that index as true
		if not, mark as false

---

#### `player_get_string`:

	for each gridcell in the main map grid
		if the gridcell is visible to player
			if it is gold
				if the player can currently seen it
					show the gold
				otherwise show it as an empty room space
			else if it is the player's character
				show it as @ symbol
			otherwise, print the gridcell as it comes

## Grid

### Data structures

bool* boolGrid
boolGrid is a 1d array of booleans aligning with the main map, where each boolean is true if the corresponding character on the map has been seen by the player, false otherwise

struct gridcell_t
The main 1d array of grid is made of gridcell_t structs, each representing a single character in the map.

### Definition of function prototypes

A function to create a new grid

```c
grid_new()
```

Load a grid from a file specified by the path name. The file represents a grid where each character in the file corresponds to a cell in the grid.

```c
void grid_load(grid_t* grid, char* pathName);
```

 Change the character at a certain location in the grid
 
 ```c
 void grid_set(grid_t* grid, int x, int y, char c);
```

Prints the char* map of the grid

```c
void grid_print(grid_t* grid);
```

Updates the map string after it has changed, when a player has moved

```c
void grid_update_map(grid_t* grid);
```

Iterates over all the gridcells in the grid

```c
void grid_iterate(grid_t* grid, void* arg, void (*itemfunc)(void* arg, void* item));
```

Determine whether a target gridcell is visible from a player gridcell.

```c
bool grid_isVisible(grid_t* grid, gridcell_t* player, gridcell_t* target);
```

Creates a random number of gold piles in a grid, between minPiles and maxPiles

```c
void grid_generateGold(grid_t* grid, int minPiles, int maxPiles, int goldTotal);
```

Deletes a grid

```c
void grid_delete(grid_t* grid );
```

### Detailed pseudo code

#### `grid_load`

	null checks
	open file
	get number of lines (rows)
	get number of columns (length of first line)
	for each line in file
		for each character in line
			create new gridcell
			if it is a wall, set wall=true
			if it is a room, set room=true

#### `grid_isVisible`

	

#### `grid_generateGold`



## Server

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

We use three data structures: an array of `player`s, which contains data for each player, a `grid` that stores the game map, and `gridcell` which stores data for an individual cell in the grid.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments
```c
static int parseArgs(const int argc, char* argv[]);
```

A function to initialize the game struct
```c
static void initializeGame(char* mapFileName, const int seed)
```

A function to update all players when changes on the map occur
```c
static void updatePlayers(players_t* allPlayers, grid_t* map)
```

A function to update all clients when changes on the map occur
```c
static void updateClients(clients_t* allClients)
```

A function to display the game over message and player stats
```c
static void gameOver(players_t* allPlayers)
```

### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	verify map file can be opened for reading
	if seed provided
		verify it is a valid seed number
		seed the random-number generator with that seed
	else
		seed the random-number generator with getpid()

---

## XYZ module

> For each module, repeat the same framework above.

### Data structures

### Definition of function prototypes

### Detailed pseudo code

---

## Testing plan

### unit testing

> How will you test each unit (module) before integrating them with a main program (client or server)?

### integration testing

> How will you test the complete main programs: the server, and for teams of 4, the client?

### system testing

> For teams of 4: How will you test your client and server together?

---

## Limitations

> Bulleted list of any limitations of your implementation.
> This section may not be relevant when you first write your Implementation Plan, but could be relevant after completing the implementation.
