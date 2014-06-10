/*
 * generate_rule_python.h
 *
 *  Created on: Mar 3, 2013
 *      Author: kukuta
 */

#ifndef GENERATE_RULE_PYTHON_H_
#define GENERATE_RULE_PYTHON_H_

#include "token.h"

class GenerateRulePython : public Token::Parser
{
	struct SymbolInfo
	{
		std::string m_sInitValue;
		std::string m_sFmt;
		std::string m_sSize;
		std::string m_sKeyType;
		std::string m_sValType;
	};
	typedef std::map<std::string, SymbolInfo> SYMBOL_TABLE;
	SYMBOL_TABLE m_mapSymbolTable;

	std::map<std::string, Token::Base*> m_mapTypedef;
public :
	GenerateRulePython(const std::string& sFileName);
	virtual bool ComplieLiteralBlock(const Token::LiteralBlock* pToken);
	virtual bool CompileTypedef(const Token::Typedef* pToken);
	virtual bool CompileMessage(const Token::Message* pToken);
	virtual bool CompileEnum(const Token::Enum* pToken);
	virtual bool CompileStmtList(const Token::StmtList* pToken);

private :
	const std::string GenerateIndent(int nIndentCount)
	{
		std::string sIndent = "";
		for(int i=0; i<nIndentCount; i++)
		{
			sIndent += "\t";
		}
		return sIndent;
	}
	const std::string GenerateInitCode(const Token::Base* pToken);
	void GenerateStore(const Token::Message* pToken);
	void GenerateVariableStore(int nIndentCount, const Token::Base* typeInfo, const std::string& sPrefix, const std::string& sVarName);
	void GenerateLoad(const Token::Message* pToken);
	void GenerateVariableLoad(int nIndentCount, const Token::Base* typeInfo, const std::string& sPrefix, const std::string& sVarName);
	const std::string TranslateVariableType(const Token::Base* pToken);
};

#endif /* GENERATE_RULE_PYTHON_H_ */
