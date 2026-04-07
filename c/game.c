#include "../h/game.h"
#include "../h/text.h"

Game* gameMake() //construct game object
{
	//alloc
	Game* g = malloc(sizeof(Game)); //allocate heap memory for game object
	if(!g) //if failed
	{
		fprintf(stderr,"Error: Failed to allocate memory during gameMake()\n");
		return NULL; //log error and return NULL
	}
	g->window = NULL; //pointers are initalized to NULL 
	g->renderer = NULL; 
	g->font = NULL;
	g->fontColA.r = 255; g->fontColA.g = 255; g->fontColA.b = 255; g->fontColA.a = 255; //set main font color
	g->fontColB.r = 255; g->fontColB.g = 100; g->fontColB.b = 100; g->fontColB.a = 255; //set other font color
	
	g->tf = NULL;
	g->fontTexA = NULL;
	g->charSrcRect.y = 0; g->charSrcRect.w = FONT_WIDTH;g->charSrcRect.h=FONT_HEIGHT; //set sizes and initalize positions of rectangles
	g->charDestRect.w = FONT_WIDTH; g->charDestRect.h = FONT_HEIGHT;

	g->ui.cursorX = 0; g->ui.cursorY = 0; //initalize cursor
	g->ui.camX = 0; g->ui.camY = 0;	

	g->isRunning = false; //false until game is started
		
	//SDL
		//SDL itself
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) //if sdl init (video and events) fails
	{
		fprintf(stderr,"Error initializing SDL: %s\n",SDL_GetError());
		free(g); 
		return NULL; //log error, free all previous allocations, then return NULL
	}
		//SDL_ttf
	if(TTF_Init()) //if sdl ttf init failed
	{
		fprintf(stderr,"Error initializing SDL_ttf module: %s\n",SDL_GetError());
		SDL_Quit();
		free(g);
		return NULL; //log error, free all previous allocations, then return NULL

	}

		//window
	g->window = SDL_CreateWindow(WINDOW_TITLE,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH, WINDOW_HEIGHT,0); //make window according to specified macros
	if(!g->window) //if creation fails 
	{
		fprintf(stderr,"Error creating window: %s\n",SDL_GetError());
		TTF_Quit();
		SDL_Quit();
		free(g);
		return NULL; //log error, free all previous allocations, then return NULL

	}
		//renderer
	g->renderer = SDL_CreateRenderer(g->window,-1,0);
	if(!g->renderer) //if creation fails
	{
		fprintf(stderr,"Error creating renderer: %s\n",SDL_GetError());
		SDL_DestroyWindow(g->window);
		TTF_Quit();
		SDL_Quit();
		free(g);
		return NULL; //log error, free all previous allocations, then return NULL
	}
	//TextField
	g->tf = textFieldMake(); //create TextField
	if(!g->tf) //if failed
	{
		fprintf(stderr,"Error creating Game's textField. \n");
		SDL_DestroyRenderer(g->renderer);
		SDL_DestroyWindow(g->window);
		TTF_Quit();
		SDL_Quit();
		free(g);
		return NULL; //log error, free all previous allocations, then return NULL
	}
	if(!addLine(g->tf,0)) return NULL; //if adding inital line fails, then return NULL
	return g; //return pointer to object if successful
}

u0 gameFree(Game* g) //free game obj from memory
{
	SDL_DestroyTexture(g->fontTexB);
	SDL_DestroyTexture(g->fontTexA);
	TTF_CloseFont(g->font);
	//LIFO
	textFieldFree(g->tf);
	SDL_DestroyRenderer(g->renderer);
	SDL_DestroyWindow(g->window);
	TTF_Quit();
	SDL_Quit();
	free(g);
}

