/*
 * generate_rule_cpp.cpp
 *
 *  Created on: Mar 8, 2013
 *      Author: kukuta
 */

#include "generate_rule_csharp.h"

GenerateRuleCSharp::GenerateRuleCSharp(const std::string& sFileName) : Parser(sFileName)
{
	m_mapSymbolTable.insert(std::make_pair("boolean", SymbolInfo( "bool", "false")));
	m_mapSymbolTable.insert(std::make_pair("byte",    SymbolInfo("sbyte", "0")));
	m_mapSymbolTable.insert(std::make_pair("ubyte",   SymbolInfo("byte", "0")));
	m_mapSymbolTable.insert(std::make_pair("int16",   SymbolInfo("short", "0")));	
	m_mapSymbolTable.insert(std::make_pair("int32",   SymbolInfo("int", "0")));
	m_mapSymbolTable.insert(std::make_pair("int64",   SymbolInfo("long", "0")));
	m_mapSymbolTable.insert(std::make_pair("uint16",  SymbolInfo("ushort", "0")));
	m_mapSymbolTable.insert(std::make_pair("uint32",  SymbolInfo("uint", "0")));
	m_mapSymbolTable.insert(std::make_pair("uint64",  SymbolInfo("ulong", "0")));
	m_mapSymbolTable.insert(std::make_pair("double",  SymbolInfo("double", "0.0")));
	m_mapSymbolTable.insert(std::make_pair("float",   SymbolInfo("float", "0.0f")));
}

const std::string GenerateRuleCSharp::TranslateVariableType(const Token::Base* pToken)
{
	switch(pToken->Type())
	{
		case Token::TYPE_PRIMITIVE :
			return m_mapSymbolTable[pToken->GetName()].m_sTypeName;
		case Token::TYPE_STRING :
			return "string";
		case Token::TYPE_MAP :
		{
			const Token::MapType* pMapType = static_cast<const Token::MapType*>(pToken);
			return "Dictionary<" 
					+ TranslateVariableType(pMapType->m_pKeyType) + ", " 
					+ TranslateVariableType(pMapType->m_pElmtType) 
				+ " >";
		}
		case Token::TYPE_STATIC_ARRAY :
		{
			const Token::StaticArrayType* pArrType = static_cast<const Token::StaticArrayType*>(pToken);
			return TranslateVariableType(pArrType->m_pElmtType) + "[]";
		}
		case Token::TYPE_ARRAY :
		{
			const Token::ArrayType* pArrType = static_cast<const Token::ArrayType*>(pToken);
			return "List<" + TranslateVariableType(pArrType->m_pElmtType) + " >";
		}
		case Token::TYPE_LIST :
		{
			const Token::ListType* pListType = static_cast<const Token::ListType*>(pToken);
			return "List<" + TranslateVariableType(pListType->m_pElmtType) + " >";
		}
		default :
		{
			if(m_mapSymbolTable.find(pToken->GetName()) == m_mapSymbolTable.end())
			{
				m_mapSymbolTable[pToken->GetName()].m_sTypeName = pToken->GetName();
				m_mapSymbolTable[pToken->GetName()].m_sInitValue = "";
			}
			return m_mapSymbolTable[pToken->GetName()].m_sTypeName;
		}
	}
	return std::string();
}

const std::string GenerateRuleCSharp::GenerateVariableInit(const Token::Base* typeInfo)
{
	switch(typeInfo->Type()) 
	{
	case Token::TYPE_PRIMITIVE :
		return m_mapSymbolTable[typeInfo->GetName()].m_sInitValue;
	case Token::TYPE_STRING :
		return "\"\"";
	case Token::TYPE_STATIC_ARRAY :
	{	
		const Token::StaticArrayType* pArrType = static_cast<const Token::StaticArrayType*>(typeInfo);
		return "new " + TranslateVariableType(pArrType->m_pElmtType) + "[" + pArrType->m_sElmtCount + "]";
	}
	default :
		return "new " + TranslateVariableType(typeInfo) + "()";
		break;
	}
	return std::string();
}

