 
#include "p1.h"



//Function Declarations
void execute();
int processLine();
void tokenizeInput();
int isKeyword(char *src, char *term);
void printToken(char *src, char *term);
bool issymbol(char c);
void setwrTok(char c);
void setKeyword(int kw);

//Definitions
#define BUFF_SIZE 1024 //the size of the buffer
#define KW_MAX_SIZE 7 //the lenght of the longest keyword
#define KW_COUNT 7 //the number of keywords
#define N_SYM 15   //the number of symbols

//Global Variables
FILE *fp;        //points at input file
char *inBuff;    //points at start of buffer
char *outBuff;   //points at terminal of buffer
char buffer[BUFF_SIZE]; //the buffer array
int buffIdx = 0;        //the current buffer index

int pa_depth = 0;       //parens depth
int brc_depth = 0;      //brace depth
int brk_depth = 0;      //bracket depth 
int cm_depth = 0;       //comment depth

bool comm = false;      // condition for in multi line comment

//keyword and character table
 	/* 
 	These are the legal characters in the c-minus language :
	lower case, 0-9, and a set of operator symbols. 
 	There exist some two-character length operators (<=, != ... etc)
 	which are checked for in the tokenize function).
 	*/
char keywords[KW_COUNT][KW_MAX_SIZE] = {"else","if","int","return","void","float","while"};
char symbols[] = {'+', '-' , '*' , '/' , '=' , '<' , '>' , '[' , ']' , '{' , '}' , '(' , ')' , ',' ,  ';'};

bool done; //condition for buffering lines. When EOF is hit, done is assigned false and the last line is lexed.

TOKEN *bufstart; //start of the buffer
TOKEN *bufptr; // return value for lex
TOKEN wrTok;
//=====================================================lex
/*
Central function of file. Reduntantly points fp
at argument FILE. Not sure why I did this, I
think was worried about integrity of the original
pointer. Betrays my weakness with C. 
*/

TOKEN *lex(FILE *file){
   //allocate 1K of TOKENs
   bufstart = (TOKEN*)calloc(BUFF_SIZE, sizeof(TOKEN));
   //set the current buffer index to the start of the buffer
   bufptr = bufstart;
   fp = file;
   inBuff = buffer;
   outBuff = buffer;
   buffIdx = 0;
   done = false;

   execute();
   bufptr->t = -1;   
   return bufstart;
}//end lex

//=================================================execute
/*
A loop which buffers a line, then analyzes it. Loop
continues until EOF. Prints each input unless it is 
a blank line.
*/
void execute(){
int count = 0;
//put line into buffer
while(!done)
   {
    count++;
   if (processLine() )
      {
      if(buffer[0] != '\0')
         {
//         printf("\n LINE %d INPUT: %s \n", count, buffer);
         tokenizeInput();
         }//if not new line
      }//if processline
    }//while
}//execute

//========================================================processLine
/*More accurately, bufferLine. Loads the current line of the file into 
the buffer. Encountering the \n or EOF char, the function assigns the current
index '\0', resulting in a string.
Returns 1 no matter what(again not sure why). Perhaps this could be used for
error conditions, but it is currently not necessary.
*/
int processLine(){

//temp char
char c;

//set buffer index to 0
buffIdx = 0;

while(c = fgetc(fp) )
   {
   if (buffIdx == BUFF_SIZE)
      {
      printf("line too long for buffer. increase buffer size");
      return 1;
      }
   if (c == EOF) 
      {
      buffer[buffIdx] = '\0';
      done = true;
      return 1; //done
      }
   else if (c == '\n')
      {
      buffer[buffIdx] = '\0';
      //reset index for next time
      return 1; //done with line
      }
   else
      {
      buffer[buffIdx++] = (char)c;
      }
   }//while getting chars      
}//processLine

