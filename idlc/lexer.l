%option noyywrap never-interactive

%{
#include "parser.h"
#include <string>
#include <iostream>
std::string g_sBuf; 
#ifdef _WIN32
#define strdup _strdup
#endif
%}

%x STATE_COMMENT STATE_LITERAL

varname		[A-Za-z_][A-Za-z0-9_]*
integer		[0-9]*|"0x"[A-Fa-f0-9]*
open_brace 	"\{"
close_brace 	"\}"
open_bracket 	"\["
close_bracket	"\]"
open_parenthesis "("
close_parenthesis ")"
colon		":"
semi_colon	";"
comma		","
less		"<"
more		">"
equal 		"="
newline		"\n"
whitespace	"[ \t\n\r]"
line_comment	"//"
open_comment	"/*"
close_comment	"*/"
literal_block	"%%"
language	".cpp"|".py"|".java"|".cs"

message		"message"
struct		"struct"
enum		"enum"
typedef 	"typedef"
boolean		"boolean"
ubyte		"ubyte"
byte		"byte"
int16		"int16"
int32		"int32"
int64		"int64"
uint16		"uint16"
uint32		"uint32"
uint64		"uint64"
double		"double"
float		"float"
string		"string"
map			"map"
array		"array"
list		"list"

%%	

<STATE_COMMENT>{close_comment} 	{ BEGIN INITIAL; return COMMENT; }
<STATE_COMMENT>.		{ }
<STATE_COMMENT>\n		{ yylineno++; }
{open_comment}			{ BEGIN STATE_COMMENT; }
{line_comment}.*	{ return COMMENT; }
<STATE_LITERAL>{literal_block} {
								BEGIN INITIAL;
								yylval.str = strdup(g_sBuf.c_str());
								return LITERAL;
							}
<STATE_LITERAL>.		{ g_sBuf.append(1, (unsigned char)yytext[0]); }
<STATE_LITERAL>\n		{ g_sBuf.append(1, (unsigned char)yytext[0]); yylineno++; }
{literal_block}				{ 
								g_sBuf = "";
								BEGIN STATE_LITERAL; 
							}
{language}		{ yylval.str = strdup(yytext); return LANGUAGE; }
{message}		{ return MESSAGE; }
{struct}		{ return STRUCT; }
{enum}			{ return ENUM; }
{boolean}		{ return BOOLEAN; }
{ubyte}			{ return UBYTE; }
{byte}			{ return BYTE; }
{int16}			{ return INT16; }
{int32}			{ return INT32; }
{int64}			{ return INT64; }
{uint16}		{ return UINT16; }
{uint32}		{ return UINT32; }
{uint64}		{ return UINT64; }
{double}		{ return DOUBLE; }
{float}			{ return FLOAT;	};
{string}		{ return STRING; }
{map}			{ return MAP; }
{array}			{ return ARRAY; }
{list}			{ return LIST; }
{typedef}		{ return TYPEDEF; }
{open_brace}		{ return OPEN_BRACE; }
{close_brace}		{ return CLOSE_BRACE; }
{open_bracket}		{ return OPEN_BRACKET; }
{close_bracket}		{ return CLOSE_BRACKET; }
{open_parenthesis}	{ return OPEN_PARENTHESIS; }
{close_parenthesis}	{ return CLOSE_PARENTHESIS; }
{semi_colon}		{ return SEMI_COLON; }
{colon}			{ return COLON; }
{comma}			{ return COMMA; }
{less}			{ return LESS; }
{more}			{ return MORE; }
{equal}			{ return EQUAL; }
{integer}		{ yylval.str = strdup(yytext); return INTEGER; }
{varname}		{ yylval.str = strdup(yytext); return VARNAME; }
{whitespace}+ 	{ }
\n				{ yylineno++;
				  yyless(1);
				}

%%

/**************   Programs Section   **************/

