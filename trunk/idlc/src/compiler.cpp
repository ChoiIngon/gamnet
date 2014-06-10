#include "compiler.h"
#include <iostream>
#include <fstream>
#include "parser.h"
#include "generate_rule_cpp.h"
#include "generate_rule_python.h"
#include "generate_rule_csharp.h"

Compiler::Compiler(const std::string& sFileName, const std::string& sOption) : m_sFileName(sFileName), m_sOption(sOption)
{
}

Compiler::~Compiler()
{
}

bool Compiler::Compile()
{
	size_t nFileNameStartPos = m_sFileName.rfind('\\') + 1;
	nFileNameStartPos = nFileNameStartPos > m_sFileName.rfind("/") + 1 ? nFileNameStartPos : m_sFileName.rfind("/") + 1;
	std::string sSrcFilePath = "";
	sSrcFilePath = m_sFileName.substr(0, nFileNameStartPos);

	size_t nFileNameEndPos = m_sFileName.rfind('.');
	if(std::string::npos == nFileNameEndPos)
	{
		std::cout << "wrong input file format" << std::endl;
		return false;
	}

	const std::string sOutPath = sSrcFilePath;

	std::string sFileName = m_sFileName.substr(nFileNameStartPos, nFileNameEndPos-nFileNameStartPos);
	std::string sOutFile = "";

	Token::Parser* pParser = NULL;
	if("py" == m_sOption)
	{
		sOutFile = sOutPath + sFileName + ".py";
		pParser = new GenerateRulePython(sFileName);
	}
	else if("cpp" == m_sOption)
	{
		sOutFile = sOutPath + sFileName + ".h";
		pParser = new GenerateRuleCpp(sFileName);
	}
	else if("cs" == m_sOption)
	{
		sOutFile = sOutPath + sFileName + ".cs";
		pParser = new GenerateRuleCSharp(sFileName);
	}
	std::cout << "in-file:" << m_sFileName << std::endl;
	std::cout << "outfile:" << sOutFile << std::endl;
	Token::StmtList* pRoot =  MakeParseTree(m_sFileName.c_str());
	if(NULL == pRoot)
	{
		return false;
	}

	std::ofstream output;
	std::streambuf* pOld;
	if("" != sFileName)
	{
		output.open(sOutFile.c_str());
		if(output.fail())
		{
			std::cout << "fail(can't open file)" << std::endl;
			return false;
		}
		pOld = std::cout.rdbuf(output.rdbuf());
	}

	if(false == pParser->CompileStmtList(pRoot))
	{
		return false;
	};

	std::cout.rdbuf(pOld);
	output.close();

	std::cout << "complete compile(" << m_sFileName << " -> " << sOutFile << ")" << std::endl;
	return true;
}