b8 gameLoadMedia(Game* g)
{
	//font
	g->font = TTF_OpenFont("a/mDOS.ttf",FONT_SIZE); //load font
	if(!g->font) //if failed
	{
		fprintf(stderr,"Error loading Font: %s\n",TTF_GetError());
		return false; //log error, free all previous allocations, then return false
	}
	//font col a
	SDL_Surface* surface = TTF_RenderText_Blended //make surface for font col a
	(
		g->font, 
		UNICODE_ALL,
		g->fontColA
	); 
	if(!surface) //if failed
	{
		fprintf(stderr,"Error creating SDL_Surface: %s\n",SDL_GetError());
		TTF_CloseFont(g->font);	
		return false; //log error, free all previous allocations, then return false
	}
	g->fontTexA = SDL_CreateTextureFromSurface(g->renderer,surface); //make font texture
	if(!g->fontTexA) //if failed
	{
		fprintf(stderr,"Error creating SDL_Texture from SDL_Surface: %s\n",SDL_GetError());
		SDL_FreeSurface(surface);
		TTF_CloseFont(g->font);	
		return false; //log error, free all previous allocations, then return false
	}	
	SDL_FreeSurface(surface); //clear surface
	//font col b
	surface = TTF_RenderText_Blended //fill surface for other font
	(
		g->font, 
		UNICODE_ALL,
		g->fontColB
	);
	if(!surface) //if failed
	{
		fprintf(stderr,"Error creating SDL_Surface: %s\n",SDL_GetError());
		SDL_DestroyTexture(g->fontTexA);
		TTF_CloseFont(g->font);	
		return false; //log error, free all previous allocations, then return false
	}
	g->fontTexB = SDL_CreateTextureFromSurface(g->renderer,surface); //make font texture
	if(!g->fontTexB) //if failed
	{
		fprintf(stderr,"Error creating SDL_Texture from SDL_Surface: %s\n",SDL_GetError());
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(g->fontTexA);
		TTF_CloseFont(g->font);	
		return false; //log error, free all previous allocations, then return false
	}	
	SDL_FreeSurface(surface); //free surface from memory after no more fonts need to be rasterized
	return true; //return true if successful
}

u0 gameRun(Game* g) //run game
{
	g->isRunning = true; //start
	while(g->isRunning) //while not stopped
	{
		gameEvents(g);	//handle events
		gameRender(g);	//render
		SDL_Delay(16); //match target fps = 60
	}
	//end game
}