//==========================================================tokenizeInput
/*
Size of function doesn't reflect its simplicity. Has two pointers src(source char)
and term (terminal char). Src and term start at beginning of buffer. Term is incremented until
largest possible token is captured by src and term. Token is accepted and src is pointed at term.
The process continues until '\0' is encountered. The conditions checked are

if comment
   look for  comment end '*' + '/'
if space
  skip
if lower
  keyword or ID
if num
   int or float
if punct
   single char or 2char operator
   or activate comment conditions
else
   illegal char

Each 'if' condition encapsulates a DFA which accumulates the token chars.
*/

void tokenizeInput(){
   
   char *src; //source char for token
   char *term; //terminal char for token
   char *termf; //terminal char for floats

   char c; //temp char
   char sav; //secondary temp char for lookahead
   //starting at beginning of line
   src = buffer;
   term = buffer;
   c = *src;

   while(c != '\0')
      {
      c = *src;
      //look for token type
      if(comm)
        {
//        printf("in comm\n");
        while(*term != '\0' && comm)
         {
//         printf("depth %d \n", cm_depth);
         if(*term == '/' && *(term + 1) == '*')
            {
            cm_depth++;
            term++;
            }
   
         else if (*term == '*' && *(term + 1) == '/')
             {
             cm_depth--;
             term++;
             }
         if (cm_depth == 0)
            comm = false;
  
         term++;
         c = *term;
         }//while com
        }//if comm  


      else if(isspace(c))		//skip spaces
         {
            term++;
            src++;
            //skip space char
         }   
      else if(islower(c))	//lower case
         {
         while(islower(c))
            {
            term++;
            c = *(term);            
            } //while lower
         int kw = 0;
         if(kw = isKeyword(src, term))
         {         
  //          printf("KEYWORD:");
            setKeyword(kw - 1);
         }
         else
            {
    //        printf("ID:");
            wrTok.t = ID;
            }
        
         printToken(src, term);
         } //if islower
      
      else if( isdigit(c) )	//digits
         {   
         //look at next char
         sav = c;
         term++;
         c = *term;
         //if digit accept it
         if(isdigit(c) || c == '.')
	    {
            while(isdigit(c))
               {
               term++;
               c = *(term);
               }//while digit
            if(c == '.')
               {
               termf = term;
               termf++;
               c = *termf;
               if(isdigit(c))
	          {
                  while(isdigit(c))
                     {
                     termf++;
                     c = *termf;
                     }
                  if(c == 'E')
                     {   
                     termf++;
                     c = *termf;
                     
                     if(c == '+' || c == '-')
                        {
                        termf++;
                        c = *termf;
                        }//if E+-
                     if(isdigit(c))
                        {
                        while(isdigit(c) )
                           {
                           termf++;
                           c = *termf;
                           }//while Edigit
      //                  printf("FLOAT CONST: ");
                        wrTok.t = FLOATCONST;
                        term = termf; 
                        }//if Edigit
                     else
                        {
                        printf("float error: ");
                        term = termf;
                        }//else Edigit
                     }//if 'E' 
                  else
                     {
                  //x.yz float accepted, reset term;
                  term = termf;
        //          printf("FLT CONST:");
                  wrTok.t = FLOATCONST;
                     }
                  }//if .digit
               else{
          //        printf("INT CONST:");
                  wrTok.t = INTCONST; 
                 }
               }//if '.'
            else
               { 
            //   printf("INT CONST:");
               wrTok.t = INTCONST;
               }
            }//if dig
         else if(isdigit(sav))
            {
       //     printf("INT CONST:");
            wrTok.t = INTCONST;
            }
         //print final tok
      printToken(src,term);
      } //if + - or digit
      
      else if(ispunct(c))
         {
         
         if(issymbol(c) || (c == '!' && *(term + 1) == '=') )
            {
            sav = c;
            c = *(++term);
                        
            if(sav == '=' && c == '='){
               wrTok.t = EQ;
               term++;
               }
            else if(sav == '<' && c == '='){
               wrTok.t = LTEQ;
               term++;
               }
            else if(sav == '>' && c == '='){
               wrTok.t = GTEQ;
               term++;
               }
            else if(sav == '!' && c == '='){
               wrTok.t = NEQ;
               term++;
               }
            else if(sav == '/' && c == '/')
               {
                 break; // break while loop
               }// if single line comm
            else if(sav == '/' && c == '*')
               {
               cm_depth++;
               comm = true;
               term++;
               }
            }//if issymbol
        else
           {
           printf("ERROR1 - invalid character: ");
           term++;
           }
       
        if(!comm)
           printToken(src,term);
	 }//if ispunct
      else
         {
         //printf("ERROR2 - invalid character: %c \n", *src);
         src++;
         term++;        
         }

      //update loop
      src = term;

//      printf("\nline tokenized curr char %c \n", c); 
      }//while
   //add newline token
   bufptr->t = LINE;
   bufptr++;
}//tokenizeInput

