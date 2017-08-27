/*
 * generate_rule_cpp.cpp
 *
 *  Created on: Mar 8, 2013
 *      Author: kukuta
 */

#include "generate_rule_cpp.h"

const std::string GenerateRuleCpp::ReplaceSpecialChar(const std::string& ori)
{
    std::string ret = ori;
    size_t pos = ret.find("[");
    if(pos < ret.length())
    {
        ret[pos] = '_';
    }
    pos = ret.find("]");
    if(pos < ret.length())
    {
        ret[pos] = '_';
    }

    return ret;
}

GenerateRuleCpp::GenerateRuleCpp(const std::string& sFileName) : Parser(sFileName)
{
	SymbolInfo booleanInfo 	= { Token::TYPE_PRIMITIVE, 	"bool", "false" };
	SymbolInfo charInfo 	= { Token::TYPE_PRIMITIVE, 	"char", "'\\0'" };
	SymbolInfo ucharInfo 	= { Token::TYPE_PRIMITIVE, 	"unsigned char", "'\\0'" };
	SymbolInfo i16Info 		= { Token::TYPE_PRIMITIVE, 	"int16_t", "0" };
	SymbolInfo i32Info 		= { Token::TYPE_PRIMITIVE, 	"int32_t", "0" };
	SymbolInfo i64Info 		= { Token::TYPE_PRIMITIVE, 	"int64_t", "0" };
	SymbolInfo ui16Info 	= { Token::TYPE_PRIMITIVE, 	"uint16_t", "0" };
	SymbolInfo ui32Info		= { Token::TYPE_PRIMITIVE, 	"uint32_t", "0" };
	SymbolInfo ui64Info		= { Token::TYPE_PRIMITIVE, 	"uint64_t", "0" };
	SymbolInfo doubleInfo	= { Token::TYPE_PRIMITIVE, 	"double", "0.0" };
	SymbolInfo floatInfo	= { Token::TYPE_PRIMITIVE, 	"float", "0.0" };
	m_mapSymbolTable.insert(std::make_pair("boolean", 	booleanInfo));
	m_mapSymbolTable.insert(std::make_pair("byte", 		charInfo));
	m_mapSymbolTable.insert(std::make_pair("ubyte",		ucharInfo));
	m_mapSymbolTable.insert(std::make_pair("int16", 	i16Info));
	m_mapSymbolTable.insert(std::make_pair("int32", 	i32Info));
	m_mapSymbolTable.insert(std::make_pair("int64", 	i64Info));
	m_mapSymbolTable.insert(std::make_pair("uint16",	ui16Info));
	m_mapSymbolTable.insert(std::make_pair("uint32",	ui32Info));
	m_mapSymbolTable.insert(std::make_pair("uint64",	ui64Info));
	m_mapSymbolTable.insert(std::make_pair("double", 	doubleInfo));
	m_mapSymbolTable.insert(std::make_pair("float", 	floatInfo));
}

const std::string GenerateRuleCpp::TranslateVariableType(const Token::Base* pToken)
{
	switch(pToken->Type())
	{
		case Token::TYPE_PRIMITIVE :
			return m_mapSymbolTable[pToken->GetName()].m_sTypeName;
		case Token::TYPE_STRING :
			return "std::string";
		case Token::TYPE_MAP :
		{
			const Token::MapType* pMapType = static_cast<const Token::MapType*>(pToken);
			return "std::map<" + TranslateVariableType(pMapType->m_pKeyType) + ", " + TranslateVariableType(pMapType->m_pElmtType) + " >";
		}
		case Token::TYPE_STATIC_ARRAY :
		{
			const Token::StaticArrayType* pArrType = static_cast<const Token::StaticArrayType*>(pToken);
			return TranslateVariableType(pArrType->m_pElmtType);
		}
		case Token::TYPE_ARRAY :
		{
			const Token::ArrayType* pArrType = static_cast<const Token::ArrayType*>(pToken);
			return "std::vector<" + TranslateVariableType(pArrType->m_pElmtType) + " >";
		}
		case Token::TYPE_LIST :
		{
			const Token::ListType* pListType = static_cast<const Token::ListType*>(pToken);
			return "std::list<" + TranslateVariableType(pListType->m_pElmtType) + " >";
		}
		default :
		{
			if(m_mapSymbolTable.find(pToken->GetName()) == m_mapSymbolTable.end())
			{
				m_mapSymbolTable[pToken->GetName()].m_sTypeName = pToken->GetName();
				m_mapSymbolTable[pToken->GetName()].m_eType = Token::TYPE_USERDEFINE;
				m_mapSymbolTable[pToken->GetName()].m_sInitValue = "";
			}
			return m_mapSymbolTable[pToken->GetName()].m_sTypeName;
		}
	};
	return std::string();
}