void GenerateRuleCSharp::GenerateVariableStore(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent)
{
	switch(typeInfo->Type())
	{
	case Token::TYPE_USERDEFINE :
		std::cout << sIndent << "if(false == " << typeInfo->GetName() << "_Serializer.Store(_buf_, " << sVarName << ")) { return false; }" << std::endl;
		break;
	case Token::TYPE_PRIMITIVE :
		std::cout << sIndent << "_buf_.Write(BitConverter.GetBytes(" << sVarName << "), 0, sizeof(" << TranslateVariableType(typeInfo) << "));" << std::endl;
		break;
	case Token::TYPE_STRING :
		std::cout << sIndent << "if(null != " << sVarName << ") {" << std::endl;
		std::cout << sIndent << "\t" << "int " << sVarName << "_length = Encoding.UTF8.GetByteCount(" << sVarName << ");" << std::endl;
		std::cout << sIndent << "\t" << "_buf_.Write(BitConverter.GetBytes(" << sVarName << "_length), 0, sizeof(int));" << std::endl;
		std::cout << sIndent << "\t" << "_buf_.Write(Encoding.UTF8.GetBytes(" << sVarName << "), 0, " << sVarName << "_length);" << std::endl;
		std::cout << sIndent << "}" << std::endl;
		std::cout << sIndent << "else {" << std::endl;
		std::cout << sIndent << "\t" << "_buf_.Write(BitConverter.GetBytes(0), 0, sizeof(int));" << std::endl;
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_MAP :
		std::cout << sIndent << "_buf_.Write(BitConverter.GetBytes(" << sVarName << ".Count), 0, sizeof(int));" << std::endl;
		std::cout << sIndent << "foreach(var " << sVarName << "_itr in " << sVarName << ") {" << std::endl;
		std::cout << sIndent << "\t" << TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType) << " " << sVarName << "_key = " << sVarName << "_itr.Key;" << std::endl;
		std::cout << sIndent << "\t" << TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType) << " " << sVarName << "_elmt = " << sVarName << "_itr.Value;" << std::endl;
		GenerateVariableStore(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType, sVarName+"_key", sIndent + "\t");
		GenerateVariableStore(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType, sVarName+"_elmt", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_LIST :
		std::cout << sIndent << "_buf_.Write(BitConverter.GetBytes(" << sVarName << ".Count), 0, sizeof(int));" << std::endl;
		std::cout << sIndent << "foreach(var " << sVarName << "_itr in " << sVarName << ") { " << std::endl;
		std::cout << sIndent << "\t" << TranslateVariableType(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType) << " " << sVarName << "_elmt = " << sVarName << "_itr;" << std::endl;
		GenerateVariableStore(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType, sVarName +"_elmt", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_ARRAY :
		std::cout << sIndent << "_buf_.Write(BitConverter.GetBytes(" << sVarName << ".Count), 0, sizeof(int));" << std::endl;
		std::cout << sIndent << "foreach(var " << sVarName << "_itr in " << sVarName << ") { " << std::endl;
		std::cout << sIndent << "\t" << TranslateVariableType(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType) << " " << sVarName << "_elmt = " << sVarName << "_itr;" << std::endl;
		GenerateVariableStore(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType, sVarName +"_elmt", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_STATIC_ARRAY :
		std::cout << sIndent << "for(int " << sVarName << "_itr=0; " << sVarName << "_itr<" << static_cast<const Token::StaticArrayType*>(typeInfo)->m_sElmtCount << "; " << sVarName << "_itr++) {" << std::endl;
		GenerateVariableStore(static_cast<const Token::StaticArrayType*>(typeInfo)->m_pElmtType, sVarName + "[" + sVarName +  "_itr]", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	default :
		std::cerr << "undefined type(" << typeInfo->GetName() << ") store error" << std::endl;
		break;
	}
}

void GenerateRuleCSharp::GenerateVariableLoad(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent)
{
	switch(typeInfo->Type())
	{
	case Token::TYPE_USERDEFINE :
		std::cout << sIndent << "if(false == " << typeInfo->GetName() << "_Serializer.Load(ref " << sVarName << ", _buf_)) { return false; }" << std::endl;
		break;
	case Token::TYPE_PRIMITIVE :
		std::cout << sIndent << "if(sizeof(" << TranslateVariableType(typeInfo) << ") > _buf_.Length - _buf_.Position) { return false; }" << std::endl;
		if("byte" == typeInfo->GetName()||"ubyte" == typeInfo->GetName())
		{
			std::cout << sIndent << sVarName << " = (" << TranslateVariableType(typeInfo) << ")_buf_.GetBuffer()[_buf_.Position];" << std::endl;
		}
		if("boolean" == typeInfo->GetName())
		{
			std::cout << sIndent << sVarName << " = BitConverter.ToBoolean(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
		}
		else if("int16" == typeInfo->GetName())
		{
			std::cout << sIndent <<  sVarName << " = BitConverter.ToInt16(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
		}
		else if("int32" == typeInfo->GetName())
		{
			std::cout << sIndent <<  sVarName << " = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
		}
		else if("int64" == typeInfo->GetName())
		{
			std::cout << sIndent <<  sVarName << " = BitConverter.ToInt64(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
		}
		else if("uint16" == typeInfo->GetName())
		{
			std::cout << sIndent <<  sVarName << " = BitConverter.ToUInt16(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
		}
		else if("uint32" == typeInfo->GetName())
		{
			std::cout << sIndent <<  sVarName << " = BitConverter.ToUInt32(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
		}
		else if("uint64" == typeInfo->GetName())
		{
			std::cout << sIndent <<  sVarName << " = BitConverter.ToUInt64(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
		}
		else if("double" == typeInfo->GetName())
		{
			std::cout << sIndent <<  sVarName << " = BitConverter.ToDouble(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
		}
		else if("float" == typeInfo->GetName())
		{
			std::cout << sIndent << sVarName << " = BitConverter.ToSingle(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
		}
		std::cout << sIndent << "_buf_.Position += sizeof(" << TranslateVariableType(typeInfo) << ");" << std::endl;
		break;
	case Token::TYPE_STRING :
		std::cout << sIndent << "if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }" << std::endl;
		std::cout << sIndent << "int " << sVarName << "_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
		std::cout << sIndent << "_buf_.Position += sizeof(int);" << std::endl;
		std::cout << sIndent << "if(" << sVarName << "_length > _buf_.Length - _buf_.Position) { return false; }" << std::endl;
		std::cout << sIndent << "byte[] " << sVarName << "_buf = new byte[" << sVarName << "_length];" << std::endl;
		std::cout << sIndent << "Array.Copy(_buf_.GetBuffer(), (int)_buf_.Position, " << sVarName << "_buf, 0, " << sVarName << "_length);" << std::endl;
		std::cout << sIndent << sVarName << " = System.Text.Encoding.UTF8.GetString(" << sVarName << "_buf);" << std::endl;
		std::cout << sIndent << "_buf_.Position += " << sVarName << "_length;" << std::endl;
		break;
	case Token::TYPE_MAP :
		{
			std::cout << sIndent << "if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }" << std::endl;
			std::cout << sIndent << "int " << sVarName << "_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
			std::cout << sIndent << "_buf_.Position += sizeof(int);" << std::endl;
			std::cout << sIndent << "for(int " << sVarName << "_itr=0; " << sVarName << "_itr<" << sVarName << "_length; " << sVarName << "_itr++) {" << std::endl;
			const std::string sKeyType = TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType);
			std::cout << sIndent << "\t" << sKeyType << " " << sVarName << "_key = " << GenerateVariableInit(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType) << ";" << std::endl;
			const std::string sElmtType = TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType);
			std::cout << sIndent << "\t" << sElmtType << " " << sVarName << "_elmt = " << GenerateVariableInit(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType) << ";" << std::endl;
			GenerateVariableLoad(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType, sVarName+"_key", sIndent + "\t");
			GenerateVariableLoad(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType, sVarName+"_elmt", sIndent + "\t");
			std::cout << sIndent << "\t" << sVarName << "[" << sVarName << "_key] = " << sVarName << "_elmt;" << std::endl;
			std::cout << sIndent << "}" << std::endl;
		}
		break;
	case Token::TYPE_LIST :
		{
			std::cout << sIndent << "if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }" << std::endl;
			std::cout << sIndent << "int " << sVarName << "_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
			std::cout << sIndent << "_buf_.Position += sizeof(int);" << std::endl;
			std::cout << sIndent << "for(int " << sVarName << "_itr=0; " << sVarName << "_itr<" << sVarName << "_length; " << sVarName << "_itr++) {" << std::endl;
			const std::string sVarType = TranslateVariableType(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType);
			std::cout << sIndent <<	"\t" << sVarType << " " << sVarName << "_val = " << GenerateVariableInit(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType) << ";" << std::endl;
			GenerateVariableLoad(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType, sVarName+"_val", sIndent+"\t");
			std::cout << sIndent << "\t" << sVarName << ".Add(" << sVarName << "_val);" << std::endl;
			std::cout << sIndent << "}" << std::endl;
		}
		break;
	case Token::TYPE_ARRAY :
		{
			std::cout << sIndent << "if(sizeof(int) > _buf_.Length - _buf_.Position) { return false; }" << std::endl;
			std::cout << sIndent << "int " << sVarName << "_length = BitConverter.ToInt32(_buf_.GetBuffer(), (int)_buf_.Position);" << std::endl;
			std::cout << sIndent << "_buf_.Position += sizeof(int);" << std::endl;
			std::cout << sIndent << "for(int " << sVarName << "_itr=0; " << sVarName << "_itr<" << sVarName << "_length; " << sVarName << "_itr++) {" << std::endl;
			const std::string sVarType = TranslateVariableType(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType);
			std::cout << sIndent <<	"\t" << sVarType << " " << sVarName << "_val = " << GenerateVariableInit(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType) << ";" << std::endl;
			GenerateVariableLoad(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType, sVarName+"_val", sIndent+"\t");
			std::cout << sIndent << "\t" << sVarName << ".Add(" << sVarName << "_val);" << std::endl;
			std::cout << sIndent << "}" << std::endl;
		}
		break;
	case Token::TYPE_STATIC_ARRAY :
		{
			std::cout << sIndent << "for(int " << sVarName << "_itr=0; " << sVarName << "_itr<" << static_cast<const Token::StaticArrayType*>(typeInfo)->m_sElmtCount << "; " << sVarName << "_itr++) {" << std::endl;
			const std::string sVarType = TranslateVariableType(static_cast<const Token::StaticArrayType*>(typeInfo)->m_pElmtType);
			std::cout << sIndent <<	"\t" << sVarType << " " << sVarName << "_val = " << GenerateVariableInit(static_cast<const Token::StaticArrayType*>(typeInfo)->m_pElmtType) << ";" << std::endl;
			GenerateVariableLoad(static_cast<const Token::StaticArrayType*>(typeInfo)->m_pElmtType, sVarName + "_val", sIndent + "\t");
			std::cout << sIndent << "\t" << sVarName << "[" << sVarName << "_itr] = " << sVarName << "_val;" << std::endl;
			std::cout << sIndent << "}" << std::endl;
		}
		break;
	default :
		std::cerr << "undefined type(" << typeInfo->GetName() << ") size error" << std::endl;
		break;
	}
}

