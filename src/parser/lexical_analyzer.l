%option noyywrap

%{

/*****************声明和选项设置  begin*****************/

#include <stdio.h>
#include <stdlib.h>


#include "syntax_tree.h"
#include "syntax_analyzer.h"



int lines;
int pos_start;
int pos_end;
char a;



void pass_node(char *text){
     yylval.node = new_syntax_tree_node(text);
}

/*****************声明和选项设置  end*****************/

%}





%%

 /* to do for students */
 /* two cases for you, pass_node will send flex's token to bison */
 
\+ 	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return ADD; }
\- {pos_start=pos_end; pos_end+=1; pass_node(yytext); return SUB; }
\* {pos_start=pos_end; pos_end+=1; pass_node(yytext); return MUL; }
\/ {pos_start=pos_end; pos_end+=1; pass_node(yytext); return DIV; }
\<\= {pos_start=pos_end; pos_end+=2; pass_node(yytext); return LToE; }
\< {pos_start=pos_end; pos_end+=1; pass_node(yytext); return LT; }
\> {pos_start=pos_end; pos_end+=1; pass_node(yytext); return GT; }
\>\= {pos_start=pos_end; pos_end+=2; pass_node(yytext); return GToE; }
\=\= {pos_start=pos_end; pos_end+=2; pass_node(yytext); return E; }
\!\= {pos_start=pos_end; pos_end+=2; pass_node(yytext); return UE; }
\= {pos_start=pos_end; pos_end+=1; pass_node(yytext); return ASSIGN; }
\; {pos_start=pos_end; pos_end+=1; pass_node(yytext); return SEMICOLON; }
\, {pos_start=pos_end; pos_end+=1; pass_node(yytext); return COMMA; }
\( {pos_start=pos_end; pos_end+=1; pass_node(yytext); return L_PARENTHESE; }
\) {pos_start=pos_end; pos_end+=1; pass_node(yytext); return R_PARENTHESE; }
\[ {pos_start=pos_end; pos_end+=1; pass_node(yytext); return L_SBRACKET; }
\] {pos_start=pos_end; pos_end+=1; pass_node(yytext); return R_SBRACKET; }
\{ {pos_start=pos_end; pos_end+=1; pass_node(yytext); return L_CBRACKET; }
\} {pos_start=pos_end; pos_end+=1; pass_node(yytext); return R_CBRACKET; }
else {pos_start=pos_end; pos_end+=4; pass_node(yytext); return ELSE;}
if {pos_start=pos_end; pos_end+=2; pass_node(yytext); return IF;}
int {pos_start=pos_end; pos_end+=3; pass_node(yytext); return INT;}
return {pos_start=pos_end; pos_end+=6; pass_node(yytext); return RETURN;}
void {pos_start=pos_end; pos_end+=4; pass_node(yytext); return VOID;}
while {pos_start=pos_end; pos_end+=5; pass_node(yytext); return WHILE;}
float {pos_start=pos_end; pos_end+=5; pass_node(yytext); return FLOAT;}
[0-9]+ {pos_start=pos_end; pos_end+=strlen(yytext); pass_node(yytext); return NUM;}
[a-zA-Z]+ {pos_start=pos_end; pos_end+=strlen(yytext); pass_node(yytext); return ID;}
([0-9]+\.|[0-9]*\.[0-9]+) {pos_start=pos_end; pos_end+=strlen(yytext); pass_node(yytext); return FLOATNUM;}
\n {lines+=1;}
\r {}
\t {pos_start=pos_end; pos_end+=4;}
" " {pos_start=pos_end; pos_end+=1;}
\/\* {
          pos_start=pos_end; 
          int state=0;
          a='1';
          while(a&&state!=2)
          {
               a=input();
               if(a=='\n')
               {
                    pos_end+=1;
                    lines+=1;
               }
               if(a=='*')
               {
                    state=1;
               }
               else if(a=='/' && state==1)
               {
                    state=2;
               }     
               else
               {
                    state=0;
               }
          }
     }
. { pos_start = pos_end; pos_end++; return ERROR; }



 /****请在此补全所有flex的模式与动作  end******/

%%