u0 gameEvents(Game* g) //handle events
{
	//input system is really barebones, could use lots of improvement
	while(SDL_PollEvent(&g->event)) //loop through events
	{
		char c = '\0'; //typed char, initally null terinatior
		switch(g->event.type) //event types
		{
			case SDL_QUIT: //window closed
				g->isRunning = false; //stop game
			break;
			case SDL_KEYDOWN: //key presses
				//printf("%u\n",g->event.key.keysym.scancode); //debug
				switch(g->event.key.keysym.scancode) //switch between scancodes
				{
					//quit game
					case SDL_SCANCODE_ESCAPE: //if esc key
						g->isRunning = false; //stop game
					break;
					//print textField to terminal
					case SDL_SCANCODE_TAB: //if tab key
						textFieldSave(g->tf,NULL); //save TextField
					break;
					//delete char
					case SDL_SCANCODE_BACKSPACE: //backspace
						if(g->ui.cursorX!=0) deleteFromLine(g->tf,--g->ui.cursorX, g->ui.cursorY); //if not at x=0, then delete char before cursor, then move back 1 
						else if(g->ui.cursorY!=0) //if at x=0, then delete line
						{
							deleteLine(g->tf,g->ui.cursorY--); //delete line and move cursor up 
							g->ui.cursorX = g->tf->lines[g->ui.cursorY].length; //move cursor to the end of line
						}
					break;
					//new line
					case SDL_SCANCODE_RETURN: //if enter
						if(!addLine(g->tf,++g->ui.cursorY)) g->isRunning = false; //if adding line failed, then stop game
						else
						{
							g->ui.cursorX = 0; //move cursor and cam to start
							g->ui.camX = 0;
						}
					break;
					//move up
					case SDL_SCANCODE_UP: //up arrow
						if(g->ui.cursorY!=0) 
						{
							if(g->ui.cursorX>=g->tf->lines[--g->ui.cursorY].length) //if line moved to is shorter
							{
								g->ui.cursorX = g->tf->lines[g->ui.cursorY].length; //move cursor to the end of line
								if((g->ui.cursorX+PADDING)*FONT_WIDTH<=WINDOW_WIDTH-FONT_WIDTH) g->ui.camX=0; //if no cam offset is needed, then push it to left completely
								else g->ui.camX-=g->tf->lines[g->ui.cursorY+1].length-g->tf->lines[g->ui.cursorY].length; //else, make it stpe back as much as the cursor did
							}
						}
					break;
					//move left
					case SDL_SCANCODE_LEFT: //left arrow
						if(g->ui.cursorX!=0) g->ui.cursorX--; //if not at start of line i move back
					break;
					//move down
					case SDL_SCANCODE_DOWN: //down arrow
						if(g->ui.cursorY<g->tf->lineCount-1) //if not at end of TextField
						{
							if(g->ui.cursorX>=g->tf->lines[++g->ui.cursorY].length) //if line moved to is shorter 
							{
								g->ui.cursorX = g->tf->lines[g->ui.cursorY].length; //move cursor to the end of line
								if((g->ui.cursorX+PADDING)*FONT_WIDTH<=WINDOW_WIDTH-FONT_WIDTH) g->ui.camX=0; //if no cam offset is needed, then push it to left completely
								else g->ui.camX-=g->tf->lines[g->ui.cursorY-1].length-g->tf->lines[g->ui.cursorY].length; //if no cam offset is needed, then push it to left completely
							}
						}
					break;
					//move right
					case SDL_SCANCODE_RIGHT: //right arrow
						if(g->ui.cursorX<g->tf->lines[g->ui.cursorY].length) g->ui.cursorX++; //if not at end of line, move forward
					break;
					//add char
					default: //typed input US Keyboard regardless of system (not intentioanally, i just didn't feel like implementing different keyboards)
						if(g->event.key.keysym.scancode>=4  && g->event.key.keysym.scancode<=29) c = g->event.key.keysym.scancode-4+'a'; //if alphabetic character
						else if(g->event.key.keysym.scancode>=30 && g->event.key.keysym.scancode<=39) c = '0'+(g->event.key.keysym.scancode+1)%10; //if number
						else //else map scancodes to values
						{
							case SDL_SCANCODE_GRAVE: c='`'; break;
							
							case SDL_SCANCODE_MINUS: c='-'; break;
							case SDL_SCANCODE_EQUALS: c='='; break;

							case SDL_SCANCODE_LEFTBRACKET: c='['; break;
							case SDL_SCANCODE_RIGHTBRACKET: c=']'; break;

							case SDL_SCANCODE_SEMICOLON: c=';'; break;
							case SDL_SCANCODE_APOSTROPHE: c='\''; break;	
							case SDL_SCANCODE_BACKSLASH: c='\\'; break;

							case SDL_SCANCODE_COMMA: c=','; break;	
							case SDL_SCANCODE_PERIOD: c='.'; break;
							case SDL_SCANCODE_SLASH: c='/'; break;
							
							case SDL_SCANCODE_SPACE: c=' '; break;
							case SDL_SCANCODE_LSHIFT: c='\0'; break; //dont collect shift as tyoed chracter
						}
					break;
				}
			break;
		}
		if(g->ui.camY>g->ui.cursorY) g->ui.camY--; //if cursor above camera, then move up
		if((g->ui.cursorY-g->ui.camY)*FONT_HEIGHT>WINDOW_HEIGHT-FONT_HEIGHT) g->ui.camY++; //if cursor under window bottom, then move down
		if(g->ui.camX>g->ui.cursorX) g->ui.camX--; //if cursor behind camera, then move back
		if((g->ui.cursorX-g->ui.camX+PADDING)*FONT_WIDTH>WINDOW_WIDTH-FONT_WIDTH) g->ui.camX++; //if cursor infront of window border, move forward

		if(c!='\0') //c is null terminator if no character was typed in this frame
		{
			SDL_PumpEvents(); //repump events to get shift state
			b8 shift = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT]; //get shift state

			if(shift) //if pressed, transform values
			{
				if(c>='a' && c<='z') c+=('A'-'a'); //if letter, capitalize by subtracting
				else
				{
					switch(c) //map values to transformed values
					{
						case '`': c='~'; break;
							
						case '1': c='!'; break;
						case '2': c='@'; break;
						case '3': c='#'; break;
						case '4': c='$'; break;
						case '5': c='%'; break;
						case '6': c='^'; break;
						case '7': c='&'; break;
						case '8': c='*'; break;
						case '9': c='('; break;
						case '0': c=')'; break;
						
						case '-': c='_'; break;
						case '=': c='+'; break;

						case '[': c='{'; break;
						case ']': c='}'; break;

						case ';': c=':'; break;
						case '\'': c='"'; break;	
						case '\\': c='|'; break;

						case ',': c='<'; break;	
						case '.': c='>'; break;
						case '/': c='?'; break;	
					}
				}
			}
			if(!addToLine(g->tf,g->ui.cursorX++,g->ui.cursorY,c)) g->isRunning = false; //if adding character to line failed, stop game
		}
	}
}

