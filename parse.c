#include "p1.h"
/*
Project 2
Recursive Descent Parser for C--
Alexander Geer
COP 4620 - Const. of Lang. Translators
Dr. R Eggen - UNF - Fall 2017
*/

//very important globals
int line = 1;
TOKEN tok; //current token (copies from input stream pointer
SYMT *tbl; //current symtable
SYMBOL *newSym; //newest symbol
char *func;  //current function being parsed
int symsize = 31;
bool mainFlag = false; //flags if main is declared;

//labels
char *lbls[] = { "ID", "INTCONST", "FLOATCONST", "INTTOK", "FLOATTOK", "IFTOK", "ELSETOK", "RET", "VOIDTOK","WHILETOK",
              "PLUS", "MINUS", "MUL", "DIV", "LT", "LTEQ", "GT", "GTEQ", "EQ", "NEQ", "ASGN",
              "SEMI", "CM", "LPAREN", "RPAREN", "LBRACK", "RBRACK", "LBRACE", "RBRACE","LINE", "INTARR","FLOATARR"};



//FUNCTION DECLARATIONS
void nextTok();
int accept(enum T_type t);
int expect(enum T_type t);
int arglist(SYMBOL*f , int n);
void args(SYMBOL* f);
enum T_type factor();
enum T_type term();
enum T_type addx();
enum T_type simexp();
enum T_type expr();
void expst();
void ret();
void whilecon();
void condst();
void st();
void stl();
void vdec();
void locdec();
void cmpd();
void parlist();
int params();
enum T_type type();
void dec2();
void dec();
void decl();
void pgm();
void error();

//input stream pointer
TOKEN* p;

//================================parse(head)
int parse(TOKEN *buf){
p = buf;
tok = *p;
tbl = (SYMT*)calloc(1,sizeof(SYMT));
tbl = newTable(tbl, NULL);

SYMBOL *input = (SYMBOL*)calloc(1,sizeof(SYMBOL));
SYMBOL *output = (SYMBOL*)calloc(1,sizeof(SYMBOL));

//int input(void)
input->id = "input";
input->func = true;
input->t = INTTOK;
input->nparams = 0;

//void output(int x);
output->id = "output";
output->func = true;
output->t = VOIDTOK;
output->nparams = 1;
output->ptypes[0] = INTTOK;

insert(tbl->symT,input);
insert(tbl->symT,output);

//printf("\nbegin parse: \n");
//printf("Line 1: ");
pgm();
//printf("\nGlobalScope");
//prTable(tbl);
}


//=================================pgm
void pgm(){
decl();

if(!find(tbl,"main",true))
   {
   //printf("no MAIN func");
   error();
   }

if(accept(-1))
   printf("ACCEPT");
else
   printf("REJECT");
} 
//=================================decl 
void decl(){ 
//new symbol declared 
newSym = (SYMBOL*)calloc(1,sizeof(SYMBOL));
dec();
while(tok.t == INTTOK || tok.t == FLOATTOK || tok.t == VOIDTOK)
   {
   if(mainFlag)
      {
      //printf("main must be last declaration");
      error();
      }

   //new symbol
   newSym = (SYMBOL*)calloc(1,sizeof(SYMBOL));
   decl();
   }
}

//=================================dec
void dec(){

//type() returns type spec for id
newSym->t = type();

//point newSym token at instream ptr
newSym->id = p->id;

expect(ID);
dec2();
}


/*===================================checkRet
returns 1 if int or float function has a return statement,
as required by the grammar
*/

int checkRet(const char *id){
//find funciton symbol
SYMBOL* s = find(tbl,func,true);

//null ptr case
if(!s)
   error();

//INT and FLT func must have return
if(s->t == INTTOK || s->t == FLOATTOK)
   if(!s->hasRet)
      return 0;

//else
  return 1;
}
 

void doMain(){
//main must be void
if(newSym->t != VOIDTOK)
  {
  //printf("main must be void");
  error();
  }

//main has void params
if(newSym->nparams > 0)
  {
  //printf("main params must be void");
  error();
  }

mainFlag = true;
}  

