/*
 * generate_rule_python.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: kukuta
 */
#include "generate_rule_python.h"
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

GenerateRulePython::GenerateRulePython(const std::string& sFileName) : Token::Parser(sFileName)
{
	SymbolInfo boolean_type =	{"False", "b", "1", "", ""};
	SymbolInfo byte_type = 		{"'\\0'",  "c", "1", "", ""};
	SymbolInfo ubyte_type = 	{"'\\0'",  "c", "1", "", ""};
	SymbolInfo int16_type = 	{"0", 	 "h", "2", "", ""};
	SymbolInfo int32_type = 	{"0", 	 "i", "4", "", ""};
	SymbolInfo int64_type = 	{"0", 	 "q", "8", "", ""};
	SymbolInfo uint16_type = 	{"0", 	 "H", "2", "", ""};
	SymbolInfo uint32_type = 	{"0", 	 "I", "4", "", ""};
	SymbolInfo uint64_type = 	{"0", 	 "Q", "8", "", ""};
	SymbolInfo double_type = 	{"0.0",  "d", "8", "", ""};
	SymbolInfo float_type = 	{"0.0",  "f", "4", "", ""};
	m_mapSymbolTable.insert(std::make_pair("boolean", boolean_type ));
	m_mapSymbolTable.insert(std::make_pair("byte", byte_type));
	m_mapSymbolTable.insert(std::make_pair("ubyte", ubyte_type));
	m_mapSymbolTable.insert(std::make_pair("int16", int16_type));
	m_mapSymbolTable.insert(std::make_pair("int32", int32_type));
	m_mapSymbolTable.insert(std::make_pair("int64", int64_type));
	m_mapSymbolTable.insert(std::make_pair("uint16", uint16_type));
	m_mapSymbolTable.insert(std::make_pair("uint32", uint32_type));
	m_mapSymbolTable.insert(std::make_pair("uint64", uint64_type));
	m_mapSymbolTable.insert(std::make_pair("double", double_type));
	m_mapSymbolTable.insert(std::make_pair("float", float_type));
}

const std::string GenerateRulePython::GenerateInitCode(const Token::Base* pToken)
{
	switch(pToken->Type())
	{
	case Token::TYPE_STATIC_ARRAY :
		return "[" + GenerateInitCode(static_cast<const Token::StaticArrayType*>(pToken)->m_pElmtType) + "] * " + static_cast<const Token::StaticArrayType*>(pToken)->m_sElmtCount;
	case Token::TYPE_ARRAY :
	case Token::TYPE_LIST :
		return "[]";
	case Token::TYPE_MAP :
		return "{}";
	case Token::TYPE_ENUM :
		return "0";
	case Token::TYPE_PRIMITIVE :
		return m_mapSymbolTable[pToken->GetName()].m_sInitValue;
	case Token::TYPE_STRING :
		return "''";
	default :
		break;
	};

	return pToken->GetName() + "Init()";
}

bool GenerateRulePython::CompileMessage(const Token::Message* pToken)
{
	std::cout << "class " << pToken->GetName() << " ";
	if("" != pToken->m_sParentName)
	{
		std::cout << "(" << pToken->m_sParentName << ") ";
	}
	std::cout << ":" << std::endl;
	if(pToken->m_nSeqNo > 0)
	{
		std::cout << "\tMSG_ID = " << pToken->m_nSeqNo << std::endl;
	}

	std::cout << "\tdef __init__(self) :" << std::endl;
	if("" != pToken->m_sParentName)
	{
		std::cout << "\t\t" << pToken->m_sParentName << ".__init__(self)" << std::endl;
	}

	int messageElmtCount = 0;
	for(const auto& itr : pToken->list_)
	{
		if (Token::TYPE_VARDECL == itr->Type())
		{
			const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(itr);
			std::cout << "\t\tself." << pVarDecl->m_pVarName->GetName() << " = " << GenerateInitCode(pVarDecl->m_pVarType) << "\t\t\t# " << pVarDecl->m_pVarType->GetName() << std::endl;
			messageElmtCount++;
		}
	}
	if(0 == messageElmtCount)
	{
		std::cout << "\t\tpass" << std::endl;
	}

	for (const auto& itr : pToken->list_)
	{
		if (Token::TYPE_LITERALBLOCK == itr->Type())
		{
			CompileLiteralBlock(static_cast<const Token::LiteralBlock*>(itr));
		}
	}
	GenerateStore(pToken);
	GenerateLoad(pToken);
	std::cout << "def " << pToken->GetName() << "Init() :" << std::endl;
	std::cout << "\treturn " << pToken->GetName() << "()" << std::endl;
	std::cout << "def " << pToken->GetName() << "Store(o) :" << std::endl;
	std::cout << "	return o.Store()" << std::endl;
	std::cout << "def " << pToken->GetName() << "Load(buf) :" << std::endl;
	std::cout << "	val = " << pToken->GetName() << "()" << std::endl;
	std::cout << "	buf = val.Load(buf)" << std::endl;
	std::cout << "	return [val, buf]" << std::endl;
	return true;
}

