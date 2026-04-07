#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h> //needed for window 
#include <SDL2/SDL_ttf.h> //needed for font rendering
#include <stdio.h> //needed for debug
#include "typedefs.h" //typenames are used
#include "text.h" //TextField system used

#define WINDOW_TITLE "A notepad by bettiecode" //title
#define WINDOW_WIDTH 900 //width
#define WINDOW_HEIGHT 600 //height
#define FONT_SIZE 16 //loaded font size
#define FONT_WIDTH 9  
#define FONT_HEIGHT 16
#define UNICODE_ALL "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" //string containing all printable characters
#define PADDING 1 //padding x (in characters)

typedef struct //user interface struct
{
	u32 cursorX; //coords (in characters)
	u32 cursorY;
	u32 camX;
	u32 camY;
}
UI;

typedef struct //game struct
{
	SDL_Window* window; //window pointer
	SDL_Renderer* renderer; //rendere pointer
	SDL_Event event; //event storage

	TTF_Font* font; //font pointer
	SDL_Color fontColA; //main font color
	SDL_Color fontColB; //other font color
	SDL_Texture* fontTexA; //main colored font's texture pointer
	SDL_Texture* fontTexB; //other colored font's tecture pointer

	SDL_Rect charSrcRect; //src rectangle storage
	SDL_Rect charDestRect; //dest rectangle storage

	TextField* tf; //pointer to textfield
	UI ui; //ui storage
		
	b8 isRunning; //determines wether the game should be stopped
}
Game;

Game* gameMake();
u0 gameFree(Game* g); 

b8 gameLoadMedia(Game* g);

u0 gameRun(Game* g);
u0 gameEvents(Game* g);
u0 gameRender(Game* g);

u0 gameRenderTextField(Game* g);
#endif

