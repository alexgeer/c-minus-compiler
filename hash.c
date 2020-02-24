 
#include "p1.h"
#define TABLESIZE 31

char *labels[] = { "ID", "INTCONST", "FLOATCONST", "INTTOK", "FLOATTOK", "IFTOK", "ELSETOK", "RET", "VOIDTOK","WHILETOK",
              "PLUS", "MINUS", "MUL", "DIV", "LT", "LTEQ", "GT", "GTEQ", "EQ", "NEQ", "ASGN",
              "SEMI", "CM", "LPAREN", "RPAREN", "LBRACK", "RBRACK", "LBRACE", "RBRACE","LINE","INTARR","FLOATARR"};




SYMBOL blank = {NULL,
		NULL,
		false,
		false,
		false,
		0,
		0,
		0,
		-1,
		-1,
		0,
		};

//creates a new table=================================
SYMT *newTable(SYMT *new, SYMT *prv){

   new->symT = (SYMBOL **)calloc(TABLESIZE,sizeof(SYMBOL*));
   new->prev = prv;

   return new;
}

//===================================================insert 
int insert(SYMBOL **target, SYMBOL *s){ 
int key = hash(s->id); 
//printf("at %p", target[key]); 
//printf("\n<<TRY %s %s at [%d]>>\n",labels[s->t], s->id,key);

if(!target[key])
   {
//   printf(" <<INSERTING %s %s>> ",labels[s->t], s->id);
   target[key] = s;
//   printf("<<%s>>", target[key]->id);
   return 1;                   //ret true
   }


while(target[key])
   {
   
   //check duplication
   
      if(strcmp(target[key]->id, s->id) == 0)
         {       
         //if same type, overlap
         if(target[key]->func == s->func)
            {
            //printf("Symbol %s already exists", s->id);
            return 0; //false
            }
         //if already doubled
         else if(target[key]->next)
            {
            return 0;
            }
         //if non-conflict duplicate (func and var)
         else 
            {
            target[key] -> next = s;
            return 1;
            }
         }
     
   //increment key
   key++;       
   
   //overflow case
   if(key > TABLESIZE)
      key = key%TABLESIZE;
   
   }
   

target[key] = s;

//printf("<< INSERTED %s %s >>",labels[s->t], s->id);      
//printf("symt : %s", target[key]->id);
return 1;                 //return true
}

//================================================find
SYMBOL *find(SYMT *target, const char *str, bool func){
int key = hash(str);

//point tmp at beginning of target table
SYMBOL **tmp = target->symT;

if(!tmp[key])
   {
   if(target->prev)
      //SUCC
      {
    //  printf("here");
      return find(target->prev,str,func);
      }
   else
      {
      //FAIL
  //    printf("SYMBOL %s  NOT FOUND", str);
      return NULL;
     }
   }

else while(tmp[key])
   {
   //compare key's id with search string
   if(strcmp(str, tmp[key]->id) == 0)
      {
      //if same func val(search for func and hit func
      if(tmp[key]-> func == func)
         return target->symT[key];
      else if(tmp[key]->next)
         return target->symT[key]->next;
      }

   key++;
   if(key>TABLESIZE)
      key = key%TABLESIZE;
   }
//check prev if it exists

if(target->prev)
   return find(target->prev,str, func);
else
   {
   //failure
//   printf("FINDsymbol %s not found", str);
   return NULL;
   }
}


int hash(const char *str){
//index ctr to 0
int i = 0;

//set key to first char
int key = (int)str[0];

//if length > 1
if(str[1] != '\0')
   while(str[i+1] != '\0')
      {
      key = (key*26 + (int)str[i+1] ) % TABLESIZE; //horners method
      i++;
      }
else //lenght is 1
   key = key % TABLESIZE;

//printf(" key is %d", key);
return key; 
}

void reset(SYMBOL **s){
*s = (SYMBOL*)calloc(1,sizeof(SYMBOL));
}

void prTable(SYMT *target){
/*
int i = 0;
int j = 0;

SYMBOL* nxt;

if(!target->symT)
   return;
printf("\n====================\n{");
for(i; i < TABLESIZE; i++)
   {
   if(target->symT[i])
      {
      printf(" %s %s",labels[target->symT[i]->t], target->symT[i]->id);
      if(target->symT[i]->func == true)
         {
         printf("(");
         while(j < target->symT[i]->nparams)
            {
            printf("%s, ", labels[ target->symT[i]->ptypes[j++] ]);
            }
         printf(")");
         }
      if(target->symT[i]->array == true)
          printf("[%d]", target->symT[i]->arrsize);
      printf(",");
      if(nxt = target->symT[i]->next)
         printf("->%s %s,",labels[nxt->t], nxt->id);
      }
   }
printf("}\n===================\n");

if(target->prev)
   prTable(target->prev);

*/
 
}