void GenerateRuleCpp::GenerateVariableInit(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent)
{
	//bool Store(char** _buf_) const;
	switch(typeInfo->Type()) 
	{
	case Token::TYPE_PRIMITIVE :
		std::cout << "\t\t" << sVarName << " = " << m_mapSymbolTable[typeInfo->GetName()].m_sInitValue << ";" << std::endl;
	default :
		break;
	}
}

void GenerateRuleCpp::GenerateVariableStore(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent)
{
	//bool Store(char** _buf_) const;
	switch(typeInfo->Type()) 
	{
	case Token::TYPE_USERDEFINE :
		std::cout << sIndent << "if(false == " << typeInfo->GetName() << "_Serializer::Store(_buf_, " << sVarName << ")) { return false; }" << std::endl;
		break;
	case Token::TYPE_PRIMITIVE :
		//std::cout << sIndent << "(*(" << m_mapSymbolTable[typeInfo->GetName()].m_sTypeName << "*)(*_buf_)) = " << sVarName << "; (*_buf_) += sizeof(" << m_mapSymbolTable[typeInfo->GetName()].m_sTypeName << ");" << std::endl;
		std::cout << sIndent << "std::memcpy(*_buf_, &" << sVarName << ", sizeof(" << m_mapSymbolTable[typeInfo->GetName()].m_sTypeName << ")); (*_buf_) += sizeof(" << m_mapSymbolTable[typeInfo->GetName()].m_sTypeName << ");" << std::endl;
		break;
	case Token::TYPE_STRING :
		std::cout << sIndent << "size_t " << ReplaceSpecialChar(sVarName) << "_size = " << sVarName << ".length();" << std::endl; 
		std::cout << sIndent << "std::memcpy(*_buf_, &" << ReplaceSpecialChar(sVarName) << "_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);" << std::endl;
		std::cout << sIndent << "std::memcpy(*_buf_, " << sVarName << ".c_str(), " << sVarName << ".length()); (*_buf_) += " << sVarName << ".length();" << std::endl;
		break;
	case Token::TYPE_MAP :
		std::cout << sIndent << "size_t " << sVarName << "_size = " << sVarName << ".size();" << std::endl; 
		std::cout << sIndent << "std::memcpy(*_buf_, &" << sVarName << "_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);" << std::endl;
		std::cout << sIndent << "for(std::map<" << 
									TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType) << ", " <<
									TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType) << 
								" >::const_iterator " << sVarName << "_itr = " << sVarName << ".begin(); " << sVarName << "_itr != " << sVarName << ".end(); " << sVarName << "_itr++) {" << std::endl;
		std::cout << sIndent << "\tconst " << TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType) << "& " << sVarName << "_key = " << sVarName << "_itr->first;" << std::endl;
		std::cout << sIndent << "\tconst " << TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType) << "& " << sVarName << "_elmt = " << sVarName << "_itr->second;" << std::endl;
		GenerateVariableStore(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType, sVarName+"_key", sIndent + "\t");
		GenerateVariableStore(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType, sVarName+"_elmt", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_LIST :
		std::cout << sIndent << "size_t " << sVarName << "_size = " << sVarName << ".size();" << std::endl; 
		std::cout << sIndent << "std::memcpy(*_buf_, &" << sVarName << "_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);" << std::endl;
		std::cout << sIndent << "for(std::list<" 
							 	<< TranslateVariableType(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType) 
							 << " >::const_iterator " << sVarName << "_itr = " << sVarName << ".begin(); " << sVarName << "_itr != " << sVarName << ".end(); " << sVarName << "_itr++)	{" << std::endl;
		std::cout << sIndent << "\tconst " << TranslateVariableType(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType) << "& " << sVarName << "_elmt = *" << sVarName << "_itr;" << std::endl;
		GenerateVariableStore(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType, sVarName +"_elmt", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_ARRAY :
		std::cout << sIndent << "size_t " << sVarName << "_size = " << sVarName << ".size();" << std::endl; 
		std::cout << sIndent << "std::memcpy(*_buf_, &" << sVarName << "_size, sizeof(int32_t)); (*_buf_) += sizeof(int32_t);" << std::endl;
		std::cout << sIndent << "for(std::vector<" 
							 	<< TranslateVariableType(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType)
							 << " >::const_iterator " << sVarName << "_itr = " << sVarName << ".begin(); " << sVarName << "_itr != " << sVarName << ".end(); " << sVarName << "_itr++)	{" << std::endl;
		std::cout << sIndent << "\tconst " << TranslateVariableType(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType) << "& " << sVarName << "_elmt = *" << sVarName << "_itr;" << std::endl;
		GenerateVariableStore(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType, sVarName +"_elmt", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_STATIC_ARRAY :
		if(true == static_cast<const Token::StaticArrayType*>(typeInfo)->m_bPrimitive)
		{
			const Token::StaticArrayType* type = static_cast<const Token::StaticArrayType*>(typeInfo);
			std::cout << sIndent << "std::memcpy(*_buf_, " << sVarName << ", sizeof(" << TranslateVariableType(type->m_pElmtType) << ") * " << type->m_sElmtCount << "); ";
			std::cout << "(*_buf_) += " << "sizeof(" << TranslateVariableType(type->m_pElmtType) << ") * " << type->m_sElmtCount << ";" << std::endl;
		}
		else
		{
			std::cout << sIndent << "for(int32_t i=0; i<" << static_cast<const Token::StaticArrayType*>(typeInfo)->m_sElmtCount << "; i++) {" << std::endl;
			GenerateVariableStore(static_cast<const Token::StaticArrayType*>(typeInfo)->m_pElmtType, sVarName + "[i]", sIndent + "\t");
			std::cout << sIndent << "}" << std::endl;
		}
		break;
	default :
		std::cerr << "undefined type(" << typeInfo->GetName() << ") store error" << std::endl;
		break;
	}
}

void GenerateRuleCpp::GenerateVariableLoad(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent)
{
	//bool Load(const char** _buf_, size_t& nSize);
	switch(typeInfo->Type()) 
	{
	case Token::TYPE_USERDEFINE :
		std::cout << sIndent << "if(false == " << typeInfo->GetName() << "_Serializer::Load(" << sVarName << ", _buf_, nSize)) { return false; }" << std::endl;
		break;
	case Token::TYPE_PRIMITIVE :
		std::cout << sIndent << "if(sizeof("  << TranslateVariableType(typeInfo) << ") > nSize) { return false; }";
		std::cout << "\tstd::memcpy(&" << sVarName << ", *_buf_, sizeof(" << TranslateVariableType(typeInfo) << "));";
		std::cout << "\t(*_buf_) += sizeof(" << TranslateVariableType(typeInfo) << "); nSize -= sizeof(" << TranslateVariableType(typeInfo) << ");" << std::endl;;
		break;
	case Token::TYPE_STRING :
		std::cout << sIndent << "if(sizeof(int32_t) > nSize) { return false; }" << std::endl;
		std::cout << sIndent << "uint32_t " << sVarName << "_length = 0; std::memcpy(&" << sVarName << "_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);" << std::endl;;
		std::cout << sIndent << "if(nSize < " << sVarName << "_length) { return false; }" << std::endl;
		std::cout << sIndent << "" << sVarName << ".assign((char*)*_buf_, " << sVarName << "_length); (*_buf_) += " << sVarName << "_length; nSize -= " << sVarName << "_length;" << std::endl;
		break;
	case Token::TYPE_MAP :
		std::cout << sIndent << "if(sizeof(int32_t) > nSize) { return false; }" << std::endl;
		std::cout << sIndent << "uint32_t " << sVarName << "_length = 0; std::memcpy(&" << sVarName << "_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);" << std::endl;;
		std::cout << sIndent << "for(uint32_t i=0; i<" << sVarName << "_length; i++) {" << std::endl;
		std::cout << sIndent <<	"\t" << TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType) << " " << sVarName << "_key;" << std::endl;
		GenerateVariableLoad(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType, sVarName + "_key", sIndent+"\t");
		std::cout << sIndent <<	"\t" << TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType) << " " << sVarName << "_val;" << std::endl;
		GenerateVariableLoad(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType, sVarName + "_val", sIndent+"\t");
		std::cout << sIndent <<	"\t" << sVarName << ".insert(std::make_pair(" << sVarName << "_key, " << sVarName << "_val));" << std::endl;
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_LIST :
		std::cout << sIndent << "if(sizeof(int32_t) > nSize) { return false; }" << std::endl;
		std::cout << sIndent << "uint32_t " << sVarName << "_length = 0; std::memcpy(&" << sVarName << "_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);" << std::endl;;
		std::cout << sIndent << "for(uint32_t i=0; i<" << sVarName << "_length; i++) {" << std::endl;
		std::cout << sIndent <<	"\t" << TranslateVariableType(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType) << " " << sVarName << "_val;" << std::endl;
		GenerateVariableLoad(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType, sVarName+"_val", sIndent+"\t");
		std::cout << sIndent << "\t" << sVarName << ".push_back(" << sVarName << "_val);" << std::endl;
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_ARRAY :
		std::cout << sIndent << "if(sizeof(int32_t) > nSize) { return false; }" << std::endl;
		std::cout << sIndent << "uint32_t " << sVarName << "_length = 0; std::memcpy(&" << sVarName << "_length, *_buf_, sizeof(uint32_t)); (*_buf_) += sizeof(uint32_t); nSize -= sizeof(uint32_t);" << std::endl;;
		std::cout << sIndent << "for(uint32_t i=0; i<" << sVarName << "_length; i++) {" << std::endl;
		std::cout << sIndent <<	"\t" << TranslateVariableType(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType) << " " << sVarName << "_val;" << std::endl;
		GenerateVariableLoad(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType, sVarName+"_val", sIndent+"\t");
		std::cout << sIndent << "\t" << sVarName << ".push_back(" << sVarName << "_val);" << std::endl;
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_STATIC_ARRAY :
		if(true == static_cast<const Token::StaticArrayType*>(typeInfo)->m_bPrimitive)
		{
			const Token::StaticArrayType* type = static_cast<const Token::StaticArrayType*>(typeInfo);
			std::cout << sIndent << "std::memcpy(" << sVarName << ", *_buf_, sizeof(" << TranslateVariableType(type) << ") * " << type->m_sElmtCount << "); ";
			std::cout << "(*_buf_) += sizeof(" << TranslateVariableType(typeInfo) << ") * " << type->m_sElmtCount << "; nSize -= sizeof(" << TranslateVariableType(typeInfo) << ") * " << type->m_sElmtCount << ";" << std::endl;;
		}
		else
		{
			std::cout << sIndent << "for(int32_t i=0; i<" << static_cast<const Token::StaticArrayType*>(typeInfo)->m_sElmtCount << "; i++) {" << std::endl;
			std::cout << sIndent <<	"\t" << TranslateVariableType(static_cast<const Token::StaticArrayType*>(typeInfo)->m_pElmtType) << " " << sVarName << "_val;" << std::endl;
			GenerateVariableLoad(static_cast<const Token::StaticArrayType*>(typeInfo)->m_pElmtType, sVarName + "_val", sIndent + "\t");
			std::cout << sIndent << "\t" << sVarName << "[i] = " << sVarName << "_val;" << std::endl;
			std::cout << sIndent << "}" << std::endl;
		}
		break;
	default :
		std::cerr << "undefined type(" << typeInfo->GetName() << ") load error" << std::endl;
		break;
	}
}

