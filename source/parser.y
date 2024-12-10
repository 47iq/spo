%{
#include "parser.tab.h"
#include "error.h"

#define YYDEBUG 1
%}
%define parse.error verbose

%union {
    TreeNode* node;
}

%token <node> PLUS MINUS MUL DIV PERCENT EQ NEQ ASSIGN
%token <node> LT GT LTEQ GTEQ
%token <node> AND OR NOT
%token <node> OF
%token <node> ARRAY
%token <node> DEF END BEGIN_BLOCK
%token <node> IDENTIFIER
%token <node> STR
%token <node> COMMA
%token <node> DOUBLE_DOT
%token <node> CHAR
%token <node> BIN HEX DEC
%token <node> TRUE FALSE
%token <node> IF ELSE THEN WHILE UNTIL DO BREAK
%token <node> SEMICOLON
%token <node> LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token <node> TYPEDEF

%left ASSIGN
%left AND OR
%left EQ NEQ
%left LT GT LTEQ GTEQ
%left PLUS MINUS
%left MUL DIV PERCENT

%type <node> typeRef
%type <node> funcSignature
%type <node> arg
%type <node> source
%type <node> sourceItem
%type <node> sourceItemList
%type <node> statement
%type <node> if
%type <node> block
%type <node> loop
%type <node> repeat
%type <node> break
%type <node> expression
%type <node> builtin
%type <node> custom
%type <node> array
%type <node> argList
%type <node> argListItems
%type <node> typeRefOptional
%type <node> literal
%type <node> place
%type <node> slice
%type <node> range
%type <node> rangeList
%type <node> expr
%type <node> exprList
%type <node> exprListItems
%type <node> call
%type <node> braces
%type <node> unary
%type <node> binary
%type <node> elseOptional
%type <node> statementList


%%

/* source */

