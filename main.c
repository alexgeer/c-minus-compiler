/*
Contruction of Language Translators - COP 4620 - Fall 2017
Alexander Geer
C-minus Compiler Project

This is the main function for the C-minus compiler.
It opens the file and then calls lex, parse, etc.

Log:
9/14/17
Lexical analysis has been finished. The lexical analysis is performed by
the function lex, which is contained in an external file called "lex.c" .

*/




//header file
#include "p1.h"

int main( int argc, const char* argv[])
{
//buf is pointer to lexed file
TOKEN *buf;

if(argc < 1)  //if no args
	{	
	printf("no input file. /n/n");
	exit(0);
	}	

//open the file
FILE *fp = fopen(argv[1],"r");

//try to open
if(fp == 0)
	{
	printf("Could not open file %s\n", argv[1]);
        exit(0);
	}
//file has been opened
else
	{
//        printf("\nFile %s open, beginning lexical analysis...", argv[1]);
	//call lex function on the file
        buf = lex(fp);
        parse(buf);
	}
//close file

fclose(fp);

return 0;
}//end main
