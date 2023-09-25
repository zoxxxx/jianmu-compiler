%option noyywrap
%{
/*****************声明和选项设置  begin*****************/
#include <stdio.h>
#include <stdlib.h>

#include "syntax_tree.h"
#include "syntax_analyzer.h"

int lines = 1;
int pos_start = 1;
int pos_end = 1;

void pass_node(char *text){
     yylval.node = new_syntax_tree_node(text);
}

/*****************声明和选项设置  end*****************/

%}

letter   [a-zA-Z]
digit    [0-9]


%x comment
%%
 /* to do for students */
 /* two cases for you, pass_node will send flex's token to bison */
\+ 	{pos_start = pos_end; pos_end ++; pass_node(yytext); return ADD;}
-    {pos_start = pos_end; pos_end ++; pass_node(yytext); return SUB;}
\*   {pos_start = pos_end; pos_end ++; pass_node(yytext); return MUL;}
\/   {pos_start = pos_end; pos_end ++; pass_node(yytext); return DIV;}
\(   {pos_start = pos_end; pos_end ++; pass_node(yytext); return LP;}
\)   {pos_start = pos_end; pos_end ++; pass_node(yytext); return RP;}
\<=  {pos_start = pos_end; pos_end += 2; pass_node(yytext); return LE;}
\>=  {pos_start = pos_end; pos_end += 2; pass_node(yytext); return GE;}
\<   {pos_start = pos_end; pos_end ++; pass_node(yytext); return LT;}
\>   {pos_start = pos_end; pos_end ++; pass_node(yytext); return GT;}
==   {pos_start = pos_end; pos_end += 2; pass_node(yytext); return EQ;}
!=   {pos_start = pos_end; pos_end += 2; pass_node(yytext); return NE;}
;    {pos_start = pos_end; pos_end ++; pass_node(yytext); return SEMI;}
=    {pos_start = pos_end; pos_end ++; pass_node(yytext); return ASSIGN;}
,    {pos_start = pos_end; pos_end ++; pass_node(yytext); return COMMA;}
\{   {pos_start = pos_end; pos_end ++; pass_node(yytext); return LBRACE;}
\}   {pos_start = pos_end; pos_end ++; pass_node(yytext); return RBRACE;}
\[   {pos_start = pos_end; pos_end ++; pass_node(yytext); return LBRACK;}
\]   {pos_start = pos_end; pos_end ++; pass_node(yytext); return RBRACK;}

else      {pos_start = pos_end; pos_end += 4; pass_node(yytext); return ELSE;}
if        {pos_start = pos_end; pos_end += 2; pass_node(yytext); return IF;}
int       {pos_start = pos_end; pos_end += 3; pass_node(yytext); return INT;}
return    {pos_start = pos_end; pos_end += 6; pass_node(yytext); return RETURN;}
void      {pos_start = pos_end; pos_end += 4; pass_node(yytext); return VOID;}
while     {pos_start = pos_end; pos_end += 5; pass_node(yytext); return WHILE;}
float     {pos_start = pos_end; pos_end += 5; pass_node(yytext); return FLOAT;}

{digit}+  {pos_start = pos_end; pos_end += strlen(yytext); pass_node(yytext); return INTEGER;}
{letter}+ {pos_start = pos_end; pos_end += strlen(yytext); pass_node(yytext); return ID;}
({digit}+\.|{digit}*\.{digit}+) {pos_start = pos_end; pos_end += strlen(yytext); pass_node(yytext); return FLOATPOINT;}

" "|\t    { pos_start = pos_end; pos_end++; }
\n|\r|\n\r { pos_start = pos_end = 1; lines++; }

 /*using start condition to deal with the comments*/
\/\*                     { BEGIN(comment); }
<comment>[^*\n\r]*       { }
<comment>\*+[^*/\n\r]*   { }
<comment>\n|\r|\n\r      lines ++;
<comment>\*\/            BEGIN(INITIAL);

. { pos_start = pos_end; pos_end++; return ERROR; }

 /****请在此补全所有flex的模式与动作  end******/
%%