//=================================dec2
void dec2(){
if(accept(SEMI))
   {
   //symbol is variable
   newSym->var = true;
   if(! insert(tbl->symT, newSym) )
      error();
   reset(&newSym);
   }
//symbol is array
else if(accept(LBRACK))
   {
   newSym->array = true;
   
   void setArrType(enum T_type t)
      {
      if(t == INTTOK)
         newSym->arrType = INTARR;
      else if(t == FLOATTOK)
         newSym->arrType = FLOATARR;
      }
   
   setArrType(newSym->t);

   if(tok.t = INTCONST)
      newSym->arrsize = atoi(tok.id);
   expect(INTCONST);
   expect(RBRACK);
   expect(SEMI);
   if(!insert(tbl->symT, newSym))
      error();
   reset(&newSym);
   }

else if(accept(LPAREN))
  {
  //set func id
  func = newSym->id;
  
  //set symbol as function
  newSym->func = true;
  
  //get parameters
  params();
  expect(RPAREN);
  
  //insert func into tbl
  if(!insert(tbl->symT, newSym))
     error();
  //if(main
  if(strcmp(func,"main") == 0)
     doMain();
  reset(&newSym);
  
  //call cmpd
  cmpd();    
  //check return
  if(!checkRet(func))
     {
     //printf("\n<INT or FLT func no ret>");
     error();
     }//end chk return
  }//end func dec

}
//==================================type(calls error)
enum T_type type(){
if(accept(INTTOK))
   return INTTOK;
else if (accept(FLOATTOK))
   return FLOATTOK;
else if (accept(VOIDTOK))
   return VOIDTOK;
else
  {
  printf("type");
  error();
  }
}

//=================================params
int params(){
   if(accept(VOIDTOK)) 
     {
     if(tok.t == RPAREN)
        {
        newSym->nparams = 0;
        return 0;
        }
     //printf("\nvoid variables not allowed");
     error();
     }
   //new scope, make new table
   SYMT *new = (SYMT*)calloc(1,sizeof(SYMT));
   tbl->next = newTable(new, tbl);
   parlist();
   return 0;
}

//=================================parlist
void parlist(){
   //MAKE A NEW SYMBOL
   SYMBOL *param = (SYMBOL*)calloc(1,sizeof(SYMBOL));
      
   void setArrType(enum T_type t)
      {
      if(t == INTTOK)
         param->arrType = INTARR;
      else if(t == FLOATTOK)
         param->arrType = FLOATARR;
      }

   if(tok.t != ID)
      param-> t = type(); 
   if(param-> t == VOIDTOK)
      {
      //printf("VOID is illegal variable type");
      error();
      } 
  
   param->id = p->id;
 
   expect(ID);
   
   if(accept(LBRACK) )
      {
      //par is array
      expect(RBRACK);
      param -> array = true;
      setArrType(param->t);
      }
   else
      param -> var = true;
   
   //insert param symbol, if duped then fail
   if( !insert(tbl->next->symT, param) )
       error();

   //increment parameter count for function symbol (param owner)
   if(param->array)
      newSym->ptypes[newSym->nparams] = param->arrType;
   else
      newSym->ptypes[newSym->nparams] = param->t;
   
   newSym->nparams++;
   //keep calling parlist while we get commas
   while(tok.t == CM)
      {
      //increment the functions parameter count
      accept(CM);
      parlist();
      }
}

//==================================cmpd
void cmpd(){
prTable(tbl);
if(tbl->next)
   tbl = tbl->next;
else
   {
   SYMT *new = (SYMT*)calloc(1,sizeof(SYMT));
   tbl->next = newTable(new,tbl);
   tbl = tbl->next;
   }

if(accept(LBRACE))
  {
  locdec();
  stl();
  expect(RBRACE);
  }

//check null
if(tbl->prev)
   {
   prTable(tbl);
   //go back one table
   tbl = tbl->prev;
   }
  //free the youngest table
  tbl->next->prev = NULL;
  free(tbl->next);
  tbl->next = NULL;
}

