#include "../h/text.h"
#include <malloc.h> //needed for dynamic arrays  

TextField* textFieldMake() //TextField object constructor
{
	TextField* r = malloc(sizeof(TextField)); //allocate memory for object
	if(!r) //if allocation failed
	{
		fprintf(stderr,"Failed to allocate memory for TextField.\n");
		return NULL; //log error and return NULL
	}
	r->lineCount = 0; //field is empty by default
	r->lineCapacity = DEFAULT_CAPACITY; //assign default capacity, must be greater than 0 (otherwise it cant double)
	r->lines = malloc(sizeof(Line)*DEFAULT_CAPACITY);	//allocate memory for lines we have a capacity for
	if(!r->lines) //if allocation failed
	{
		fprintf(stderr,"Failed to allocate memory for TextField's lines.\n"); 
		return NULL; //log error and return NULL
	}
	//for(u32 i = 0; i<DEFAULT_CAPACITY; i++) r->lines[i].data = NULL; //pointers can be initalized to NULL for safety
	return r; //return pointer to object if everythign went correctly
}

u0 textFieldFree(TextField* tf) //delete and free TextField from memory
{
	for(u32 i=0; i<tf->lineCount; i++) //for each used line
	{
		if(!tf->lines[i].data) free(tf->lines[i].data); //free line's characters from memory
	}
	if(!tf->lines) free(tf->lines); //free lines array
};

b8 addToLine(TextField* tf, u32 x, u32 y, char c) //add character to a line
{
	tf->lines[y].length++; //length grows by 1
	
	if(tf->lines[y].length==tf->lines[y].capacity) //if array is full
	{
		tf->lines[y].capacity*=2; //double line's capacity
		tf->lines[y].data = realloc(tf->lines[y].data,tf->lines[y].capacity*sizeof(char)); //reallocate memory of line's characters
		if(!tf->lines[y].data) //if reallocation failed
		{
			fprintf(stderr,"Failed to reallocate memory for line %u.\n",y);	
			return false; //log error and return false 
		}
	}

	for(u32 i=tf->lines[y].length-1; i>x; i--) //push elements after inserted character to the right by 1
	{
		tf->lines[y].data[i]=tf->lines[y].data[i-1]; //push value right
	}	
	tf->lines[y].data[x]=c; //fill the hole made by pushing with the passed value 
	
	return true; //return true if adding is successful
}
u0 deleteFromLine(TextField* tf, u32 x, u32 y) //remove character from line
{
	tf->lines[y].length--; //length shrinks by 1	
	for(u32 i=x; i<tf->lines[y].length; i++) //push elements after deleted character to the left by 1
	{
		tf->lines[y].data[i]=tf->lines[y].data[i+1]; //push value left
	}
	//no need to clear the leftover char after the end, since it is should never be read 
}

b8 addLine(TextField* tf, u32 y) //add new line to TextField
{
	tf->lineCount++; //count grows by 1

	if(tf->lineCount==tf->lineCapacity) //if array is ull
	{
		tf->lineCapacity*=2; //double capacity
		tf->lines = realloc(tf->lines,tf->lineCapacity*sizeof(Line)); //reallocate memory
		if(!tf->lines) //if allocation failed
		{
			fprintf(stderr,"Failed to reallocate memory for lines.\n");
			return false; //log error and return false
		}
		//for(u32 i =	tf->lineCount; i<tf->lineCapacity; i++)	tf->lines[i].data = NULL; //pointers can be initialized to NULL for safety 
	}

	for(u32 i=tf->lineCount-1; i>y; i--) //push line pointers after inserted line to the right by 1
	{
		tf->lines[i]=tf->lines[i-1]; //push to the right
	}	
	tf->lines[y].length=0; //new line is empty by default, it's place is the made hole
	tf->lines[y].capacity=DEFAULT_CAPACITY; //assign default capacity
	tf->lines[y].data=malloc(sizeof(char)*DEFAULT_CAPACITY); //allocate memory for characters
	if(!tf->lines[y].data) //if allocation failed
	{
		fprintf(stderr,"Failed to allocate memory for line.\n"); 
		return false; //log error and return false
	}
	return true; //return true if everything went correctly
}
u0 deleteLine(TextField* tf, u32 y) //remove line from TextField
{
	tf->lineCount--; //count shrinks by 1
	free(tf->lines[y].data); //free memory of line's data
	for(u32 i=y; i<tf->lineCount; i++) //push line pointers after deleted line to the left by one
	{
		tf->lines[i]=tf->lines[i+1]; //push left
	}
	//tf->lines[tf->lineCount].data = NULL; //unused pointer can be set back to NULL for safety 
}

u0 textFieldPrint(TextField* tf) //print TextField to terminal
{
	printf("lines: %u, capacity: %u\n",tf->lineCount,tf->lineCapacity); //print data about TextField
	for(u32 y = 0; y<tf->lineCount; y++) //for each line
	{
		for(u32 x = 0; x<tf->lines[y].length; x++) //for each character
		{
			printf("%c",tf->lines[y].data[x]); //print character
		}
		printf("\n"); //start new line
	}
}

char* textFieldToCString(TextField* tf) //convert TextField into a cstring (needed for saving)
{
	u32 rLength = 0; //size needed for allocation, initially zero
	for(u32 y=0; y<tf->lineCount; y++) rLength+=(tf->lines[y].length+1); //sum line lenghts (+1 for newline or null terminator character)
	char* r = malloc(rLength*sizeof(char)); //allocate memory for returned string
	if(!r) //if allocation failed
	{
		fprintf(stderr,"Failed to allocate for string into which the TextField would have been converted\n");
		return NULL; //log error and return NULL 
	}
	u32 i = 0;
	for(u32 y=0; y<tf->lineCount; y++) //for each line
	{
		for(u32 x=0; x<tf->lines[y].length; x++) r[i++]=tf->lines[y].data[x]; //add each character to string
		r[i++] = '\n'; //add newline
	}
	r[i-1]='\0'; //set end to null terminator
	return r; //return
}

b8 textFieldSave(TextField* tf, char* filename) //save textField to filename
{
	FILE* f = fopen(filename ? filename : "text.txt","w"); //if NULL, save to default filename, else save to passed filename
	char* s = textFieldToCString(tf); //string to write
	if(!s) //if conversion failed
	{
		fprintf(stderr,"Failed to save TextField.\n");
		return false; //log error and return false
	}
	fprintf(f,s); //write string to file
	free(s); //free string 
	fclose(f); //close file
	return true; //return true if successful
}