void GenerateRuleCpp::GenerateVariableSize(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent)
{
	switch(typeInfo->Type()) 
	{
	case Token::TYPE_USERDEFINE :
		std::cout << sIndent << "nSize += " << typeInfo->GetName() << "_Serializer::Size(" << sVarName << ");" << std::endl;
		break;
	case Token::TYPE_PRIMITIVE :
		std::cout << sIndent << "nSize += sizeof(" << TranslateVariableType(typeInfo) << ");" << std::endl;
		break;
	case Token::TYPE_STRING :
		std::cout << sIndent << "nSize += sizeof(uint32_t); nSize += " << sVarName << ".length();" << std::endl;
		break;
	case Token::TYPE_MAP :
		std::cout << sIndent << "nSize += sizeof(uint32_t);" << std::endl;
		std::cout << sIndent << "for(std::map<" << 
									TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType) << ", " <<
									TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType) << 
								" >::const_iterator " << sVarName << "_itr = " << sVarName << ".begin(); " << sVarName << "_itr != " << sVarName << ".end(); " << sVarName << "_itr++) {" << std::endl;
		{
			auto itr = m_mapSymbolTable.find(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType->GetName());
			//if(Token::TYPE_PRIMITIVE != itr->second.m_eType)
			if ((m_mapSymbolTable.end() != itr && Token::TYPE_PRIMITIVE != itr->second.m_eType) || m_mapSymbolTable.end() == itr)
			{
				std::cout << sIndent << "\tconst " << TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType) << "& " << sVarName << "_key = " << sVarName << "_itr->first;" << std::endl;
			}
		}
		std::cout << sIndent << "\tconst " << TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType) << "& " << sVarName << "_elmt = " << sVarName << "_itr->second;" << std::endl;
		GenerateVariableSize(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType, sVarName+"_key", sIndent + "\t");
		GenerateVariableSize(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType, sVarName+"_elmt", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_LIST :
		std::cout << sIndent << "nSize += sizeof(int32_t);" << std::endl;
		std::cout << sIndent << "for(std::list<" 
							 	<< TranslateVariableType(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType) 
							 << " >::const_iterator " << sVarName << "_itr = " << sVarName << ".begin(); " << sVarName << "_itr != " << sVarName << ".end(); " << sVarName << "_itr++)	{" << std::endl;
		{
			auto itr = m_mapSymbolTable.find(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType->GetName());
			//if(Token::TYPE_PRIMITIVE != itr->second.m_eType)
			if((m_mapSymbolTable.end() != itr && Token::TYPE_PRIMITIVE != itr->second.m_eType) || m_mapSymbolTable.end() == itr)
			{
				std::cout << sIndent << "\tconst " << TranslateVariableType(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType) << "& " << sVarName << "_elmt = *" << sVarName << "_itr;" << std::endl;
			}
		}
		GenerateVariableSize(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType, sVarName +"_elmt", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_ARRAY :
		std::cout << sIndent << "nSize += sizeof(int32_t);" << std::endl;
		std::cout << sIndent << "for(std::vector<" 
							 	<< TranslateVariableType(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType) 
							 << " >::const_iterator " << sVarName << "_itr = " << sVarName << ".begin(); " << sVarName << "_itr != " << sVarName << ".end(); " << sVarName << "_itr++)	{" << std::endl;
		{
			auto itr = m_mapSymbolTable.find(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType->GetName());
			//if(Token::TYPE_PRIMITIVE != itr->second.m_eType)
			if ((m_mapSymbolTable.end() != itr && Token::TYPE_PRIMITIVE != itr->second.m_eType) || m_mapSymbolTable.end() == itr)
			{
				std::cout << sIndent << "\tconst " << TranslateVariableType(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType) << "& " << sVarName << "_elmt = *" << sVarName << "_itr;" << std::endl;
			}
		}
		GenerateVariableSize(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType, sVarName +"_elmt", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_STATIC_ARRAY :
		if(true == static_cast<const Token::StaticArrayType*>(typeInfo)->m_bPrimitive)
		{
			const Token::StaticArrayType* type = static_cast<const Token::StaticArrayType*>(typeInfo);
			std::cout << sIndent << "nSize += sizeof(" << TranslateVariableType(type->m_pElmtType) << ") * " << type->m_sElmtCount << ";" << std::endl;
		}
		else
		{
			std::cout << sIndent << "for(int32_t i=0; i<" << static_cast<const Token::StaticArrayType*>(typeInfo)->m_sElmtCount << "; i++) {" << std::endl;
			GenerateVariableSize(static_cast<const Token::StaticArrayType*>(typeInfo)->m_pElmtType, sVarName + "[i]", sIndent + "\t");
			std::cout << sIndent << "}" << std::endl;
		}
		break;
	default :
		std::cerr << "undefined type(" << typeInfo->GetName() << ") size error" << std::endl;
		break;
	};
}

bool GenerateRuleCpp::CompileMessage(const Token::Message* pToken)
{
	std::cout << "struct " << pToken->GetName();
	if("" != pToken->m_sParentName) {	std::cout << " : public " + pToken->m_sParentName;	}
	std::cout << " {" << std::endl;
	if(pToken->m_nSeqNo > 0)
	{
		std::cout << "	enum { MSG_ID = " << pToken->m_nSeqNo << " }; " << std::endl;
	}

	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_VARDECL == (*itr)->Type())
		{
			const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(*itr);
			std::cout << "\t" << TranslateVariableType(pVarDecl->m_pVarType) << "\t" << pVarDecl->m_pVarName->GetName();
			if(Token::TYPE_STATIC_ARRAY == pVarDecl->m_pVarType->Type())
			{
				std::cout << "[" << static_cast<const Token::StaticArrayType*>(pVarDecl->m_pVarType)->m_sElmtCount << "]";
			}
			std::cout << ";" << std::endl;
		}
	}

	// Constructor
	std::cout << "\t" << pToken->GetName() << "()";
	if("" != pToken->m_sParentName)
	{
		std::cout << " : " << pToken->m_sParentName << "()";
	}
	std::cout << "\t{" << std::endl;
	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_NULL == (*itr)->Type())
		{
			continue;
		}
		const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(*itr);
		GenerateVariableInit(pVarDecl->m_pVarType, pVarDecl->m_pVarName->GetName());
	}
	std::cout << "\t}" << std::endl;

	if("" != pToken->m_sParentName) 
	{
		std::cout << "\t" << pToken->GetName() << "(const " << pToken->m_sParentName << "& base)";
		std::cout << " : " << pToken->m_sParentName << "(base)";
		std::cout << "\t{" << std::endl;
		for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
		{
			if(Token::TYPE_NULL == (*itr)->Type())
			{
				continue;
			}
			const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(*itr);
			GenerateVariableInit(pVarDecl->m_pVarType, pVarDecl->m_pVarName->GetName());
		}
		std::cout << "\t}" << std::endl;
	}
	// Size
	std::cout << "	size_t Size() const {" << std::endl;
	std::cout << "		size_t nSize = 0;" << std::endl;
	if("" != pToken->m_sParentName)
	{
		std::cout << "		nSize += " << pToken->m_sParentName << "::Size();" << std::endl;
	}
	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_NULL == (*itr)->Type())
		{
			continue;
		}

		const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(*itr);
		GenerateVariableSize(pVarDecl->m_pVarType, pVarDecl->m_pVarName->GetName());
	}
	std::cout << "		return nSize;" << std::endl;
	std::cout << "	}" << std::endl;

	// Store
	std::cout << "	bool Store(std::vector<char>& _buf_) const {" << std::endl;
	std::cout << "		size_t nSize = Size();" << std::endl;
	std::cout << " 		if(0 == nSize) { return true; }" << std::endl;
	std::cout << "		if(nSize > _buf_.size()) { " << std::endl;
	std::cout << "			_buf_.resize(nSize);" << std::endl;
	std::cout << "		}" << std::endl;
	std::cout << "		char* pBuf = &(_buf_[0]);" << std::endl;
	std::cout << "		if(false == Store(&pBuf)) return false;" << std::endl;
	std::cout << "		return true;" << std::endl;
	std::cout << "	}" << std::endl;
	std::cout << "	bool Store(char** _buf_) const {" << std::endl;
	if("" != pToken->m_sParentName)
	{
		std::cout << "		if(false == " << pToken->m_sParentName << "::Store(_buf_)) return false;" << std::endl;
	}
	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_NULL == (*itr)->Type())
		{
			continue;
		}
		const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(*itr);
		GenerateVariableStore(pVarDecl->m_pVarType, pVarDecl->m_pVarName->GetName());
	}
	std::cout << "		return true;" << std::endl;
	std::cout << "	}" << std::endl;

	// Load
	std::cout << "	bool Load(const std::vector<char>& _buf_) {" << std::endl;
	std::cout << "		size_t nSize = _buf_.size();" << std::endl;
	std::cout << " 		if(0 == nSize) { return true; }" << std::endl;
	std::cout << "		const char* pBuf = &(_buf_[0]);" << std::endl;
	std::cout << "		if(false == Load(&pBuf, nSize)) return false;" << std::endl;
	std::cout << "		return true;" << std::endl;
	std::cout << "	}" << std::endl;
	std::cout << "	bool Load(const char** _buf_, size_t& nSize) {" << std::endl;
	if("" != pToken->m_sParentName)
	{
		std::cout << "		if(false == " << pToken->m_sParentName  << "::Load(_buf_, nSize)) return false;" << std::endl;
	}

	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_NULL == (*itr)->Type())
		{
			continue;
		}
		const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(*itr);
		GenerateVariableLoad(pVarDecl->m_pVarType, pVarDecl->m_pVarName->GetName());
	}
	std::cout << "		return true;" << std::endl;
	std::cout << "	}" << std::endl;
	std::cout << "}; //" << pToken->GetName() << std::endl;

	std::cout << "struct " << pToken->GetName() << "_Serializer {" << std::endl;
	std::cout << "\tstatic bool Store(char** _buf_, const " << pToken->GetName() << "& obj) { return obj.Store(_buf_); }" << std::endl;
	std::cout << "\tstatic bool Load(" << pToken->GetName() << "& obj, const char** _buf_, size_t& nSize) { return obj.Load(_buf_, nSize); }" << std::endl;
	std::cout << "\tstatic size_t Size(const " << pToken->GetName() << "& obj) { return obj.Size(); }" << std::endl;
	std::cout << "};" << std::endl;
	return true;
}

