#ifndef ACTIONSANDRULES_EXTENDED_H
#define ACTIONSANDRULES_EXTENDED_H

//////////////////////////////////////////////////////////////////////////
//	ActionsAndRulesExtended.cpp - the function body of some extended	//
//	functions defined in ActionsAndRulesExtended.h						//
//																		//
//	ver 0.1																//
//	Language:		Visual C++ 2012, SP1								//
//	Platform:		Dell Studio 1558, Windows 7 Pro x64 Sp1				//
//	Application:	Type Dependency Analysis							//
//	Author:			Kevin Wang, Syracuse University						//
//					kevixw@gmail.com									//
//////////////////////////////////////////////////////////////////////////
/*
Module Operations: 
==================
This module is an extension of ActionsAndRules.h, it includes detection of
Type (class, struct), namespace, enum, try-catch cause, if cause, while-loop,
for-loop, etc.  In some of the actions, we add the detected types into a graph,
where Vertex is type, Edge is the relationship between types

Maintanence Information:
========================
Required files:
---------------
Parser.h, ITokCollection.h, ScopeStack.h, Tokenizer.h, SemiExpression.h, ActionsAndRules.h, Graph.h, StringHelper.h

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv TypeDependencyAnalysis.sln /rebuild debug

Revision History:
-----------------
- ver 0.1 first version

*/

// define this will enable detecting template
#define DETECTING_TEMPLATE
// DEBUG: Show stack
//#define _DEBUG_A

#include <queue>
#include <string>
#include <sstream>
#include "Parser.h"
#include "ITokCollection.h"
#include "ScopeStack.h"
#include "Tokenizer.h"
#include "SemiExpression.h"
#include "ActionsAndRules.h"
#include "Graph.h"
#include "StringHelper.h"

// Rename vertex
typedef GraphLib::Vertex<std::string, std::string> vertex;

///////////////////////////////////////////////////////////////
// this func is used to get the parent element, extremely useful when trying figuring out in which scope we are now

element getKnownParentElem(Repository* pRepos, std::string type="type");

///////////////////////////////////////////////////////////////
// rule to detect statement, including variable declaration

