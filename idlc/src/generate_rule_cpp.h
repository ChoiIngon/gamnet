#ifndef __GENERATE_RULE_CPP_H__
#define __GENERATE_RULE_CPP_H__

#include "token.h"

class GenerateRuleCpp : public Token::Parser
{
	struct SymbolInfo
	{
		Token::TYPE m_eType;
		std::string m_sTypeName;
		std::string m_sInitValue;
	};
	typedef std::map<std::string, SymbolInfo> SYMBOL_TABLE;
	SYMBOL_TABLE m_mapSymbolTable;
public :
	GenerateRuleCpp(const std::string& sFileName);

	virtual bool CompileLiteralBlock(const Token::LiteralBlock* pToken);
	virtual bool CompileTypedef(const Token::Typedef* pToken);
	virtual bool CompileMessage(const Token::Message* pToken);
	virtual bool CompileEnum(const Token::Enum* pToken);
	virtual bool CompileStmtList(const Token::StmtList* pToken);
private :
	const std::string TranslateVariableType(const Token::Base* pToken);

	void GenerateVariableStore(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent = "\t\t");
	void GenerateVariableLoad(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent = "\t\t");
	void GenerateVariableSize(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent = "\t\t");
	void GenerateVariableInit(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent = "\t\t");
	const std::string ReplaceSpecialChar(const std::string& str);
};
#endif//__GENERATE_RULE_H__