source: sourceItemList {{TreeNode* nodes[] = {$1}; $$ = createNode("source", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};


/* sourceItem */

sourceItemList: sourceItem sourceItemList {{TreeNode* nodes[] = {$1, $2}; $$ = createNode("sourceItemList", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | {{$$ = NULL;}};

sourceItem: DEF funcSignature statementList END {{TreeNode* nodes[] = {$2, $3}; $$ = createNode("sourceItem", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}


/* funcSignature */

funcSignature: IDENTIFIER LPAREN argList RPAREN typeRefOptional {{TreeNode* nodes[] = {$3, $5}; $$ = createNode("funcSignature", nodes, sizeof(nodes) / sizeof(nodes[0]), $1->value);}};

argList: argListItems   {{TreeNode* nodes[] = {$1}; $$ = createNode("argList", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | {{$$ = NULL;}}

argListItems: arg            {{TreeNode* nodes[] = {$1}; $$ = createNode("argListItems", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | arg COMMA argListItems {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("argListItems", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

arg: IDENTIFIER typeRefOptional {{TreeNode* nodes[] = {$1, $2}; $$ = createNode("arg", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

typeRefOptional:    {{ $$ = NULL; }}
    | LPAREN OF typeRef RPAREN {{$$ = $3;}};

typeRef: builtin    {{$$ = $1;}}
    | custom        {{$$ = $1;}}
    | array         {{$$ = $1;}};

builtin: TYPEDEF    {{$$ = $1;}};

custom: IDENTIFIER  {{$$ = $1;}};

array: typeRef ARRAY LBRACKET DEC RBRACKET {{TreeNode* nodes[] = {$1}; $$ = createNode("array", nodes, sizeof(nodes) / sizeof(nodes[0]), $4->value);}};


/* statement */

statementList: statement statementList {{TreeNode* nodes[] = {$1, $2}; $$ = createNode("statementList", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | {{$$ = NULL;}};

statement: if            {{$$ =  $1;}}
    | loop          {{$$ =  $1;}}
    | repeat        {{$$ =  $1;}}
    | block         {{$$ =  $1;}}
    | break         {{$$ =  $1;}}
    | expression    {{$$ =  $1;}};

if: IF expr THEN statement elseOptional {{TreeNode* nodes[] = {$2, $4, $5}; $$ = createNode("if", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

elseOptional: ELSE statement            {{TreeNode* nodes[] = {$2}; $$ = createNode("else", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    |                                   {{$$ = NULL;}};

loop: WHILE expr statementList END      {{TreeNode* nodes[] = {$2, $3}; $$ = createNode("loop", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | UNTIL expr statementList END      {{TreeNode* nodes[] = {$2, $3}; $$ = createNode("loop", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

repeat: statement WHILE expr SEMICOLON  {{TreeNode* nodes[] = {$3, $1}; $$ = createNode("repeat", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | statement UNTIL expr SEMICOLON    {{TreeNode* nodes[] = {$3, $1}; $$ = createNode("repeat", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

break: BREAK SEMICOLON {{$$ = createNode("break", NULL, 0, "");}};

expression: expr SEMICOLON {{$$ = $1;}};

block: LBRACE statementList RBRACE      {{TreeNode* nodes[] = {$2}; $$ = createNode("block", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | BEGIN_BLOCK statementList END     {{TreeNode* nodes[] = {$2}; $$ = createNode("block", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | LBRACE sourceItemList RBRACE      {{TreeNode* nodes[] = {$2}; $$ = createNode("block", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | BEGIN_BLOCK sourceItemList END    {{TreeNode* nodes[] = {$2}; $$ = createNode("block", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | LBRACE RBRACE                     {{$$ = createNode("block", NULL, 0, "");}}
    | BEGIN_BLOCK END                   {{$$ = createNode("block", NULL, 0, "");}};


/* expr */

expr: binary    {{$$ = $1;}}
    | unary     {{$$ = $1;}}
    | braces    {{$$ = $1;}}
    | call      {{$$ = $1;}}
    | slice     {{$$ = $1;}}
    | place     {{$$ = $1;}}
    | literal   {{$$ = $1;}};

binary: expr ASSIGN expr         {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("ASSIGN", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr PLUS expr            {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("PLUS", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr MINUS expr           {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("MINUS", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr MUL expr            {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("MUL", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr DIV expr             {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("DIV", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr PERCENT expr         {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("PERCENT", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr EQ expr     {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("EQITY", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr NEQ expr        {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("NEQ", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr LT expr        {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("LT", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr GT expr     {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("GT", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr LTEQ expr      {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("LTEQ", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr GTEQ expr   {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("GTEQ", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr AND expr             {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("AND", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr OR expr              {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("OR", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

unary: PLUS expr                {{TreeNode* nodes[] = {$2}; $$ = createNode("PLUS", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | MINUS expr                {{TreeNode* nodes[] = {$2}; $$ = createNode("MINUS", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | NOT expr                  {{TreeNode* nodes[] = {$2}; $$ = createNode("NOT", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

braces: LPAREN expr RPAREN      {{TreeNode* nodes[] = {$2}; $$ = createNode("braces", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

call: expr LPAREN exprList RPAREN  {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("call", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

slice: expr LBRACKET rangeList RBRACKET {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("slice", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

range: expr DOUBLE_DOT expr {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("range", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr {{TreeNode* nodes[] = {$1}; $$ = createNode("range", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

rangeList: range COMMA rangeList {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("rangeList", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | range {{TreeNode* nodes[] = {$1}; $$ = createNode("rangeList", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};

place: IDENTIFIER {{$$ = $1;}};

literal: TRUE   {{$$ = $1;}}
    | FALSE     {{$$ = $1;}}
    | STR       {{$$ = $1;}}
    | CHAR      {{$$ = $1;}}
    | HEX       {{$$ = $1;}}
    | BIN       {{$$ = $1;}}
    | DEC       {{$$ = $1;}};

exprList: exprListItems  {{TreeNode* nodes[] = {$1}; $$ = createNode("exprList", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | {{$$ = NULL;}};

exprListItems: expr COMMA exprListItems {{TreeNode* nodes[] = {$1, $3}; $$ = createNode("exprListItems", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}}
    | expr {{TreeNode* nodes[] = {$1}; $$ = createNode("exprListItems", nodes, sizeof(nodes) / sizeof(nodes[0]), "");}};
%%