void GenerateRulePython::GenerateStore(const Token::Message* pToken)
{
	int nIndentCount = 1;
	std::cout << "\t" << "def Store(self) :" << std::endl;
	std::cout << "\t" << "\t" << "data = ''" << std::endl;
	if("" != pToken->m_sParentName)
	{
		std::cout << "\t" << "\t" << "data += " << pToken->m_sParentName << ".Store(self)" <<  std::endl;
	}
	for (const auto& itr : pToken->list_)
	{
		if (Token::TYPE_VARDECL == itr->Type())
		{
			const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(itr);
			GenerateVariableStore(nIndentCount + 1, pVarDecl->m_pVarType, "self.", pVarDecl->m_pVarName->GetName());
		}
	}
	std::cout << "\t" << "\t" << "return data" << std::endl;
}

void GenerateRulePython::GenerateVariableStore(int nIndentCount, const Token::Base* typeInfo, const std::string& sPrefix, const std::string& sVarName)
{
	char szKeyName[256] = {0};
	char szValName[256] = {0};
	sprintf(szKeyName, "k%d", nIndentCount);
	sprintf(szValName, "v%d", nIndentCount);

	switch(typeInfo->Type())
	{
	case Token::TYPE_PRIMITIVE :
		std::cout << GenerateIndent(nIndentCount) << "data += struct.pack('" << m_mapSymbolTable[typeInfo->GetName()].m_sFmt << "', " << sPrefix << sVarName << ")" << std::endl;
		break;
	case Token::TYPE_STRING :
		std::cout << GenerateIndent(nIndentCount) << "data += struct.pack('" << m_mapSymbolTable["int32"].m_sFmt << "', len(" << sPrefix << sVarName << "))" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << "fmt = str(len(" + sPrefix + sVarName +")) + 's'" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << "data += struct.pack(fmt, " << sPrefix << sVarName << ")" << std::endl;
		break;
	case Token::TYPE_MAP :
		std::cout << GenerateIndent(nIndentCount) << "data += struct.pack('" << m_mapSymbolTable["int32"].m_sFmt << "', len(" << sPrefix << sVarName << "))" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << "for " << szKeyName << ", " << szValName << " in " << sPrefix << sVarName << ".iteritems() : " << std::endl;
		GenerateVariableStore(nIndentCount+1, static_cast<const Token::MapType*>(typeInfo)->m_pKeyType, "", szKeyName);
		GenerateVariableStore(nIndentCount+1, static_cast<const Token::MapType*>(typeInfo)->m_pElmtType, "", szValName);
		break;
	case Token::TYPE_LIST :
		std::cout << GenerateIndent(nIndentCount) << "data += struct.pack('" << m_mapSymbolTable["int32"].m_sFmt << "', len(" << sPrefix << sVarName << "))" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << "for " << szValName << " in " << sPrefix << sVarName << " :" << std::endl;
		GenerateVariableStore(nIndentCount+1, static_cast<const Token::ListType*>(typeInfo)->m_pElmtType, "", szValName);
		break;
	case Token::TYPE_ARRAY :
		std::cout << GenerateIndent(nIndentCount) << "data += struct.pack('" << m_mapSymbolTable["int32"].m_sFmt << "', len(" << sPrefix << sVarName << "))" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << "for " << szValName << " in " << sPrefix << sVarName << " :" << std::endl;
		GenerateVariableStore(nIndentCount+1, static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType, "", szValName);
		break;
	case Token::TYPE_STATIC_ARRAY :
		std::cout << GenerateIndent(nIndentCount) << sPrefix << sVarName << " = list(" << sPrefix << sVarName << ") + [" << GenerateInitCode(static_cast<const Token::StaticArrayType*>(typeInfo)->m_pElmtType) << "] * (" << static_cast<const Token::StaticArrayType*>(typeInfo)->m_sElmtCount << " - len(" << sPrefix << sVarName << "))" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << "for " << szKeyName << " in range(0, " << static_cast<const Token::StaticArrayType*>(typeInfo)->m_sElmtCount << ") :" << std::endl;
		GenerateVariableStore(nIndentCount+1, static_cast<const Token::StaticArrayType*>(typeInfo)->m_pElmtType, "self.", sVarName + "[" + szKeyName + "]");
		break;
	default :
		std::cout << GenerateIndent(nIndentCount) << "data += " << typeInfo->GetName() << "Store(" << sPrefix << sVarName << ")" << std::endl;
			break;
	};
}

