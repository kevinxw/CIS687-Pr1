///////////////////////////////////////////////////////////////
// This file is a designed test file.  It may not pass the compile. 
// I try to include every possible situation here, to test the correctness parser
// The codes are formatted in strange way on purpose
// - Kevin Wang  2013 Spring

#ifndef EXTENDEDACTIONSANDRULES_H
#define EXTENDEDACTIONSANDRULES_H

#include "ActionsAndRules.h"

namespace Namespace1 {
	struct Struct0 : InheritanceStruct0 {};struct Struct1 : InheritanceStruct2 {std::string Struct1Field1;testType1 Struct1_Field2;};

	struct Struct2 : InheritanceStruct2 {std::string type2;/*describe which type it is, class/struct/enum?*/Struct2Field2 test;
	} structVar2;
}

struct Struct3 {std::string type2;/*describe which type it is, class/struct/enum?*/} structVar3;

struct Struct4 {public:private:Struct4Field1 ttt;};

struct Struct5 {public:Struct5Field1 tt;private:};

enum {
	MONDAY, THUSDAY, WEDNESDAY
} AnonymousEnumVar1;

enum Enum1 {MONDAY, THUSDAY, WEDNESDAY};

enum Enum2 {
	MONDAY, THUSDAY, WEDNESDAY
} AnonymousEnumVar3;

enum std2::WEEKDAY {MONDAY=3, THUSDAY, WEDNESDAY};

typedef TypeDef1 typeDefined1;
typedef TypeDef2<std::string, std::string> typeDefined2;
typedef void(*FunctionPtrTypedef) ();

template<class V, typename E>	// templated function
void showVert(Vertex<V,E>& v){std::cout << "\n  " << v.id();}

///////////////////////////////////////////////////////////////
// rule to detect type definitions, including "class" & "struct"
class SimicolonDetector1 {public:private:};
class SimicolonDetector2 {public:private:;};
class SimicolonDetector3 {public:;private:};
class SimicolonDetector4 {public:;private:;};

class {public:;private:;} AnonymousClass1;

template<typename V, class E>
class std::TemplatedClass : public TemplatedClassInheritance1<V,E>, private TemplatedClassInheritance2<a,e*>, public TemplatedClassInheritance3
{
private:TemplatedClassField1& AAA;
		TemplatedClassField2* BBB;
		TemplatedClassField3 CCC;
		class NestedClass1 {
			public:
				NestedClassField1 xxx;
		};
public:
	typename typedef std::vector< Vertex<V,E*>>::iterator iterator;
	bool doTest(ITokCollection<V,E>
	*
	
	& pTc)
	{
		const static std::string keys[] = { "class", "struct" };
		long int LongIntVar;
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{")
		{
			for (LoopVariable1 i=0;i<2;i++)	// for loop test
				;	// do something
			for (LoopVariable2 x=22;;) {}	// do something
			for (auto & v:vector);
			for (LoopVariable3 xx:ss){
				a=b;	// do something
			}
			try {
			}
			catch (CatchExeception1 e)
				;
			try {} catch (CatchException2 e2){}

			try	// sample form cplusplus.com
			{
				int * myarray= new int[1000];
			}
			catch (bad_alloc &)
			{
				cout << "Error allocating memory." << endl;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push function name onto ScopeStack

class std::PushType : public std::IAction{
	Repository* p_Repos;
public:
	PushType(Repository* pRepos):cc(2)
	{
		p_Repos = pRepos;
	}
	inline void doAction(const ITokCollection*& pTc)
	{
		const static std::string keys[] = { "class", "struct" };
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push type scope
		std::string name;
		for (int i=0,len;i<2;i++)
			if ((len = pTc->find(keys[i]))<pTc->call(new JustNewClass0))
				name = (*pTc)[len +1];
		element elem;
		elem.type = "type";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		std::cout<<"\n "<<elem.show();
		p_Repos->scopeStack().push(elem);
	}
};

///////////////////////////////////////////////////////////////
// action to send semi-expression that starts a function def
// to console

class HandleType : public Inheritance1<TEST,s&*>{	// no space between >{
	Aggregation* p_Repos;
	Composition p_Repos;
public:
	HandleType(Using0* pRepos) : aa(2),
		bb(22){
			p_Repos = pRepos;
	}
	~HandleType() {}	// deconstructor
	void doAction(Using*& pTc, 
		Using2<E*> & aa,*Using3 & aa)
	{
		if(p_Repos->scopeStack().size() == 0)return;
		element elem = p_Repos->scopeStack().pop();
		if(elem.type == "type")
		{
			//std::cout << "\nHandleType";
			//std::cout << "\n--popping at line count = " << p_Repos->lineCount();
			std::cout << "\n  Type " << elem.name << ", lines = " << p_Repos->lineCount() - elem.lineCount + 1;
			while (WhileTest1 test=1){};
			while (WhileTest2 test=3);
			do { }
			while (DoWhileTest1 test!=2);
			ArrayType0 type[]= { "ss" };
			ArrayType1 type[1];
			
			FancyClass clas(ss);
			
			new JustNewClassTest1(someVar);
			new JustNewClassTest2;
			((new JustNewClassTest3(
			)	// test for new line
			)
			);
			(new JustNewClassTest4(new JustNewClassTest5));
		}
	}
};

namespace Namespace2 {
	template <>
	class SpecializationClass <bool>
	{
	private:
		unsigned int *vector_data;int length;
		int size;ArrayType2 type[10];
	};

	template <  >
	class SpecializationClass2 <bool>{};

	template <  >
	class SpecializationClass2 <bool>{
		SpecializationClass2Field test;
	};
}

template <  >	// same class name, different namespace, again
class SpecializationClass2 <bool>{};

#endif