class SimicolonDetector : public IRule
{
	// ignore some special phrases (see http://goo.gl/VC7Xn)
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
		= { "typedef", "typename", "return", "for", "while", "catch", "if" };
		for(int i=0; i<7; ++i)
			if(tok == keys[i])
				return true;
		return false;
	}
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		bool findEqual = false, findIncrement = false;
		if (tc.length()>1 && tc[tc.length()-1] == ";") {  // when .length()==1, means it is an empty phrase, or a {..};
			for (size_t i=0;i<tc.length()-1;i++) {
				if (isSpecialKeyWord(tc[i]))
					return false;
				// statement
				findEqual = findEqual || (tc[i].find("=") != std::string::npos);
				// increment
				findIncrement = findIncrement || tc[i]=="++" || tc[i]=="--";
			}
			if (!findEqual && findIncrement)
				return false;
			//std::cout<<"\n Simicolon detected!";
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// rule to detect type definitions, including "class" & "struct"

class TypeDefinition : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		const static std::string keys[] = { "class", "struct" };
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{")
		{
			for (int i=0;i<2;i++)
				if (tc.find(keys[i])<tc.length()) {
					doActions(pTc);
					return true;
				}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push type name onto ScopeStack

class PushType : public IAction
{
	Repository* p_Repos;
	std::string getTypeName(ITokCollection& tc) {
		std::string name, appendix;
		// we need to detect the template here, in case template<class A>
		size_t start=tc.find("template")+1;
		if (start<tc.length()) {
			for (size_t popCount=0;start<tc.find("{");start++) {
				if (Helper::ltrim(tc[start]).length()<1 || tc[start]=="class" || tc[start]=="typename")
					continue;
				if (tc[start]=="<")
					popCount++;
				else if (tc[start]==">")
					popCount--;
				appendix+=tc[start];
				if (popCount==0)
					break;
			}
			if (appendix=="<>")
				appendix="";
		}
		else
			start = 0;
		// find the key after the tamplate
		const static std::string keys[] = { "class", "struct" };
		bool isType = false;
		size_t len = 0;
		for (size_t i=start;i<tc.length()-1;i++) {
			for (size_t j=0;j<2;j++) {
				if (keys[j]==tc[i]) {
					isType=true;
					len=i;
					break;
				}
			}
			if (isType) break;
		}
		if (!isType)
			return "";
		if (len<tc.length()-1) {  // notice here is length()-1, for anonymous type
#ifdef DETECTING_TEMPLATE
			for (size_t j=len+1;j<tc.length()-1 && tc[j]!=":";j++)
				name+=Helper::ltrim(tc[j]);
#else
			for (size_t j=len+1;j<tc.length()-1 && tc[j]!=":" && tc[j]!="<";j++)
				name+=Helper::ltrim(tc[j]);
#endif
		}
		if (Helper::ltrim(name).length() < 1 || name=="{")
			name = "(anonymous type)";
		name += appendix;
		return name;
	}
public:
	PushType(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		// push type scope
		std::string name = getTypeName(*pTc);
		if (name.length()==0)	// it is a template function maybe
			return;
		// pop type anonymous scope
		p_Repos->scopeStack().pop();
		element parent_elem = getKnownParentElem(p_Repos,"type|namespace");
		//std::cout<<"\ntype scope "<<parent_elem.show();
		if (parent_elem.type=="namespace" || parent_elem.type=="type")	// if this is not the global_scope, maybe it's in a namespace, or in a nested class
			name = parent_elem.name+"::"+name;
		name = Helper::trim(name);
		element elem;
		elem.type = "type";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);	// replace anonymous scope with "type" scope

		// add type to graph here
		p_Repos->graph()->addVertexByValue(name);	// only when a type definition is detected

#ifdef _DEBUG_A
		std::cout<<"\n type stack";
		showStack(p_Repos->scopeStack());
#endif
	}
};

///////////////////////////////////////////////////////////////
// action to push type inheritance relationship onto ScopeStack

class PushTypeInheritance : public IAction
{
	Repository* p_Repos;
	bool isSpecialKeyword(const std::string& tok) {
		const static std::string keys[] = { "public", "private", "protected" };
		for(int i=0; i<3; ++i)
			if(tok == keys[i])
				return true;
		return false;
	}
public:
	PushTypeInheritance(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		std::string name = "";
		element parent_elem = getKnownParentElem(p_Repos);
		for (size_t i=pTc->find(":")+1, popCount=0;i<pTc->length();i++) {
			std::string tok = (*pTc)[i];
			if (isSpecialKeyword(tok) || Helper::ltrim(tok).length()<1)
				continue;
			if (tok.find("<") != std::string::npos)
				popCount++;
			else if (tok.find(">") != std::string::npos) {
				popCount--;
#ifdef DETECTING_TEMPLATE
				if (popCount==0)
					name+=tok.substr(0,tok.find(">")+1);
#endif
			}
			if (popCount==0 && name.length()>0 && (tok.find(">") != std::string::npos || i==pTc->length()-1)) {
				// add this type to graph here
				p_Repos->graph()->tryAddEdge("inheritance", parent_elem.name, name);

				name="";
			}
#ifdef DETECTING_TEMPLATE
			else
#else
			else if (popCount==0 && tok.compare(0,1,">")!=0)
#endif
				name += tok;

		}
	}
};

///////////////////////////////////////////////////////////////
// rule to detect special statement, like "for", "while", "catch"

class BracketStatementDefinition : public IRule
{
	std::string keyword;
public:
	BracketStatementDefinition(const std::string & key): keyword(key) {}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{" || tc[tc.length()-1] == ";")
		{
			size_t len = tc.find("(");	// check len>0 in case it's (statement);
			if(len>0 && len < tc.length() && tc[len-1]==keyword)
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// push for loop into stack scope

class PushForLoop : public IAction
{
	Repository* p_Repos;
	std::string getTypeName(ITokCollection& tc) {
		std::string name;
#ifdef DETECTING_TEMPLATE
		for (size_t i=tc.find("(") +1, popCount=0;i<tc.find(";")-1;i++){
			if (tc[i].find("<") != std::string::npos)
				popCount++;
			else if (tc[i].find(">") != std::string::npos) {	// consider the situation "Vector<V,E>&", ">&" will be extract as one token, so check "starts with" here
				popCount--;
				if (popCount==0) {
					name+=tc[i].substr(0,tc[i].find(">")+1);
					break;
				}
			}
			else if (name.length()>0)
				break;
			name += tc[i];
		}
#else
		name = tc[tc.find("(") +1];
#endif
		return name;
	}
public:
	PushForLoop(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		std::string name=getTypeName(*pTc);
		element elem;
		elem.type = "loop";
		elem.name = "for";
		elem.lineCount = p_Repos->lineCount();
		if((*pTc)[pTc->length()-1] == "{") {	// sometimes there is no scope follows
			p_Repos->scopeStack().pop();
			p_Repos->scopeStack().push(elem);
		}
		element parent_elem = getKnownParentElem(p_Repos);
		// definitely aggregation
		p_Repos->graph()->tryAddEdge("aggregation", parent_elem.name, name);
#ifdef _DEBUG_A
		std::cout<<"\n for-loop stack";
		showStack(p_Repos->scopeStack());
#endif
	}
};

///////////////////////////////////////////////////////////////
// puch exception capture onto stack

class PushExceptionCatch : public IAction
{
	Repository* p_Repos;
public:
	PushExceptionCatch(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;

		std::string name;
#ifdef DETECTING_TEMPLATE
		for (size_t i=tc.find("(") +1, popCount=0;i<tc.find(")")-1;i++){
			if (tc[i].find("<") != std::string::npos)
				popCount++;
			else if (tc[i].find(">") != std::string::npos) {	// consider the situation "Vector<V,E>&", ">&" will be extract as one token, so check "starts with" here
				popCount--;
				if (popCount==0) {
					name+=tc[i].substr(0,tc[i].find(">")+1);
					break;
				}
			}
			else if (name.length()>0)
				break;
			name += tc[i];
		}
#else
		name = tc[tc.find("(") +1];
#endif
		// aggregation here
		element elem;
		elem.type = "exception";
		elem.name = "catch";
		elem.lineCount = p_Repos->lineCount();
		if(tc[tc.length()-1] == "{") {	// sometimes there is no scope follows
			p_Repos->scopeStack().pop();
			p_Repos->scopeStack().push(elem);
		}
		element parent_elem = getKnownParentElem(p_Repos);
		p_Repos->graph()->tryAddEdge("aggregation", parent_elem.name, name);
#ifdef _DEBUG_A
		std::cout<<"\n catch stack";
		showStack(p_Repos->scopeStack());
#endif
	}
};

///////////////////////////////////////////////////////////////
// detect exception try part

class ExceptionTryDefinition : public IRule {
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{" && tc[0]=="try") {
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// push exception try part onto stack

class PushExceptionTry : public IAction
{
	Repository* p_Repos;
public:
	PushExceptionTry(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		p_Repos->scopeStack().pop();
		element elem;
		elem.type = "exception";
		elem.name = "try";
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);
#ifdef _DEBUG_A
		std::cout<<"\n try stack";
		showStack(p_Repos->scopeStack());
#endif
	}
};

///////////////////////////////////////////////////////////////
// detect namespace

class NamespaceDefinition : public IRule {
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{" && tc[0]=="namespace")
		{
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// push namespace onto scopestack

class PushNamespace : public IAction
{
	Repository* p_Repos;
public:
	PushNamespace(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push function scope
		std::string name = (*pTc)[1];
		for (size_t i=2;i<pTc->length()-1;i++)
			if (Helper::ltrim((*pTc)[i]).length()>0)
			name+=(*pTc)[i];
		element elem;
		elem.type = "namespace";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);
#ifdef _DEBUG_A
		std::cout<<"\n namespace stack";
		showStack(p_Repos->scopeStack());
#endif
	}
};

///////////////////////////////////////////////////////////////
// action to push field statement onto ScopeStack

class PushField : public IAction
{
	Repository* p_Repos;
	// if the first token is one of the following, this semiExpression must not be a statement
	bool isAcceptableKeyword(const std::string & tok) {
		static const std::string keys [] = {"(","+","-",".",">>","<<"};
		for (int i=0;i<6;i++)
			if (tok.find(keys[i]) != std::string::npos)
				return false;
		return true;
	}
	/////////////////////////////////////////////////////
	// There is an interest case: 1. new Class();
	// 2. (new Class());((new Class()));
	// 3. (new Class);
	// 4. new Class;
	std::string isSpecialCase_newOnly(ITokCollection& tc) {
		size_t start = 0;
		for (;start<tc.length()-1 && Helper::ltrim(tc[start]).length()<1;start++);
		if (tc[start]!="new" && tc[start]!="(")
			return "";
		std::string name;
		for (size_t i=start;i<tc.length()-1;i++) {
			if (Helper::ltrim(tc[i]).length()<1)
				continue;
			if (tc[i]!="new" && tc[i]!="(") {
				name = tc[i];
				break;
			}
		}
		return name;
	}
public:
	PushField(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		std::string name=isSpecialCase_newOnly(tc);
		if (name.length()>0) {
			element parent_elem = getKnownParentElem(p_Repos);
			if (parent_elem.type == "type")
				p_Repos->graph()->tryAddEdge("inheritance", name, parent_elem.name);
			return;
		}
		// these "non-acceptable keywords should not appear here
		if (tc.length()>2 && (!isAcceptableKeyword(tc[0]) || !isAcceptableKeyword(tc[1])))
			return;
		bool isPtr=false;
		for (size_t i=0, popCount=0;i<tc.length()-1;i++) {
			if (Helper::isModifierKeyword(tc[i]) || Helper::ltrim(tc[i]).length()<1)
				continue;
			else if (tc[i]==":")	// for public: and private: etc.
				name="";
			else {
				if (tc[i].find("<") != std::string::npos)
					popCount++;
				else if (tc[i].find(">") != std::string::npos) {	// consider the situation "Vector<V,E>&", ">&" will be extract as one token, so check "starts with" here
					popCount--;
#ifdef DETECTING_TEMPLATE
					if (popCount==0)
						name+=tc[i].substr(0,tc[i].find(">")+1);
#endif
				}
				if (name.length()>0 && popCount==0){
					int j=1;
					if ((isPtr=(tc[i].find("*")!= std::string::npos)||(tc[i].find("&")!= std::string::npos)) && (tc[i].find("=")== std::string::npos))
						j++;
					if (i+j>tc.length()-1)
						return;
					else if (tc[i+j].find("=") == std::string::npos && tc[i+j]!=";")   // this is not a field
						return;
					else
						break;
				}
				if (name.length()==0)
					name = tc[i];
				else if (popCount>0)
					name += tc[i];
			}
		}
		if (Helper::ltrim(name).length()<1)
			return;
		element parent_elem = getKnownParentElem(p_Repos,"type|namespace");
		if (parent_elem.type=="namespace")	// global field
			name = parent_elem.name+"::"+name;
		std::string elem_type;
		if (parent_elem.type=="global_scope" || parent_elem.type=="namespace") {  // global field
			elem_type = "global_variable";
		}
		else if (parent_elem.type=="type") {  // composition & aggregation
			if (isPtr || getKnownParentElem(p_Repos,"function").type=="function")
				elem_type = "aggregation";
			else
				elem_type = "composition";
			p_Repos->graph()->tryAddEdge(elem_type, parent_elem.name, name);
		}
		else
			elem_type = "???-"+parent_elem.type;	// unknown parent type!
	}
};

///////////////////////////////////////////////////////////////
// action to push function argument to repository

class PushArgument : public IAction
{
	Repository* p_Repos;
	bool isSpecialKeyword(const std::string& tok){
		const static std::string keys[] = { "const" };
		for(int i=0; i<1; ++i)
			if(tok == keys[i])
				return true;
		return false;
	}
public:
	PushArgument(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		size_t bracket_end = pTc->find(")")-1;	// for the constructor who may has more than one brackets

		std::string name = "";
		element parent_elem = getKnownParentElem(p_Repos);
		for (size_t i=pTc->find("(")+1, popCount=0, reset=0;i<bracket_end;i++) {
			std::string tok = (*pTc)[i];
			if (isSpecialKeyword(tok) || Helper::ltrim(tok).length()<1)	// ignore special keywords
				continue;
			if (tok.find("<") != std::string::npos)
				popCount++;
			else if (tok.find(">") != std::string::npos) {	// consider the situation "Vector<V,E>&", ">&" will be extract as one token, so check "starts with" here
				popCount--;
#ifdef DETECTING_TEMPLATE
				if (popCount==0)
					name+=tok.substr(0,tok.find(">")+1);
#endif
			}
			if (popCount==0 && name.length()>0) {
				if (!reset) {
					// using - if this function is in a "type" (it can be a global func)
					if (parent_elem.type=="type")
						p_Repos->graph()->tryAddEdge("using", parent_elem.name, name);

					name="";
					reset=1;
				}
				else if (tok.find(",") != std::string::npos) {	// reset until a comma is found
					reset=0;
					name="";
				}
			}
#ifdef DETECTING_TEMPLATE
			else
#else
			else if (popCount==0 && tok.compare(0,1,">")!=0)
#endif
				name += tok;
		}
	}
};

///////////////////////////////////////////////////////////////
// action to display element

class HandlePopExtended : public IAction
{
	Repository* p_Repos;
public:
	HandlePopExtended(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		if(p_Repos->scopeStack().size() == 0)
			return;
		element elem = p_Repos->scopeStack().pop();
		//if(elem.type != "unknown")
		//std::cout << "\n pop:"<<elem.show();
	}

};

///////////////////////////////////////////////////////////////
// rule to detect typedef alias

class TypeDefDefinition : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == ";")
		{
			if (tc.find("typedef")<tc.length()) {
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push typedef name onto ScopeStack

class PushTypeDef : public IAction
{
	Repository* p_Repos;
public:
	PushTypeDef(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		std::string alias;
		// push type scope
		for (int i=pTc->length()-2; i>0; i--) {
			if (Helper::ltrim((*pTc)[i]).length()>0) {
				if ((alias = (*pTc)[i])!=")")
					break;
				else
					return;	// well, this is a function pointer
			}
		}
		size_t len = pTc->find("typedef");
		std::string name = (*pTc)[len+1];
		for (size_t i=len+2;i<pTc->length()-2;i++) {	// connect the whole name
#ifndef DETECTING_TEMPLATE
			if ((*pTc)[i]=="<")
				break;
#endif
			name+=(*pTc)[i];
		}
		//std::cout<<"\n typedef "<<name<<" "<<alias;

		element parent_elem = getKnownParentElem(p_Repos,"type|namespace");
		if (parent_elem.type != "global_scope")
			alias = parent_elem.name +"::"+ alias;

		p_Repos->graph()->tryAddEdge("typedef", alias, name);
		//p_Repos->graph()->addOrFindVertexIndexByValue(name);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect enum definitions, including "class" & "struct"

class EnumDefinition : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{")
		{
			if (tc.find("enum")<tc.length()) {
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push enum name onto ScopeStack

class PushEnum : public IAction
{
	Repository* p_Repos;
public:
	PushEnum(Repository* pRepos) : p_Repos(pRepos) {}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push type scope
		size_t len = pTc->find("enum");
		std::string name = (*pTc)[len +1];
		// connect namespace etc.
		for (size_t i=len+2;i<pTc->length()&&(*pTc)[i]!="{";i++)
			name+=(*pTc)[i];
		if (Helper::ltrim(name).length() < 1 || name=="{")	// anonymous enum
			name = "(anonymous enum)";
		element elem;
		elem.type = "enum";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);

		element parent_elem = getKnownParentElem(p_Repos);
		p_Repos->graph()->addOrFindVertexIndexByValue(name);
#ifdef _DEBUG_A
		std::cout<<"\n enum stack";
		showStack(p_Repos->scopeStack());
#endif

	}
};

#endif