%{
#include <string>
#include "lexer.h" 
int nLineNo = 0;
void yyerror(const char* text);
//void yyerrorEx(int line_no, const char* text, const char* error_code);

#include "token.h"
Token::StmtList* g_pRoot = NULL;
%}

%code requires {
#include "token.h"
Token::StmtList* MakeParseTree(const std::string& sFileName);
}

%union {
	const char* str;
	Token::Base* tok;
}

%type <tok> stmt stmt_list
%type <tok> typedef message enum literal struct
%type <tok> var_name user_define var_type var_decl var_decl_list enum_decl enum_decl_list
%token <str> VARNAME INTEGER LITERAL LANGUAGE
%token MESSAGE STRUCT TYPEDEF ENUM COMMENT 
%token OPEN_BRACE CLOSE_BRACE OPEN_BRACKET CLOSE_BRACKET LESS MORE EQUAL COLON SEMI_COLON COMMA OPEN_PARENTHESIS CLOSE_PARENTHESIS
%token BOOLEAN BYTE UBYTE INT16 INT32 INT64 UINT16 UINT32 UINT64 DOUBLE FLOAT STRING ARRAY LIST MAP
%%

stmt_list	: stmt {
				g_pRoot = new Token::StmtList("stmt");
				g_pRoot->list_.push_back($1);
			}
		| stmt_list stmt {
				g_pRoot->list_.push_back($2);
			}
		;

stmt 		: message { $$ = $1; }
			| struct  { $$ = $1; } 
			| typedef { $$ = $1; }
			| enum	  { $$ = $1; }
			| literal { $$ = $1; } 
			| COMMENT { $$ = new Token::Base("comment"); }			 
			;

typedef		: TYPEDEF var_decl {
				Token::Typedef* pTypedef = new Token::Typedef("typedef", (Token::VarDecl*)$2);
				$$ = pTypedef;
			}
		;

message		: MESSAGE var_name COLON INTEGER OPEN_BRACE var_decl_list CLOSE_BRACE SEMI_COLON {
				int nSeqNo = ::atoi($4); 
				Token::Message* pMessage = new Token::Message($2->GetName().c_str(), "", nSeqNo);
				pMessage->list_ = ((Token::List*)$6)->list_;	
				$$ = pMessage;
			}
		| MESSAGE var_name COLON INTEGER OPEN_BRACE CLOSE_BRACE SEMI_COLON {
				int nSeqNo = ::atoi($4); 
				Token::Message* pMessage = new Token::Message($2->GetName().c_str(), "", nSeqNo);
				$$ = pMessage;
			}
		| MESSAGE var_name OPEN_PARENTHESIS var_name CLOSE_PARENTHESIS COLON INTEGER OPEN_BRACE var_decl_list CLOSE_BRACE SEMI_COLON {
				int nSeqNo = ::atoi($7); 
				Token::Message* pMessage = new Token::Message($2->GetName().c_str(), $4->GetName().c_str(), nSeqNo);
				pMessage->list_ = ((Token::List*)$9)->list_;	
				$$ = pMessage;
			}
		| MESSAGE var_name OPEN_PARENTHESIS var_name CLOSE_PARENTHESIS COLON INTEGER OPEN_BRACE CLOSE_BRACE SEMI_COLON {
				int nSeqNo = ::atoi($7); 
				Token::Message* pMessage = new Token::Message($2->GetName().c_str(), $4->GetName().c_str(), nSeqNo);
				$$ = pMessage;
			}
		;

struct	: STRUCT var_name OPEN_BRACE var_decl_list CLOSE_BRACE SEMI_COLON {
				Token::Message* pMessage = new Token::Message($2->GetName().c_str(), "", -1);
				pMessage->list_ = ((Token::List*)$4)->list_;	
				$$ = pMessage;
			}
		| STRUCT var_name OPEN_BRACE CLOSE_BRACE SEMI_COLON {
				Token::Message* pMessage = new Token::Message($2->GetName().c_str(), "", -1);
				$$ = pMessage;
			}
		| STRUCT var_name OPEN_PARENTHESIS var_name CLOSE_PARENTHESIS OPEN_BRACE var_decl_list CLOSE_BRACE SEMI_COLON {
				Token::Message* pMessage = new Token::Message($2->GetName().c_str(), $4->GetName().c_str(), -1);
				pMessage->list_ = ((Token::List*)$7)->list_;	
				$$ = pMessage;
			}
		| STRUCT var_name OPEN_PARENTHESIS var_name CLOSE_PARENTHESIS OPEN_BRACE CLOSE_BRACE SEMI_COLON {
				Token::Message* pMessage = new Token::Message($2->GetName().c_str(), $4->GetName().c_str(), -1);
				$$ = pMessage;
			}
		;