//==================================locdec
void locdec(){
if(tok.t == INTTOK || tok.t == FLOATTOK || tok.t == VOIDTOK) {
   vdec();
   while(tok.t == INTTOK || tok.t == FLOATTOK || tok.t == VOIDTOK )
      locdec();
   }
else
   ; //empty
}

//=================================vdec
void vdec(){
//stupid nested function ARE YOU TRIGGERED
  void setArrType(enum T_type t)
      {
      if(t == INTTOK)
         newSym->arrType = INTARR;
      else if(t == FLOATTOK)
         newSym->arrType = FLOATARR;
      }

newSym->t = type();
if(newSym->t == VOIDTOK)
   {
   //printf("VOID var not allowed");
   error();
   }
newSym->var = true;
newSym->id = p->id;
expect(ID);

if(accept(SEMI))
   ;
else if(accept(LBRACK)){
   newSym->array = true;
   newSym->arrsize = atoi(p->id);
   expect(INTCONST);
   expect(RBRACK);
   expect(SEMI);
   setArrType(newSym->t);
   }
//else
   //printf("syntax error: variable declaration");

insert(tbl->symT, newSym);
reset(&newSym);
}

//=================================stl
void stl(){
if(tok.t != RBRACE)
   {
   st();
   while(tok.t != RBRACE)
     stl();
   }
}

//=================================st
void st(){

if(accept(IFTOK))
   condst();
else if(accept(WHILETOK))
    whilecon();
else if(accept(RET))
    ret();
else if(tok.t == LBRACE)
    cmpd();
else
   expst();
}

//================================condst
void condst(){
expect(LPAREN);
expr();
expect(RPAREN);
st();
if(accept(ELSETOK))
   st();
}

//================================whilecon
void whilecon(){
expect(LPAREN);
expr();
expect(RPAREN);
}

//==================================ret
void ret(){
SYMBOL *f = find(tbl,func,true);

if(accept(SEMI))
   {
   //func must be voi
   if(f->t != VOIDTOK)
      {
      //printf("%s() requires return %s",f->id,lbls[f->t]);
      error();
      }
   }
else{
   //func cant be void
   if(f->t == VOIDTOK)
      {
      //printf("void func cannot return values");
      error();
      }
   enum T_type ret = expr();
   if(ret != f->t)
      {
      //printf("wrong return type");
      error();
      }
   expect(SEMI);
   }
   //function returns something
   f->hasRet = true;
}

//==================================expst
void expst(){
if(accept(SEMI))
   ;
else{
   expr();
   expect(SEMI);
   }

}

//=================================expr
enum T_type expr(){
enum T_type ls;
enum T_type rs;

TOKEN *save = p;

if( tok.t == ID ){
   SYMBOL* s = find(tbl, tok.id, false);
   if(!s) 
     if( !(s = find(tbl, tok.id, true) ) )
       {
       //printf("1sym DNE");
       error();
       }
     
   accept(ID);
   ls = s->t;
   bool subbed = false;
   if(accept(LBRACK) )
      {
      if(!s->array)
         {
         //printf("subscript on non-array");
         error();
         }
      if(expr() != INTTOK)
         {
         //printf("SUBSCRIPT NOT INT");
         error();
         }
      expect(RBRACK);
      subbed = true;
      }
   if(accept(ASGN) )
      {
      if(s->func)
         {
         //printf("cannot assign function");
         error();
         }
      if(s->array && !subbed)
          {
          //printf("arr must be subscripted, cant assign");
          error();
          }
      rs = expr();
      if(ls != rs)
         {
         //printf("LS != RS");
         error();
         }
      }
   else{
      p = save;
      tok = *p;
      return simexp();
      }  
   }
else
   return simexp();
}

//================================simexp
enum T_type simexp(){
enum T_type ls;
enum T_type rs;

ls = addx();
rs = ls;

if(tok.t == EQ || tok.t == NEQ || tok.t == GTEQ || tok.t == LTEQ ||tok.t ==  GT || tok.t == LT ){
   //printf("%s", tok.id);
   nextTok();
   rs = addx();
   }

if(ls != rs)
   {
   //printf("LS!=RS");
   error();
   }

return ls;
}