void GenerateRuleCSharp::GenerateVariableSize(const Token::Base* typeInfo, const std::string& sVarName, const std::string& sIndent)
{
	switch(typeInfo->Type())
	{
	case Token::TYPE_USERDEFINE :
		std::cout << sIndent << "nSize += " << typeInfo->GetName() << "_Serializer.Size(" << sVarName << ");" << std::endl;
		break;
	case Token::TYPE_PRIMITIVE :
		std::cout << sIndent << "nSize += sizeof(" << TranslateVariableType(typeInfo) << ");" << std::endl;
		break;
	case Token::TYPE_STRING :
		std::cout << sIndent << "nSize += sizeof(int); " << std::endl;
		std::cout << sIndent << "if(null != " << sVarName << ") { nSize += Encoding.UTF8.GetByteCount(" << sVarName << "); }" << std::endl;
		break;
	case Token::TYPE_MAP :
		std::cout << sIndent << "nSize += sizeof(int);" << std::endl;
		std::cout << sIndent << "foreach(var " << sVarName << "_itr in " << sVarName << ") {" << std::endl;
		std::cout << sIndent << "\t" << TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType) << " " << sVarName << "_key = " << sVarName << "_itr.Key;" << std::endl;
		std::cout << sIndent << "\t" << TranslateVariableType(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType) << " " << sVarName << "_elmt = " << sVarName << "_itr.Value;" << std::endl;
		GenerateVariableSize(static_cast<const Token::MapType*>(typeInfo)->m_pKeyType, sVarName+"_key", sIndent + "\t");
		GenerateVariableSize(static_cast<const Token::MapType*>(typeInfo)->m_pElmtType, sVarName+"_elmt", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_LIST :
		std::cout << sIndent << "nSize += sizeof(int);" << std::endl;
		std::cout << sIndent << "foreach(var " << sVarName << "_itr in " << sVarName << ") { " << std::endl;
		std::cout << sIndent << "\t" << TranslateVariableType(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType) << " " << sVarName << "_elmt = " << sVarName << "_itr;" << std::endl;
		GenerateVariableSize(static_cast<const Token::ListType*>(typeInfo)->m_pElmtType, sVarName +"_elmt", sIndent + "\t");
		std::cout << sIndent << "}" << std::endl;
		break;
	case Token::TYPE_ARRAY :
		std::cout << sIndent << "nSize += sizeof(int);" << std::endl;
		std::cout << sIndent << "foreach(var " << sVarName << "_itr in " << sVarName << ") { " << std::endl;
		std::cout << sIndent << "\t" << TranslateVariableType(static_cast<const Token::ArrayType*>(typeInfo)->m_pElmtType) << " " << sVarName << "_elmt = " << sVarName << "_itr;" << std::endl;
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
			std::cout << sIndent << "for(int " << sVarName << "_itr=0; " << sVarName << "_itr<" << static_cast<const Token::StaticArrayType*>(typeInfo)->m_sElmtCount << "; " << sVarName << "_itr++) {" << std::endl;
			GenerateVariableSize(static_cast<const Token::StaticArrayType*>(typeInfo)->m_pElmtType, sVarName + "[" + sVarName + "_itr]", sIndent + "\t");
			std::cout << sIndent << "}" << std::endl;
		}
		break;
	default :
		std::cerr << "undefined type(" << typeInfo->GetName() << ") size error" << std::endl;
		break;
	}
}

