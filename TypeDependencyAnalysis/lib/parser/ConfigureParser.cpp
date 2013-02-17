/////////////////////////////////////////////////////////////////////
//  ConfigureParser.cpp - builds and configures parsers            //
//  ver 2.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2005                                 //
//  Platform:      Dell Dimension 9150, Windows XP SP2             //
//  Application:   Prototype for CSE687 Pr1, Sp06                  //
//  Author:        Jim Fawcett, CST 2-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include <string>
#include "Parser.h"
#include "SemiExpression.h"
#include "Tokenizer.h"
#include "ActionsAndRules.h"
#include "ActionsAndRulesExtended.h"
#include "ConfigureParser.h"
#include "Graph.h"

//----< destructor releases all parts >------------------------------

ConfigParseToConsole::~ConfigParseToConsole()
{
	// when Builder goes out of scope, everything must be deallocated

	delete pHandlePush;
	delete pBeginningOfScope;
	//delete pHandlePop;
	delete pEndOfScope;
	delete pPushFunction;
	delete pFunctionDefinition;
	delete pFR;
	delete pRepo;
	delete pParser;
	delete pSemi;
	delete pToker;
	delete pHandlePopExtended;
	delete pTypeDefinition;
	delete pPushType;
	delete pTypeDefDefinition;
	delete pPushTypeDef;
	delete pEnumDefinition;
	delete pPushEnum;
	delete pPushArgument;
	delete pPushField;
	delete pSimicolonDetector;
	delete pNamespaceDefinition;
	delete pPushNamespace;
	delete pForLoopDefinition;
	delete pPushForLoop;
	delete pExceptionCatchDefinition;
	delete pPushExceptionCatch;
	delete pExceptionTryDefinition;
	delete pPushExceptionTry;
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseToConsole::Attach(const std::string& name, bool isFile)
{
	if(pToker == 0)
		return false;
	return pToker->attach(name, isFile);
}
//----< Here's where alll the parts get assembled >----------------

Parser* ConfigParseToConsole::Build()
{
	try
	{
		// add Parser's main parts

		pToker = new Toker;
		pToker->returnComments(false);	// set as do not return comment
		pSemi = new SemiExp(pToker);
		pParser = new Parser(pSemi);
		pRepo = new Repository(pToker, pGraph);
		// add code folding rules

		pFR = new codeFoldingRules;
		pParser->addFoldingRules(pFR);

		// configure to manage scope
		// these must come first - they return true on match
		// so rule checking continues

		pBeginningOfScope = new BeginningOfScope();
		pHandlePush = new HandlePush(pRepo);
		pBeginningOfScope->addAction(pHandlePush);
		pParser->addRule(pBeginningOfScope);

		pEndOfScope = new EndOfScope();
		//pHandlePop = new HandlePop(pRepo);
		//pEndOfScope->addAction(pHandlePop);
		pHandlePopExtended = new HandlePopExtended(pRepo);
		pEndOfScope->addAction(pHandlePopExtended);
		pParser->addRule(pEndOfScope);

		pSimicolonDetector = new SimicolonDetector;
		pPushField = new PushField(pRepo);
		pSimicolonDetector->addAction(pPushField);
		pParser->addRule(pSimicolonDetector);

		pExceptionTryDefinition = new ExceptionTryDefinition;
		pPushExceptionTry = new PushExceptionTry(pRepo);
		pExceptionTryDefinition->addAction(pPushExceptionTry);
		pParser->addRule(pExceptionTryDefinition);

		pNamespaceDefinition = new NamespaceDefinition;
		pPushNamespace = new PushNamespace(pRepo);
		pNamespaceDefinition->addAction(pPushNamespace);
		pParser->addRule(pNamespaceDefinition);

		// configure to detect and act on function definitions
		// these will stop further rule checking by returning false

		pFunctionDefinition = new FunctionDefinition;
		pPushFunction = new PushFunction(pRepo);  // no action
		pPushArgument = new PushArgument(pRepo);
		pFunctionDefinition->addAction(pPushFunction);
		// must push function first then push function argument
		pFunctionDefinition->addAction(pPushArgument);
		pParser->addRule(pFunctionDefinition);

		pTypeDefinition = new TypeDefinition;
		pPushType = new PushType(pRepo);
		pPushTypeInheritance = new PushTypeInheritance(pRepo);
		pTypeDefinition->addAction(pPushType);
		// must push type first then push type inheritance
		pTypeDefinition->addAction(pPushTypeInheritance);
		pParser->addRule(pTypeDefinition);

		pTypeDefDefinition = new TypeDefDefinition;
		pPushTypeDef = new PushTypeDef(pRepo);
		pTypeDefDefinition->addAction(pPushTypeDef);
		pParser->addRule(pTypeDefDefinition);

		//pExceptionCatchDefinition = new ExceptionCatchDefinition;
		pExceptionCatchDefinition = new BracketStatementDefinition("catch");
		pPushExceptionCatch = new PushExceptionCatch(pRepo);
		pExceptionCatchDefinition->addAction(pPushExceptionCatch);
		pParser->addRule(pExceptionCatchDefinition);

		pForLoopDefinition = new BracketStatementDefinition("for");
		pPushForLoop = new PushForLoop(pRepo);
		pForLoopDefinition->addAction(pPushForLoop);
		pParser->addRule(pForLoopDefinition);

		pEnumDefinition = new EnumDefinition;
		pPushEnum = new PushEnum(pRepo);
		pEnumDefinition->addAction(pPushEnum);
		pParser->addRule(pEnumDefinition);

		// push a empty file scope here, so that we need not to worry out of stack index
		element globalScope;
		globalScope.type="global_scope";
		globalScope.name="global";
		globalScope.lineCount = 0;
		pRepo->scopeStack().push(globalScope);

		return pParser;
	}
	catch(std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
		return 0;
	}
}

// return a pointer to the graph
GraphLib::Graph<std::string, std::string>* ConfigParseToConsole::graph() {
	return pGraph;
}

#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing ConfigureParser module\n "
		<< std::string(32,'=') << std::endl;

	// collecting tokens from files, named on the command line

	if(argc < 2)
	{
		std::cout 
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	for(int i=1; i<argc; ++i)
	{
		std::cout << "\n  Processing file " << argv[i];
		std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');

		ConfigParseToConsole configure;
		Parser* pParser = configure.Build();
		try
		{
			if(pParser)
			{
				if(!configure.Attach(argv[i]))
				{
					std::cout << "\n  could not open file " << argv[i] << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return 1;
			}
			// now that parser is built, use it

			while(pParser->next())
				pParser->parse();
			std::cout << "\n\n";
		}
		catch(std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
		std::cout << "\n\n";
	}
}

#endif
