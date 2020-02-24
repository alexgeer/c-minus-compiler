#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <ctype.h>

//some useful enumerated types
enum T_type { ID, INTCONST, FLOATCONST, INTTOK, FLOATTOK, IFTOK, ELSETOK, RET, VOIDTOK, WHILETOK,
              PLUS, MINUS, MUL, DIV, LT, LTEQ, GT, GTEQ, EQ, NEQ, ASGN, 
              SEMI, CM, LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE, LINE, INTARR, FLOATARR};

/*
char *lbls[] = { "ID", "INTCONST", "FLOATCONST", "INTTOK", "FLOATTOK", "IFTOK", "ELSETOK", "RET", "VOIDTOK","WHILETOK",
              "PLUS", "MINUS", "MUL", "DIV", "LT", "LTEQ", "GT", "GTEQ", "EQ", "NEQ", "ASGN",
              "SEMI", "CM", "LPAREN", "RPAREN", "LBRACK", "RBRACK", "LBRACE", "RBRACE"};
*/

//strctures
typedef struct {
   enum T_type t;

   char id[30];   
} TOKEN;

//The Symbol
/*
tok is the Token being stored
func is function flag  
var is variable flag  
nparams is the number of parameters
ptypes is an array of parameter types(it uses the token-Type enum)
t is the return value
*/

typedef struct SYMBOL SYMBOL;
struct SYMBOL{
  TOKEN *tok;
  char *id;
  bool func;
  bool var;
  bool array;
  enum T_type arrType;
  int arrsize;
  //function props
  int nparams; //number of parameters
  enum T_type ptypes[10]; //types of parameters (initialized at runtime using malloc)
  bool hasRet;            //makes sure int/float func has return  
  //variable/return props
  enum T_type t; //this is return type if the symbol is a function, otherwise a variable type;
  int ival;
  float fval;

  SYMBOL *next; //linked list chain in hash table
};

/*Symbol Table
  symT points to beginning of SYMBOL* array
  next points to the next sym table(linked list)
  prev points to the previous sym table
*/

typedef struct SYMT SYMT;
struct SYMT 
  {
   SYMBOL **symT;
   SYMT *next;
   SYMT *prev;
  };

//Function declarations
TOKEN *lex(FILE *file);
int parse(TOKEN *buf);

SYMT *newTable(SYMT *new, SYMT *prv);
SYMBOL *find(SYMT *target, const char *str, bool func);
int insert(SYMBOL **target, SYMBOL *s);
int hash(const char *str);
void reset(SYMBOL **s);
void prTable(SYMT *target);