void GenerateRulePython::GenerateLoad(const Token::Message* pToken)
{
	int nIndentCount = 1;
	std::cout << "\t" << "def Load(self, buf) :" << std::endl;

	if("" != pToken->m_sParentName)
	{
		std::cout << "\t" << "\t" << "buf = " << pToken->m_sParentName << ".Load(self, buf)" <<  std::endl;
	}

	for (const auto& itr : pToken->list_)
	{
		if (Token::TYPE_VARDECL == itr->Type())
		{
			const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(itr);
			GenerateVariableLoad(nIndentCount + 1, pVarDecl->m_pVarType, "self.", pVarDecl->m_pVarName->GetName());
		}
	}
	std::cout << "\t" << "\t" << "return buf" << std::endl;
}

void GenerateRulePython::GenerateVariableLoad(int nIndentCount, const Token::Base* typeInfo, const std::string& sPrefix, const std::string& sVarName)
{
	char szKeyName[256] = {0};
	char szValName[256] = {0};
	sprintf(szKeyName, "k%d", nIndentCount);
	sprintf(szValName, "v%d", nIndentCount);
	switch(typeInfo->Type())
	{
	case Token::TYPE_PRIMITIVE :
		std::cout << GenerateIndent(nIndentCount) << sPrefix << sVarName << " = struct.unpack('" << m_mapSymbolTable[typeInfo->GetName()].m_sFmt << "', buf[0:" << m_mapSymbolTable[typeInfo->GetName()].m_sSize << "])[0]; buf = buf[" << m_mapSymbolTable[typeInfo->GetName()].m_sSize << ":]" << std::endl;
		break;
	case Token::TYPE_STRING :
		std::cout << GenerateIndent(nIndentCount) << "str_length = struct.unpack('" << m_mapSymbolTable["int32"].m_sFmt << "', buf[0:" << m_mapSymbolTable["int32"].m_sSize << "])[0]; buf = buf[" << m_mapSymbolTable["int32"].m_sSize << ":]" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << sPrefix << sVarName << " = buf[0:str_length]; buf = buf[str_length:]" << std::endl;
		break;
	case Token::TYPE_MAP :
		std::cout << GenerateIndent(nIndentCount) << "elmt_count = struct.unpack('" << m_mapSymbolTable["int32"].m_sFmt << "', buf[0:" << m_mapSymbolTable["int32"].m_sSize << "])[0]; buf = buf[" << m_mapSymbolTable["int32"].m_sSize << ":]" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << "for i  in range(elmt_count) :" << std::endl;
		GenerateVariableLoad(nIndentCount+1, static_cast<const Token::MapType*>(typeInfo)->m_pKeyType, "", szKeyName);
		GenerateVariableLoad(nIndentCount+1, static_cast<const Token::MapType*>(typeInfo)->m_pElmtType, "", szValName);
		std::cout << GenerateIndent(nIndentCount) << "\t" << sPrefix << sVarName << "[" << szKeyName << "] = " << szValName << std::endl;
		break;
	case Token::TYPE_LIST :
		std::cout << GenerateIndent(nIndentCount) << "elmt_count = struct.unpack('" << m_mapSymbolTable["int32"].m_sFmt << "', buf[0:" << m_mapSymbolTable["int32"].m_sSize << "])[0]; buf = buf[" << m_mapSymbolTable["int32"].m_sSize << ":]" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << sVarName << " = []" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << "for i in range(elmt_count) :" << std::endl;
		GenerateVariableLoad(nIndentCount+1, static_cast<const Token::ListType*>(typeInfo)->m_pElmtType, "", szValName);
		std::cout << GenerateIndent(nIndentCount) << "\t" << sPrefix << sVarName << ".append(" << szValName << ")" << std::endl;
		break;
	case Token::TYPE_ARRAY :
		std::cout << GenerateIndent(nIndentCount) << "elmt_count = struct.unpack('" << m_mapSymbolTable["int32"].m_sFmt << "', buf[0:" << m_mapSymbolTable["int32"].m_sSize << "])[0]; buf = buf[" << m_mapSymbolTable["int32"].m_sSize << ":]" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << sVarName << " = []" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << "for i in range(elmt_count) :" << std::endl;
		GenerateVariableLoad(nIndentCount+1, static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType, "", szValName);
		std::cout << GenerateIndent(nIndentCount) << "\t" << sPrefix << sVarName << ".append(" << szValName << ")" << std::endl;
		break;
	case Token::TYPE_STATIC_ARRAY :
		std::cout << GenerateIndent(nIndentCount) << sPrefix << sVarName << " = []" << std::endl;
		std::cout << GenerateIndent(nIndentCount) << "for i in range(0, " << static_cast<const Token::StaticArrayType*>(typeInfo)->m_sElmtCount << ") :" << std::endl;
		GenerateVariableLoad(nIndentCount+1, static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType, "", szValName);
		std::cout << GenerateIndent(nIndentCount) << "\t" << sPrefix << sVarName << ".append(" << szValName << ")" << std::endl;
		break;
	default :
		std::cout << GenerateIndent(nIndentCount) << "[" << sPrefix << sVarName << ", buf] = " << typeInfo->GetName() << "Load(buf)" << std::endl;
		break;
	};
}