u0 gameRender(Game* g) //render game
{
	SDL_SetRenderDrawColor(g->renderer,16,16,16,255); //bg color
	SDL_RenderClear(g->renderer); //clear bg
	gameRenderTextField(g); //render textfield
	SDL_RenderPresent(g->renderer); //finish drawing
}

u0 gameRenderTextField(Game* g) //render textfield
{
	if(g->ui.camY>0) //if camera is not at top, draw marker
	{
		g->charDestRect.w*=4; g->charDestRect.h*=4; //quadruple size
		g->charDestRect.x = WINDOW_WIDTH-FONT_WIDTH*4; g->charDestRect.y = 0; //place to top right corner
		g->charSrcRect.x = FONT_WIDTH*('^'-'!'); //find ^ character
		SDL_RenderCopy(g->renderer,g->fontTexB,&g->charSrcRect,&g->charDestRect); //draw char
		g->charDestRect.w/=4; g->charDestRect.h/=4; //reset size
	}

	SDL_SetRenderDrawColor(g->renderer, 100,100,100,255); //set cursor color
	g->charDestRect.x = FONT_WIDTH*(g->ui.cursorX-g->ui.camX+PADDING);  g->charDestRect.y = FONT_HEIGHT*(g->ui.cursorY-g->ui.camY); //find cursor pos
	SDL_RenderDrawRect(g->renderer, &g->charDestRect); //draw rectangle

	for(u32 y = g->ui.camY; y<g->tf->lineCount; y++) //for each line
	{
		if(g->ui.camX==0) //if camera is at the left end, then draw line start indicators
		{
			g->charSrcRect.x = FONT_WIDTH*('>'-'!'); //find > character
			g->charDestRect.x = 0; g->charDestRect.y = FONT_HEIGHT*(y-g->ui.camY); //place to side
			SDL_RenderCopy(g->renderer,g->fontTexB,&g->charSrcRect,&g->charDestRect); //draw char
		}	
		for(u32 x=g->ui.camX; x<g->tf->lines[y].length; x++) //for each char
		{
			u8 c = g->tf->lines[y].data[x]; //get value
			c = (c>='!' && c<='~') ? c : 32; //set to space if not printable
			g->charSrcRect.x = FONT_WIDTH*(c-'!'); //find on texture map
			g->charDestRect.x = FONT_WIDTH*(x-g->ui.camX+PADDING); g->charDestRect.y = FONT_HEIGHT*(y-g->ui.camY); //place
			SDL_RenderCopy(g->renderer,g->fontTexA,&g->charSrcRect,&g->charDestRect); //draw char
		}
		//draw line end indicator 
		g->charSrcRect.x = FONT_WIDTH*('<'-'!'); //find < character
		g->charDestRect.x = FONT_WIDTH*(g->tf->lines[y].length-g->ui.camX+PADDING); g->charDestRect.y = FONT_HEIGHT*(y-g->ui.camY); //place
		SDL_RenderCopy(g->renderer,g->fontTexB,&g->charSrcRect,&g->charDestRect); //draw character

	}
}

