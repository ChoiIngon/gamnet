#ifndef __GENERATE_RULE_CSHARP_H__
#define __GENERATE_RULE_CSHARP_H__

#include "token.h"

class GenerateRuleCSharp : public Token::Parser
{
	struct SymbolInfo
	{
		SymbolInfo() {}
		SymbolInfo(const char* typeName, const char* initValue)
			: m_sTypeName(typeName), m_sInitValue(initValue)
		{
		}
		std::string m_sTypeName;
		std::string m_sInitValue;
	};
	typedef std::map<std::string, SymbolInfo> SYMBOL_TABLE;
	SYMBOL_TABLE m_mapSymbolTable;
	std::list<Token::Typedef*> m_lstTypedef;
public :
	GenerateRuleCSharp(const std::string& sFileName);

	virtual bool ComplieLiteralBlock(const Token::LiteralBlock* pToken);
	virtual bool CompileTypedef(const Token::Typedef* pToken);
	virtual bool CompileMessage(const Token::Message* pToken);
	virtual bool CompileEnum(const Token::Enum* pToken);
	virtual bool CompileStmtList(const Token::StmtList* pToken);
	virtual bool Parse(const Token::StmtList* pToken)
	{
		for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
		{
			const Token::Base* pStmt = *itr;
			if(Token::TYPE_TYPEDEF == pStmt->Type())
			{
				CompileTypedef(static_cast<const Token::Typedef*>(pStmt));
			}
		}
		for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
		{
			const Token::Base* pStmt = *itr;
			switch(pStmt->Type())
			{
			case Token::TYPE_LITERALBLOCK :
				{
					ComplieLiteralBlock(static_cast<const Token::LiteralBlock*>(pStmt));
				}
				break;
			case Token::TYPE_MESSAGE :
				{
					CompileMessage(static_cast<const Token::Message*>(pStmt));
				}
				break;
			case Token::TYPE_ENUM :
				{
					CompileEnum(static_cast<const Token::Enum*>(pStmt));
				}
				break;
			default :
				break;
			};
		}
		return true;
	}
private :
	const std::string TranslateVariableType(const Token::Base* pToken);
	void GenerateStubCode();
	void GeneratePrimitiveStubCode(const std::string& sType);
//	void GenerateStore(const Token::Message* pToken, const std::string& sVarName);
	void GenerateVariableStore(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent = "\t\t");
	void GenerateVariableLoad(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent = "\t\t");
	void GenerateVariableSize(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent = "\t\t");
	const std::string GenerateVariableInit(const Token::Base* typeInfo);
};
#endif//__GENERATE_RULE_H__