bool GenerateRulePython::CompileEnum(const Token::Enum* pToken)
{
	std::cout << "class " << pToken->GetName() << " :" << std::endl;
	int nMaxNumber = -1;
	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_NULL == (*itr)->Type())
		{
			continue;
		}
		const Token::EnumElmt* pElmt = static_cast<const Token::EnumElmt*>(*itr);
		std::cout << "\t" << pElmt->m_pElmt->GetName();
		if("" != pElmt->m_sNumber)
		{
			if (std::string::npos != pElmt->m_sNumber.find("0x"))
			{
				std::stringstream ss;
				ss << std::hex << pElmt->m_sNumber.c_str();
				ss >> nMaxNumber;
			}
			else {
				nMaxNumber = atoi(pElmt->m_sNumber.c_str());
			}
		}
		else
		{
			nMaxNumber++;
		}
		std::cout << " = " << nMaxNumber << std::endl;
	}
	if(0 == pToken->list_.size())
	{
		std::cout << "	pass" << std::endl;
	}

	std::cout << "def " << pToken->GetName() << "Init() : " << std::endl;
	std::cout << "	return 0" << std::endl;
	std::cout << "def " << pToken->GetName() << "Store(e) :" << std::endl;
	std::cout << "	data = ''" << std::endl;
	std::cout << "	data += struct.pack('" << m_mapSymbolTable["int32"].m_sFmt << "', e)" << std::endl;
	std::cout << "	return data" << std::endl;
	std::cout << "def " << pToken->GetName() << "Load(buf) :" << std::endl;
	std::cout << "\t" << "val = struct.unpack('" << m_mapSymbolTable["int32"].m_sFmt << "', buf[0:" << m_mapSymbolTable["int32"].m_sSize << "])[0]; buf = buf[" << m_mapSymbolTable["int32"].m_sSize << ":]" << std::endl;
	std::cout << "	return [val, buf]" << std::endl;
	return true;
}

bool GenerateRulePython::CompileStmtList(const Token::StmtList* pToken)
{
	std::cout << "import struct" << std::endl;

	if(false == Parse(pToken))
	{
		return false;
	}
	return true;
}

bool GenerateRulePython::CompileLiteralBlock(const Token::LiteralBlock* pToken)
{
	if(".py" == pToken->m_sLanguage)
	{
		std::cout << pToken->m_sContext << std::endl;
	}
	return true;
}

bool GenerateRulePython::CompileTypedef(const Token::Typedef* pToken)
{
	std::cout << "def " << pToken->m_pVarDecl->m_pVarName->GetName() << "Init() :" << std::endl;
	std::cout << "\treturn " << GenerateInitCode(pToken->m_pVarDecl->m_pVarType) << std::endl;

	std::cout << "def " << pToken->m_pVarDecl->m_pVarName->GetName() << "Store(val) :" << std::endl;
	std::cout << "	data = ''" << std::endl;
	GenerateVariableStore(1, pToken->m_pVarDecl->m_pVarType, "", "val");
	std::cout << "	return data" << std::endl;
	std::cout << "def " << pToken->m_pVarDecl->m_pVarName->GetName() << "Load(buf) :" << std::endl;
	GenerateVariableLoad(1, pToken->m_pVarDecl->m_pVarType, "", "val");
	std::cout << "	return [val, buf]" << std::endl;
	return true;
}