bool GenerateRuleCSharp::CompileMessage(const Token::Message* pToken)
{
	std::cout << "public class " << pToken->GetName();
	if("" != pToken->m_sParentName) 
	{
		std::cout << " : " << pToken->m_sParentName;
	}
	std::cout << " {" << std::endl;
	if(pToken->m_nSeqNo > 0)
	{
		std::cout << "\t" << "public const int MSG_ID = " << pToken->m_nSeqNo << ";" << std::endl;
	}

	// member list
	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_NULL == (*itr)->Type())
		{
			continue;
		}
		const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(*itr);
		std::cout << "\t" << "public " << TranslateVariableType(pVarDecl->m_pVarType) <<
			     "\t" << pVarDecl->m_pVarName->GetName() << " = " << GenerateVariableInit(pVarDecl->m_pVarType);
		std::cout << ";" << std::endl;
	}

	// constructor
	std::cout << "\t" << "public " << pToken->GetName() << "() {" << std::endl;
	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_NULL == (*itr)->Type())
		{
			continue;
		}
		const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(*itr);
		if(Token::TYPE_STATIC_ARRAY == pVarDecl->m_pVarType->Type())
		{
			const Token::StaticArrayType* pArrType = static_cast<const Token::StaticArrayType*>(pVarDecl->m_pVarType);
			std::cout << "\t\t" << "for(int i=0;i<" << pArrType->m_sElmtCount << "; i++) { ";
			std::cout << pVarDecl->m_pVarName->GetName() << "[i] = new " << TranslateVariableType(pArrType->m_pElmtType) << "();";
			std::cout << " }" << std::endl;
		}
	}
	std::cout << "\t}" << std::endl;

	std::cout << "\t" << "public int Size() {" << std::endl;
	std::cout << "\t\t" << "int nSize = 0;" << std::endl;
	std::cout << "\t\t" << "try {" << std::endl;
	if("" != pToken->m_sParentName)
	{
		std::cout << "\t\t\t" << "nSize = base.Size();" << std::endl;
	}
	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_NULL == (*itr)->Type())
		{
			continue;
		}
		const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(*itr);
		GenerateVariableSize(pVarDecl->m_pVarType, pVarDecl->m_pVarName->GetName(), "\t\t\t");
	}
	std::cout << "\t\t" << "} catch(System.Exception) {" << std::endl;
	std::cout << "\t\t\t" << "return -1;" << std::endl;
	std::cout << "\t\t" << "}" << std::endl;
	std::cout << "\t\t" << "return nSize;" << std::endl;
	std::cout << "\t" << "}" << std::endl;

	std::cout << "\t" << "public bool Store(MemoryStream _buf_) {" << std::endl;
	std::cout << "\t\t" << "try {" << std::endl;
	if("" != pToken->m_sParentName)
	{
		std::cout << "\t\t\t" << "base.Store(_buf_);" << std::endl;
	}
	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_NULL == (*itr)->Type())
		{
			continue;
		}
		const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(*itr);
		GenerateVariableStore(pVarDecl->m_pVarType, pVarDecl->m_pVarName->GetName(), "\t\t\t");
	}
	std::cout << "\t\t" << "} catch(System.Exception) {" << std::endl;
	std::cout << "\t\t\t" << "return false;" << std::endl;
	std::cout << "\t\t" << "}" << std::endl;
	std::cout << "\t\t" << "return true;" << std::endl;
	std::cout << "\t" << "}" << std::endl;


	std::cout << "\t" << "public bool Load(MemoryStream _buf_) {" << std::endl;
	std::cout << "\t\t" << "try {" << std::endl;
	if("" != pToken->m_sParentName)
	{
		std::cout << "\t\t\t" << "if(false == base.Load(_buf_)) return false;" << std::endl;
	}
	for(std::list<Token::Base*>::const_iterator itr = pToken->list_.begin(); itr != pToken->list_.end(); itr++)
	{
		if(Token::TYPE_NULL == (*itr)->Type())
		{
			continue;
		}
		const Token::VarDecl* pVarDecl = static_cast<const Token::VarDecl*>(*itr);
		GenerateVariableLoad(pVarDecl->m_pVarType, pVarDecl->m_pVarName->GetName(), "\t\t\t");
	}
	std::cout << "\t\t" << "} catch(System.Exception) {" << std::endl;
	std::cout << "\t\t\t" << "return false;" << std::endl;
	std::cout << "\t\t" << "}" << std::endl;
	std::cout << "\t\t" << "return true;" << std::endl;
	std::cout << "\t" << "}" << std::endl;
	std::cout << "};" << std::endl;

	std::cout << "public struct " << pToken->GetName() << "_Serializer {" << std::endl;
	std::cout << "\t" << "public static bool Store(MemoryStream _buf_, " << pToken->GetName() << " obj) { return obj.Store(_buf_); }" << std::endl;
	std::cout << "\t" << "public static bool Load(ref " << pToken->GetName() << " obj, MemoryStream _buf_) { return obj.Load(_buf_); }" << std::endl;
	std::cout << "\t" << "public static int Size(" << pToken->GetName() << " obj) { return obj.Size(); }" << std::endl;
	std::cout << "};" << std::endl;

	return true;
}

