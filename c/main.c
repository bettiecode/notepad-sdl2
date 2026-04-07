#include "../h/game.h" //needed for game system

int main(void)
{
	Game* game = gameMake(); //make game object
	if(!game) //if it failed
	{
		fprintf(stderr,"Error: Failed to initialize Game object\n");
		return 1; //log error and exit with failiure
	}
	if(!gameLoadMedia(game)) //if loading media failed
	{
		fprintf(stderr,"Error: Failed to load Game Media\n");
		return 2; //log error and exit with failiure
	}
	gameRun(game); //run game
	gameFree(game); //after game has finished, free it from memory
	return 0; //exit with no errors
}

