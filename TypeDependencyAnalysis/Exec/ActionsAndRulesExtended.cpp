
//////////////////////////////////////////////////////////////////////////
//	ActionsAndRulesExtended.cpp - the function body of some extended	//
//	functions defined in ActionsAndRulesExtended.h						//
//																		//
//	ver 0.1																//
//	Language:		Visual C++ 2012, SP1								//
//	Platform:		Dell Studio 1558, Windows 7 Pro x64 Sp1				//
//	Application:	Type Dependency Analysis, CIS687 Pr#1, 2013 Spring	//
//	Author:			Kevin Wang, Syracuse University						//
//					kevixw@gmail.com									//
//////////////////////////////////////////////////////////////////////////
/*
Package Operations:
===================
This file only include the definition of getKnownParentElem();
getKnownParentElem() is used in ActionsAndRulesExtended.h, aim to finding the 
parent element of current element with specific type.  This function will look
through the whole scope stack and return the first found element which fulfill
the requirement.

Usage:
getKnownParentElem(p_Repos, "type");
getKnownParentElem(p_Repos, "type|namespace");	// looking for either "type" type or "namespace" type

*/

#include "ActionsAndRulesExtended.h"
#include "ActionsAndRules.h"

// the definition of getKnownParentElem(), looking for a parent element with specific type
element getKnownParentElem(Repository* pRepos, std::string type) {
	element elem, res;
	if (pRepos->scopeStack().size()<1)	// return an empty element
		return res;
	elem = pRepos->scopeStack().pop();
	if (("|"+type+"|").find("|"+elem.type+"|") == std::string::npos && pRepos->scopeStack().size()>0)
		res = getKnownParentElem(pRepos,type);	// call recursively
	else
		res = elem;
	// push back
	pRepos->scopeStack().push(elem);
	return res;
};


#ifdef TEST_ACTIONSANDRULES_EXTENDED

#include <iostream>
#include "ActionsAndRules.h"
#include "ActionsAndRulesExtended.h"
#include "Tokenizer.h"
#include "SemiExpression.h"

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing ActionsAndRulesExtended class\n "
		<< std::string(30,'=') << std::endl;

	try
	{
		//std::queue<std::string> resultsQ;
		TypeDefinition* pTypeDefinition = new TypeDefinition;
		NamespaceDefinition* pNamespaceDefinition = new NamespaceDefinition;

		Toker toker("../ActionsAndRules.h");
		SemiExp se(&toker);
		Parser parser(&se);
		parser.addRule(&pTypeDefinition);
		parser.addRule(&pNamespaceDefinition);

		while(se.get())
			parser.parse();
		std::cout << "\n\n";
	}
	catch(std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
	}
}
#endif