bool GenerateRuleCpp::CompileEnum(const Token::Enum* pToken)
{
	std::string default_value = "";
	std::cout << "struct " << pToken->GetName() << " {" << std::endl;
	std::cout << "\tenum TYPE {" << std::endl;
	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_NULL == (*itr)->Type())
		{
			continue;
		}
		const Token::EnumElmt* pElmt = static_cast<const Token::EnumElmt*>(*itr);
		std::cout << "\t\t\t" << pElmt->m_pElmt->GetName();
		if("" == default_value)
		{
			default_value = pElmt->m_pElmt->GetName();
		}
		if("" != pElmt->m_sNumber)
		{
			std::cout << " = " << pElmt->m_sNumber;
		}
		std::cout << "," << std::endl;
	}
	std::cout << "\t};" << std::endl;
	std::cout << "\tTYPE type;" << std::endl;
	std::cout << "\t" << pToken->GetName() << "() : type(" << default_value << ") {}" << std::endl;
	std::cout << "\t" << pToken->GetName() << "(int obj) : type((TYPE)obj) {} " << std::endl;
	std::cout << "\t" << pToken->GetName() << "(TYPE obj) : type(obj) {}" << std::endl;
	std::cout << "\toperator TYPE() { return type; }" << std::endl;
	std::cout << "\toperator int() { return (int)type; }" << std::endl;
	std::cout << "\tTYPE operator = (const TYPE& obj) { type = obj; return type; }" << std::endl;
	std::cout << "}; // " << pToken->GetName() << std::endl;

	std::cout << "struct " << pToken->GetName() << "_Serializer {" << std::endl;
	std::cout << "\tstatic bool Store(char** _buf_, const " << pToken->GetName() << "& obj) { " << std:: endl;
	std::cout << "\t\t" << "(*(" << pToken->GetName() << "::TYPE*)(*_buf_)) = obj.type;	(*_buf_) += sizeof(" << pToken->GetName() << "::TYPE);" << std::endl;
	std::cout << "\t\treturn true;" << std::endl;
	std::cout << "\t}" << std::endl;
	std::cout << "\tstatic bool Load(" << pToken->GetName() << "& obj, const char** _buf_, size_t& nSize) { " << std::endl;
	std::cout << "\t\tif(sizeof("  << pToken->GetName() << "::TYPE) > nSize) { return false; }";
	std::cout << "\t\tstd::memcpy(&obj.type, *_buf_, sizeof(" << pToken->GetName() << "::TYPE));";
	std::cout << "\t\t(*_buf_) += sizeof(" << pToken->GetName() << "::TYPE); nSize -= sizeof(" << pToken->GetName() << "::TYPE);" << std::endl;;
	std::cout << "\t\treturn true;" << std::endl;
	std::cout << "\t}" << std::endl;
	std::cout << "\tstatic size_t Size(const " << pToken->GetName() << "& obj) { return sizeof(" << pToken->GetName() << "::TYPE); }" << std::endl;
	std::cout << "};" << std::endl;
	return true;
}

