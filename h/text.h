#ifndef TEXT_H
#define TEXT_H
#include "typedefs.h" 
#define DEFAULT_CAPACITY 1 //macro for default capacity of dynamic arrays

typedef struct //line struct
{
	char* data; //pointer to line's content
	u32 length; //length of line, no null terminator or newline character counted
	u32 capacity; //line's capacity
}
Line;

typedef struct //TextField struct
{
	Line* lines; //pointer to array holding lines
	u32 lineCount; //amount of lines used
	u32 lineCapacity; //capacity of array
} TextField;

TextField* textFieldMake(); 
u0 textFieldFree(TextField* tf);

b8 addToLine(TextField* tf, u32 x, u32 y, char c);
u0 deleteFromLine(TextField* tf, u32 x, u32 y);

b8 addLine(TextField* tf, u32 y);
u0 deleteLine(TextField* tf, u32 y);

u0 textFieldPrint(TextField* tf);

char* textFieldToCString(TextField* tf);

b8 textFieldSave(TextField* tf, char* filename);

#endif