enum 	: 	ENUM var_name OPEN_BRACE CLOSE_BRACE SEMI_COLON {
			Token::Enum* pEnum = new Token::Enum($2->GetName().c_str()); 
			$$ = pEnum; 
		}
		| ENUM var_name OPEN_BRACE enum_decl_list CLOSE_BRACE SEMI_COLON { 
			Token::Enum* pEnum = new Token::Enum($2->GetName().c_str()); 
			$$ = pEnum; 
			pEnum->list_ = ((Token::List*)$4)->list_; 
		};

enum_decl_list	: 	enum_decl {
				Token::List* pList = new Token::List("enum_decl_list");
				pList->list_.push_back((Token::EnumElmt*)$1);
				$$ = pList;
			}
			| enum_decl_list enum_decl {
				Token::List* pList = (Token::List*)$1;
				pList->list_.push_back((Token::EnumElmt*)$2);
				$$ = pList;
			};
								
enum_decl	: var_name EQUAL INTEGER {
			$$ = new Token::EnumElmt("enum_decl", $1, $3); 
		} 
		| var_name EQUAL INTEGER COMMA {
			$$ = new Token::EnumElmt("enum_decl", $1, $3); 
		} 
		| var_name {
			$$ = new Token::EnumElmt("enum_decl", $1, ""); 
		} 
		| var_name COMMA {
			$$ = new Token::EnumElmt("enum_decl", $1, ""); 
		} 
		| COMMENT { 
			$$ = new Token::Base("comment"); 
		};

var_decl_list   
		: var_decl { 
			Token::List* pList = new Token::List("var_decl"); 
			pList->list_.push_back((Token::VarDecl*)$1); 
			$$ = pList; 
		} 
		| var_decl_list var_decl { 
			Token::List* pList = (Token::List*)$1; 
			pList->list_.push_back((Token::VarDecl*)$2); 
			$$ = pList; 
		};

var_decl	
		: var_type var_name SEMI_COLON  { 
			$$ = new Token::VarDecl("var_decl", $1, $2); 
		}
		| literal { 
			$$ = $1; 
		} 
		| COMMENT { 
			$$ = new Token::Base("comment"); 
		};

literal		: LANGUAGE LITERAL { $$ = new Token::LiteralBlock("literal", $1, $2);}
			;

var_type	: BOOLEAN { $$ = new Token::PrimitiveType("boolean"); }
			| BYTE { $$ = new Token::PrimitiveType("byte"); }
			| UBYTE { $$ = new Token::PrimitiveType("ubyte"); }
			| INT16 { $$ = new Token::PrimitiveType("int16"); }
			| INT32 { $$ = new Token::PrimitiveType("int32"); }
			| INT64 { $$ = new Token::PrimitiveType("int64"); }
			| UINT16 { $$ = new Token::PrimitiveType("uint16"); }
			| UINT32 { $$ = new Token::PrimitiveType("uint32"); }
			| UINT64 { $$ = new Token::PrimitiveType("uint64"); }
			| DOUBLE { $$ = new Token::PrimitiveType("double"); }
			| FLOAT { $$ = new Token::PrimitiveType("float"); }
			| STRING { $$ = new Token::StringType("string"); }
			| ARRAY LESS var_type MORE { $$ = new Token::ArrayType("array", $3); }
			| ARRAY LESS var_type COMMA INTEGER MORE { 
				$$ = new Token::StaticArrayType("static_array", $3, $5); 
				if(Token::TYPE_PRIMITIVE == $3->Type())
				{
					((Token::StaticArrayType*)$$)->m_bPrimitive = true;
				}
			}
			| LIST LESS var_type MORE { $$ = new Token::ListType("list", $3); }
			| MAP LESS var_type COMMA var_type MORE { $$ = new Token::MapType("map", $3, $5); }
			| user_define { $$ = new Token::UserDefineType($1->GetName().c_str()); }
			; 

user_define : VARNAME { $$ = new Token::VarName($1); }
var_name	: VARNAME { $$ = new Token::VarName($1); }
			;
%% 

static char sFilePath[255] = {0};
Token::StmtList* MakeParseTree(const std::string& sFileName) 
{
	strcpy(sFilePath, sFileName.c_str());
	yyin = fopen(sFilePath, "r");
	if(NULL == yyin)
	{
		const std::string err = std::string("can't open file : ") + sFileName;
		yyerror(err.c_str());
		return NULL;
	}
	
	yyparse();
	fclose(yyin);
	return g_pRoot;	
}

void yyerror(const char* text)
{
	std::cerr << "error at " << sFilePath << "(" << yylineno << "):" << text << " before \"" << yytext << "\"" << std::endl;
	exit(1);
}


