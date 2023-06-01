# CS50
## Design Spec
### CecsC, Spring, 2023

*> **Extra Credit:**
The additional functionality is as follows:

As a non-spectator player of the game, if you "step" on top of another player's character, you steal 10% of their gold that they have, none if they have no gold. It also displays a message to both the theif and the victim, detailing how much gold was stolen, and their remaining total gold.

The modules that are affected are the `server` module, and the `client` module.

Functions affected:

The `static bool moveOnMap(player_t* player, int newX, int newY)` function within the `server` module has added logic that checks whether another player has been stepped on, and if so, calculates 10% of the other player's gold and transfers it to the player doing the motion.

The `static bool handleGOLD(const char* message)` function within the `client` module is changed to deal with the message of stolen gold, in order to display that message to the client's screen, either if they have stolen gold, or gold has been stolen from them.


Note: see the submit-final branch to see the rest of the DESIGN.md document 