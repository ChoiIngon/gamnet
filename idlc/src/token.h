#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <iostream>
#include <list>
#include <string>
#include <map>
namespace Token {

enum TYPE {
	TYPE_NULL,
	TYPE_VARNAME,
	TYPE_PRIMITIVE,
	TYPE_STRING,
	TYPE_USERDEFINE,
	TYPE_MAP,
	TYPE_ARRAY,
	TYPE_STATIC_ARRAY,
	TYPE_LIST,
	TYPE_VARDECL,
	TYPE_MESSAGE,
	TYPE_TYPEDEF,
	TYPE_STMTLIST,
	TYPE_ENUM,
	TYPE_ENUMELMT,
	TYPE_LITERALBLOCK
};

struct Base 
{
	Base(const char* sName) : m_sName(sName) {
		//std::cerr << __FUNCTION__ << "(" << GetName() << ")" << std::endl;
	}
	virtual ~Base() {}
	const std::string m_sName;
	const std::string& GetName() const {
		return m_sName;
	}
	virtual TYPE Type() const 
	{
		return TYPE_NULL;
	}
};

struct LiteralBlock : public Base {
	LiteralBlock(const char* sName, const char* sLanguage, const char* sContext) : Base(sName), m_sLanguage(sLanguage), m_sContext(sContext) {
		//std::cerr << __FUNCTION__ << "(" << GetName() << ", " << m_sLanguage << ")" << std::endl;
	}
	virtual TYPE Type() const
	{
		return TYPE_LITERALBLOCK;
	}
	const std::string m_sLanguage;
	const std::string m_sContext;
};

struct VarName : public Base {
	VarName(const char* sName) : Base(sName) {
		//std::cerr << __FUNCTION__ << "(" << GetName() << ")" << std::endl;
	}
	virtual TYPE Type() const 
	{
		return TYPE_VARNAME;
	}
};

struct PrimitiveType : public Base {
	PrimitiveType (const char* sName) : Base(sName) {}
	virtual TYPE Type() const 
	{
		return TYPE_PRIMITIVE;
	}
};

struct StringType : public Base {
	StringType(const char* sName) : Base(sName) {	}
	virtual TYPE Type() const
	{
		return TYPE_STRING;
	}
};

struct UserDefineType : public  Base {
	UserDefineType(const char* sName) : Base(sName) {}
	virtual TYPE Type() const 
	{
		return TYPE_USERDEFINE;
	}
};

struct ListType : public Base {
	ListType (const char* sName, Base* pElmtType) : Base(sName), m_pElmtType(pElmtType) {	}
	Base* m_pElmtType;
	virtual TYPE Type() const 
	{
		return TYPE_LIST;
	}
};

struct ArrayType : public Base {
	ArrayType (const char* sName, Base* pElmtType) : Base(sName), m_pElmtType(pElmtType) {}
	Base* m_pElmtType;
	virtual TYPE Type() const 
	{
		return TYPE_ARRAY;
	}
};

struct StaticArrayType : public Base {
	StaticArrayType(const char* sName, Base* pElmtType, const char* sElmtCount) : Base(sName),  m_pElmtType(pElmtType), m_sElmtCount(sElmtCount), m_bPrimitive(false) {}
	Base* m_pElmtType;
	std::string m_sElmtCount;
	bool m_bPrimitive;
	virtual TYPE Type() const 
	{
		return TYPE_STATIC_ARRAY;
	}
};

struct MapType : public Base {
	MapType (const char* sName, Base* pKeyType, Base* pElmtType) : Base(sName), m_pKeyType(pKeyType), m_pElmtType(pElmtType) {}
	Base* m_pKeyType;
	Base* m_pElmtType;
	virtual TYPE Type() const 
	{
		return TYPE_MAP;
	}
};

struct VarDecl : public Base {
	VarDecl(const char* sName, Base* pVarType, Base* pVarName) : Base(sName), m_pVarType(pVarType), m_pVarName(pVarName) {
		//std::cerr << __FUNCTION__ << "(" << GetName() << ", " << m_pVarType->GetName() << ", " << m_pVarName->GetName() << ")" << std::endl;
	}
	Base* m_pVarType;
	Base* m_pVarName;
	virtual TYPE Type() const 
	{
		return TYPE_VARDECL;
	}
};

struct List : public Base {
	List(const char* sName) : Base(sName) {}
	std::list<Base*> list_;
};

struct Enum : public Base {
	Enum(const char* sName) : Base(sName) {}
	virtual TYPE Type() const	{ return TYPE_ENUM;	}
	std::list<Base*> list_;
};

struct EnumElmt : public Base {
	EnumElmt(const char* sName, Base* pElmt, const char* sNumber) : Base(sName), m_pElmt(pElmt), m_sNumber(sNumber) {}

	virtual TYPE Type() const {	return TYPE_ENUMELMT;	}
	Base*		   m_pElmt;
	const std::string m_sNumber;
};

struct Message : public Base {
	Message(const char* sName, const char* sParentName, int nSeqNo) : Base(sName), m_sParentName(sParentName), m_nSeqNo(nSeqNo)	{}

	virtual TYPE Type() const {	return TYPE_MESSAGE;	}
	
	const std::string m_sParentName;
	const int m_nSeqNo;
	std::list<Base*> list_;
};

struct Typedef : public Base {
	Typedef(const char* sName, VarDecl* pVarDecl) : Base(sName), m_pVarDecl(pVarDecl)
	{
		//std::cerr << __FUNCTION__ << "(" << GetName() << ")" << std::endl;
	}

	virtual TYPE Type() const 
	{
		return TYPE_TYPEDEF;
	}

	VarDecl* m_pVarDecl;
};

struct StmtList : public Base 
{
	StmtList(const char* sName) : Base(sName)
	{
	}
	virtual TYPE Type() const 
	{
		return TYPE_STMTLIST;
	}
	std::list<Base*> list_;
};

struct Parser
{
	std::string m_sFileName;
	Parser(const std::string& sFileName) : m_sFileName(sFileName) {}
	virtual ~Parser() {}
	virtual bool CompileStmtList(const StmtList* pToken) = 0;
protected :
	virtual bool CompileLiteralBlock(const LiteralBlock* pToken) = 0;
	virtual bool CompileTypedef(const Typedef* pToken) = 0;
	virtual bool CompileMessage(const Message* pToken) = 0;
	virtual bool CompileEnum(const Enum* pToken) = 0;
	virtual bool Parse(const StmtList* pToken)
	{
		for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
		{
			const Token::Base* pStmt = *itr;
			switch(pStmt->Type())
			{
			case Token::TYPE_LITERALBLOCK :
				{
					CompileLiteralBlock(static_cast<const LiteralBlock*>(pStmt));
				}
				break;
			case Token::TYPE_TYPEDEF:
				{
					CompileTypedef(static_cast<const Typedef*>(pStmt));
				}
				break;
			case Token::TYPE_MESSAGE :
				{
					CompileMessage(static_cast<const Message*>(pStmt));
				}
				break;
			case Token::TYPE_ENUM :
				{
					CompileEnum(static_cast<const Enum*>(pStmt));
				}
				break;
			default :
				break;
			};
		}
		return true;
	}
};
};

#endif //__TOKEN_H__