bool GenerateRuleCpp::CompileStmtList(const Token::StmtList* pToken)
{
	std::cout << "#ifndef __" << m_sFileName << "_H__" <<std::endl;
	std::cout << "#define __" << m_sFileName << "_H__" <<std::endl;
	std::cout << "#include <string>" << std::endl;
	std::cout << "#include <list>" << std::endl;
	std::cout << "#include <vector>" << std::endl;
	std::cout << "#include <map>" << std::endl;
	std::cout << "#include <cstring>" << std::endl;
	std::cout << "#include <stdint.h>" << std::endl;

	//GenerateStubCode();

	if(false == Parse(pToken))
	{
		return false;
	}

	std::cout << "#endif // __" << m_sFileName << "_H__" << std::endl;
	return true;
}

bool GenerateRuleCpp::CompileLiteralBlock(const Token::LiteralBlock* pToken)
{
	if(".cpp" == pToken->m_sLanguage)
	{
		std::cout << pToken->m_sContext << std::endl;
	}
	return true;
}

bool GenerateRuleCpp::CompileTypedef(const Token::Typedef* pToken)
{
	std::cout << "typedef " << TranslateVariableType(pToken->m_pVarDecl->m_pVarType) << " " << pToken->m_pVarDecl->m_pVarName->GetName() << ";" << std::endl;
	std::cout << "struct " << pToken->m_pVarDecl->m_pVarName->GetName() << "_Serializer {" << std::endl;
	std::cout << "\tstatic bool Store(char** _buf_, const " << pToken->m_pVarDecl->m_pVarName->GetName() << "& obj) { " << std:: endl;
	GenerateVariableStore(pToken->m_pVarDecl->m_pVarType, "obj", "\t\t");
	std::cout << "\treturn true;" << std::endl; 
	std::cout << "\t}" << std::endl;
	std::cout << "\tstatic bool Load(" << pToken->m_pVarDecl->m_pVarName->GetName() << "& obj, const char** _buf_, size_t& nSize) { " << std::endl;
	GenerateVariableLoad(pToken->m_pVarDecl->m_pVarType, "obj", "\t\t");
	std::cout << "\t\treturn true;" << std::endl; 
	std::cout << "\t}" << std::endl;
	std::cout << "\tstatic size_t Size(const " << pToken->m_pVarDecl->m_pVarName->GetName() << "& obj) { " << std::endl;
	std::cout << "\t\tint32_t nSize = 0;" << std::endl;
	GenerateVariableSize(pToken->m_pVarDecl->m_pVarType, "obj", "\t\t");
	std::cout << "\t\treturn nSize;" << std::endl;
	std::cout << "\t}" << std::endl;
	std::cout << "};" << std::endl;
	return true;
}