//===================================addx
enum T_type addx(){

enum T_type ls;
enum T_type rs;

ls = term();
rs = ls;

while(accept(PLUS) || accept(MINUS)){
   rs = term();
   }

if(rs!=ls)
   {
   //printf("LS!=RS");
   error();
   }

return ls;
}

//===================================term
enum T_type term(){

enum T_type ls;
enum T_type rs;

ls = factor();
rs = ls;

while(accept(DIV) || accept(MUL) ){
  rs = factor();
   }

if(rs != ls)
   {
   //printf("LS!=RS");
   error();
   }


return ls;
}

//==============================================factor (calls error)
enum T_type factor(){
//(exp)
char *id;

if(accept(LPAREN)){
   enum T_type ret;
   ret = expr();
   expect(RPAREN);
   return ret;
   }


//var or call
else if(tok.t == ID)
   {
   id = p->id;
   //look for var
   SYMBOL *s = find(tbl, id, false);
   
   //if NULL, s !exist
   if(!s)
      //look for func instead
      if(!(s = find(tbl,id,true) ))
         {
         //printf("symbol %s DNE! ",tok.id);
         error();
         }
   accept(ID);
 
   if(accept(LBRACK))
      {
      if(! s->array)
         {
         //printf("not array");
         error();
         }               
      if(expr() != INTTOK)
         {
         //printf("subscript must be INT");
         error();
         }
      expect(RBRACK);
     return s->t; 
     }
      
      
   else if (accept(LPAREN))
      {
      //chck if func
      if(!(s->func))
         {
        
         s = find(tbl, id, true);
         if(!s)
            {
            //printf("2Sym %s  DNE", id);
            error();
            }
         }
      args(s);
      expect(RPAREN);
      return s->t;
      }

   //return IDs type
   if(s->array) //this can only work if passed as argument (check is in expression, i.e arrays cannot be assigned or referenced out of this context)
      {
      //printf("%s", lbls[s->arrType]);
      return s->arrType;
      }

   //condition for var
   if(!s->var)
      {
      //printf("%s is not a var", s->id);
      error();
      }
   return s->t;
   }

//num
else if(accept(INTCONST))
   return INTTOK;

//float
else if(accept(FLOATCONST))
   return FLOATTOK;

//error?
else{
   //printf("\nLine %d || Syntax error: factor() %d ", line, tok.t);
   error();
   }

}

//========================================args
void args(SYMBOL* f){

//n is recursive counter, returns the number of times arglist is called
int n = 0;

if( tok.t == ID || tok.t == INTCONST || tok.t == FLOATCONST || tok.t == LPAREN)
   {
   //if f is void, error to have args
   n = arglist(f,n);
   if(n != f->nparams)
      {
      //printf("wrong number(%d) of args", n);
      error();
      }
   }

if(n != f->nparams)
   {
   //printf("nparams");
   error();
   }
}

//========================================arglist
int arglist(SYMBOL* f, int n){

if(f->ptypes[n] != expr() )
   {
   //printf("WRONG ARG TYPE ");
   error();
   }
n++;

if(accept(CM))
   {
   n = arglist(f,n);
   }
return n;
}


//==================================accept
int accept(enum T_type t){
if(tok.t == LINE)
   {
   nextTok();
   }
if (tok.t == t)
   {
//   printf("%s ", tok.id);
   nextTok();
   return 1;
   }
//else
   return 0;
}
//==================================expect(calls error)
int expect(enum T_type t){
if(accept(t))
   {
   return 1;
   }
//printf("\nLine %d ||Found : < %s >\n Expected: %s\n",line, lbls[tok.t], lbls[t]);
error();
return 0;
}

void nextTok()
{
p++;
tok = *p;
if(tok.t == LINE)
   {
  // printf("\nLine %d: ", ++line);
  line++; 
   nextTok();
   }
}

//===================================error
void error(){
//printf("ERROR line %d : %s %s\n",line, lbls[tok.t],tok.id);
printf("REJECT");
exit(1);
}