//=======================================================isSymbol
/*
Checks if punct char is legal operator symbol
*/
bool issymbol(char c){
   int i;

   for(i = 0; i < N_SYM; i++)
      {
      if( c == symbols[i])
         {
         setwrTok(c);
         return true;
         }
      }
   return false;
}//issymbol

//========================================================
void setwrTok(char c){
switch(c) {
   case '+' : wrTok.t = PLUS;
   break; 
   case '-' : wrTok.t = MINUS;
   break;
   case '*' : wrTok.t = MUL;
   break;
   case '/' : wrTok.t = DIV;
   break;
   case '<' : wrTok.t = LT;
   break;
   case '>' : wrTok.t = GT;
   break;
   case '=' : wrTok.t = ASGN;
   break;
   case '(' : wrTok.t = LPAREN;
   break;
   case ')' : wrTok.t = RPAREN;
   break;
   case '[' : wrTok.t = LBRACK;
   break;
   case ']' : wrTok.t = RBRACK;
   break;
   case '{' : wrTok.t = LBRACE;
   break;
   case '}' : wrTok.t = RBRACE;
   break;
   case ',' : wrTok.t = CM;
   break;
   case ';' : wrTok.t = SEMI;
   break;
   }

}//setwrTok
//========================================================
void printToken(char *src, char *term){
// uncomment to print lexing output
int i = 0;
while(src < term )
   {
//   printf("%c",*src);
   bufptr->id[i++] = *src;
   src++;
   }  
   //initialize TOKENs t code
   bufptr->t = wrTok.t;
   //move to next buffer space
   bufptr++;   

}//printToken

//=======================================================isKeyword
/*
2-dimensional linear search on the keyword string array
returns true if token(chars between src/term) is a keyword, false if not
*/

int isKeyword(char *src, char *term){
     bool hit = false;
     int i; //outer loop index
     int j; //inner loop index
 

//     printf("token length  %d |  " , term - src);

     //check size
     if(term - src > KW_MAX_SIZE)
	return 0;
     if(term - src < 1)
        return 0;

     for(i = 0; i < KW_COUNT; i++)
        {
        //check first letter
        if(*src == keywords[i][0])
           {
//         printf("poss match w %s ", keywords[i]);
               for(j = 0; j < (term - src); j++)
		  {
                  if(*(src + j) == keywords[i][j])
                     hit = true;
                  else{
//                     printf(" wrong hit at %d , char %c \n ", j, *(src + j) );
                     hit = false;
                     j = term - src;
                     } 
                  }// inner for
	      //test for succ
              if(hit && keywords[i][j] == '\0' )
		  return i + 1;
           }//if
        }//outer for
      return 0;
}//isKeyword

//===================================================
void setKeyword(int kw){
switch(kw){
   case(0) : wrTok.t = ELSETOK;
             break;
   case(1) : wrTok.t = IFTOK;
             break;
   case(2) : wrTok.t = INTTOK;
             break;
   case(3) : wrTok.t = RET;
             break;
   case(4) : wrTok.t = VOIDTOK;
             break;
   case(5) : wrTok.t = FLOATTOK;
             break;
   case(6) : wrTok.t = WHILETOK;
             break;
}


}//setKeyword