bool GenerateRuleCSharp::CompileEnum(const Token::Enum* pToken)
{
	std::cout << "public enum " << pToken->GetName() << " {" << std::endl;
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
			std::cout << " = " << pElmt->m_sNumber;
		}
		std::cout << "," << std::endl;
	}
	std::cout << "}; // " << pToken->GetName() << std::endl;

	std::cout << "public struct " << pToken->GetName() << "_Serializer {" << std::endl;
	std::cout << "\t" << "public static bool Store(System.IO.MemoryStream _buf_, " << pToken->GetName() << " obj) { " << std:: endl;
	std::cout << "\t\t" << "try {" << std::endl;
	std::cout << "\t\t\t" << "_buf_.Write(System.BitConverter.GetBytes((int)obj), 0, sizeof(" << pToken->GetName() << "));" << std::endl;
	std::cout << "\t\t" << "}" << std::endl;
	std::cout << "\t\t" << "catch(System.Exception) {" << std::endl;
	std::cout << "\t\t\t" << "return false;" << std::endl;
	std::cout << "\t\t" << "}" << std::endl;
	std::cout << "\t\t" << "return true;" << std::endl;
	std::cout << "\t" << "}" << std::endl;

	std::cout << "\t" << "public static bool Load(ref " << pToken->GetName() << " obj, MemoryStream _buf_) { " << std::endl;
	std::cout << "\t\t" << "try {" << std::endl;
	std::cout << "\t\t\t" << "obj = (" << pToken->GetName() << ")System.BitConverter.ToInt32(_buf_.ToArray(), (int)_buf_.Position);" << std::endl;
	std::cout << "\t\t\t" << "_buf_.Position += sizeof(" << pToken->GetName() << ");" << std::endl;
	std::cout << "\t\t" << "}" << std::endl;
	std::cout << "\t\t" << "catch(System.Exception) { " << std::endl;
	std::cout << "\t\t\t" << "return false;" << std::endl;
	std::cout << "\t\t" << "}" << std::endl;
	std::cout << "\t\t" << "return true;" << std::endl;
	std::cout << "\t" << "}" << std::endl;
	std::cout << "\t" << "public static System.Int32 Size(" << pToken->GetName() << " obj) { return sizeof(" << pToken->GetName() << "); }" << std::endl;
	std::cout << "};" << std::endl;

	return true;
}

