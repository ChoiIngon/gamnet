/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* "%code requires" blocks.  */

/* Line 2068 of yacc.c  */
#line 12 "parser.y"

#include "token.h"
Token::StmtList* MakeParseTree(const std::string& sFileName);



/* Line 2068 of yacc.c  */
#line 45 "parser.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     VARNAME = 258,
     INTEGER = 259,
     LITERAL = 260,
     LANGUAGE = 261,
     MESSAGE = 262,
     STRUCT = 263,
     TYPEDEF = 264,
     ENUM = 265,
     COMMENT = 266,
     OPEN_BRACE = 267,
     CLOSE_BRACE = 268,
     OPEN_BRACKET = 269,
     CLOSE_BRACKET = 270,
     LESS = 271,
     MORE = 272,
     EQUAL = 273,
     COLON = 274,
     SEMI_COLON = 275,
     COMMA = 276,
     OPEN_PARENTHESIS = 277,
     CLOSE_PARENTHESIS = 278,
     BOOLEAN = 279,
     BYTE = 280,
     UBYTE = 281,
     INT16 = 282,
     INT32 = 283,
     INT64 = 284,
     UINT16 = 285,
     UINT32 = 286,
     UINT64 = 287,
     DOUBLE = 288,
     FLOAT = 289,
     STRING = 290,
     ARRAY = 291,
     LIST = 292,
     MAP = 293
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 17 "parser.y"

	const char* str;
	Token::Base* tok;



/* Line 2068 of yacc.c  */
#line 107 "parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


