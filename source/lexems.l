%{
#include "error.h"
#include "parser.tab.h"
#include <string.h>
%}

%option yylineno

%%

[0-9]+ {
    yylval.node = createNode("DEC", NULL, 0, yytext);
    return DEC;
}
\"[^\"\\]*(?:\\.[^\"\\]*)*\" {
    char buffer[1024];
    strncpy(buffer, yytext + 1, strlen(yytext) - 2);
    buffer[strlen(buffer) - 1] = 0;
    yylval.node = createNode("STR", NULL, 0, buffer);
    return STR;
}
'[^']' {
    char buffer[1];
    strncpy(buffer, yytext + 1, 1);
    yylval.node = createNode("CHAR", NULL, 0, buffer);
    return CHAR;
}
0[xX][0-9A-Fa-f]+ {
    char* buffer = malloc(256);
    sprintf(buffer, "%ld", strtol(yytext + 2, NULL, 16));
    yylval.node = createNode("HEX", NULL, 0, buffer);
    return HEX;
}
0[bB][01]+ {
    char* buffer = malloc(256);
    sprintf(buffer, "%ld", strtol(yytext + 2, NULL, 2));
    yylval.node = createNode("BIN", NULL, 0, buffer);
    return BIN;
}
"byte" {
           yylval.node = createNode("TYPEDEF", NULL, 0, yytext);
           return TYPEDEF;
       }
"int" {
          yylval.node = createNode("TYPEDEF", NULL, 0, yytext);
          return TYPEDEF;
      }
"uint" {
           yylval.node = createNode("TYPEDEF", NULL, 0, yytext);
           return TYPEDEF;
       }
"long" {
           yylval.node = createNode("TYPEDEF", NULL, 0, yytext);
           return TYPEDEF;
       }
"bool" {
           yylval.node = createNode("TYPEDEF", NULL, 0, yytext);
           return TYPEDEF;
       }
"ulong" {
            yylval.node = createNode("TYPEDEF", NULL, 0, yytext);
            return TYPEDEF;
        }
"char" {
           yylval.node = createNode("TYPEDEF", NULL, 0, yytext);
           return TYPEDEF;
       }
"string"  {
              yylval.node = createNode("TYPEDEF", NULL, 0, yytext);
              return TYPEDEF;
          }
"if"                                {
 yylval.node = createNode("IF", NULL, 0, "");
 return IF;
 }
"of"                                {
 yylval.node = createNode("OF", NULL, 0, "");
 return OF;
 }
"array"                                {
 yylval.node = createNode("ARRAY", NULL, 0, "");
 return ARRAY;
 }
"def"                                {
 yylval.node = createNode("DEF", NULL, 0, "");
 return DEF;
 }
"end"                                {
 yylval.node = createNode("END", NULL, 0, "");
 return END;
 }
"begin"                                {
 yylval.node = createNode("BEGIN_BLOCK", NULL, 0, "");
 return BEGIN_BLOCK;
 }
"else"                              {
 yylval.node = createNode("ELSE", NULL, 0, "");
 return ELSE;
 }
"then"                              {
 yylval.node = createNode("THEN", NULL, 0, "");
 return THEN;
 }
"do"                                {
 yylval.node = createNode("DO", NULL, 0, "");
 return DO;
 }
"while"                             {
 yylval.node = createNode("WHILE", NULL, 0, "");
 return WHILE;
 }
"until"                             {
 yylval.node = createNode("UNTIL", NULL, 0, "");
 return UNTIL;
 }
"break"                             {
 yylval.node = createNode("BREAK", NULL, 0, "");
 return BREAK;
 }
"{"                                 {
 return LBRACE;
 }
"}"                                 {
 return RBRACE;
 }
"["                                 {
 return LBRACKET;
 }
"]"                                 {
 return RBRACKET;
 }
"true"                              {
 yylval.node = createNode("TRUE", NULL, 0, "");
 return TRUE;
 }
"false"                             {
 yylval.node = createNode("FALSE", NULL, 0, "");
 return FALSE;
 }
"%"                                 {
 return PERCENT;
 }
".."                                 {
 return DOUBLE_DOT;
 }
";"                                 {
 return SEMICOLON;
 }
","                                 {
 return COMMA;
 }
"!="                                {
 return NEQ;
 }
"!"                                 {
 return NOT;
 }
"\|\|"                              {
 return OR;
 }
"&&"                                {
 return AND;
 }
"+"                                 {
 return PLUS;
 }
"-"                                 {
 return MINUS;
 }
"<="                                {
 return LTEQ;
 }
">="                                {
 return GTEQ;
 }
"<"                                 {
 return LT;
 }
">"                                 {
 return GT;
 }
"*"                                 {
 return MUL;
 }
"\/"                                 {
  return DIV;
}
"("                                 {
 return LPAREN;
 }
")"                                 {
 return RPAREN;
 }
"=="                                 {
 return EQ;
 }
"="                                 {
 return ASSIGN;
 }
[a-zA-Z_][a-zA-Z_0-9]* {
    yylval.node = createNode("IDENTIFIER", NULL, 0, yytext);
    return IDENTIFIER;
}
\/\/.*
\n
[ \t]
.
%%


int yywrap() {
    // Your implementation here, typically returning 1 to signal EOF.
    return 1;
}