bool GenerateRuleCSharp::CompileStmtList(const Token::StmtList* pToken)
{
	std::cout << "using System;" << std::endl;
	std::cout << "using System.Collections.Generic;" << std::endl;
	//std::cout << "using System.Linq;" << std::endl;
	std::cout << "using System.Text;" << std::endl;
	//std::cout << "using System.Threading.Tasks;" << std::endl;
	std::cout << "using System.IO;" << std::endl;
	
	std::cout << "namespace "<< m_sFileName << "{" <<std::endl;
	if(false == Parse(pToken))
	{
		return false;
	}
	
	for(std::list<Token::Typedef*>::const_iterator itr=m_lstTypedef.begin(); itr!=m_lstTypedef.end(); itr++)
	{
		const Token::Typedef* pToken = *itr;
		std::cout << "public struct " << pToken->m_pVarDecl->m_pVarName->GetName() << "_Serializer {" << std::endl;
		std::cout << "\t" << "public static bool Store(MemoryStream _buf_, " << pToken->m_pVarDecl->m_pVarName->GetName() << " obj) { " << std:: endl;
		std::cout << "\t\t" << "try {" << std::endl;
		GenerateVariableStore(pToken->m_pVarDecl->m_pVarType, "obj", "\t\t\t");
		std::cout << "\t\t" << "} catch(System.Exception) {" << std::endl;
		std::cout << "\t\t\t" << "return false;" << std::endl;
		std::cout << "\t\t" << "};" << std::endl;
		std::cout << "\t\t" << "return true;" << std::endl;
		std::cout << "\t}" << std::endl;

		std::cout << "\t" << "public static bool Load(ref " << pToken->m_pVarDecl->m_pVarName->GetName() << " obj, MemoryStream _buf_) { " << std::endl;
		std::cout << "\t\t" << "try {" << std::endl;
		GenerateVariableLoad(pToken->m_pVarDecl->m_pVarType, "obj", "\t\t");
		std::cout << "\t\t" << "} catch(System.Exception) {" << std::endl;
		std::cout << "\t\t\t" << "return false;" << std::endl;
		std::cout << "\t\t" << "};" << std::endl;
		std::cout << "\t\t" << "return true;" << std::endl;
		std::cout << "\t" << "}" << std::endl;

		std::cout << "\t" << "public static int Size(" << pToken->m_pVarDecl->m_pVarName->GetName() << " obj) { " << std::endl;
		std::cout << "\t\t" << "int nSize = 0;" << std::endl;
		std::cout << "\t\t" << "try {" << std::endl;
		GenerateVariableSize(pToken->m_pVarDecl->m_pVarType, "obj", "\t\t");
		std::cout << "\t\t" << "} catch(System.Exception) {" << std::endl;
		std::cout << "\t\t\t" << "return -1;" << std::endl;
		std::cout << "\t\t" << "};" << std::endl;
		std::cout << "\t\t" << "return nSize;" << std::endl;
		std::cout << "\t}" << std::endl;

		std::cout << "};" << std::endl;
	}
	std::cout << "}" << std::endl;
	return true;
}

bool GenerateRuleCSharp::ComplieLiteralBlock(const Token::LiteralBlock* pToken)
{
	if(".cs" == pToken->m_sLanguage)
	{
		std::cout << pToken->m_sContext << std::endl;
	}
	return true;
}

bool GenerateRuleCSharp::CompileTypedef(const Token::Typedef* pToken)
{
	std::cout << "using " << pToken->m_pVarDecl->m_pVarName->GetName() << " = " << TranslateVariableType(pToken->m_pVarDecl->m_pVarType) << ";" << std::endl;
	m_lstTypedef.push_back(const_cast<Token::Typedef*>(pToken));
	return true;
}

