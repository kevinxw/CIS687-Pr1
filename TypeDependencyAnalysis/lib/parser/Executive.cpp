/////////////////////////////////////////////////////////////////////
// Executive.cpp - Demonstrate Operation of Parser                 //
//                                                                 //
//  Language:      Visual C++ 2012, update 1                       //
//  Platform:      Dell XPS 8300, Windows 7                        //
//  Application:   Prototype Helper for CSE687 Pr1, Sp13           //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
  Package Operations: 
  ===================
  This package simply runs a simple set of analysis rules and actions
  to demonstrate how the parser works.

  You should look at the packages:
    ActionsAndRules and ConfigureParser, as these are application 
    specific.

  Build Process:
  ==============
  Required files
    - Executive, Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp,
      ActionsAndRules.h, ActionsAndRules.cpp, 
      ConfigureParser.h, ConfigureParser.cpp,
      ItokCollection.h, SemiExpression.h, SemiExpression.cpp, 
      tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv Parser.sln
    - cl /EHsc Executive.cpp parser.cpp ActionsAndRules.cpp \
         ConfigureParser.cpp FoldingRules.cpp ScopeStack.cpp
         semiexpression.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.0 : 21 Jan 13
  - first release

*/


#include <string>
#include <iostream>
#include "Tokenizer.h"
#include "SemiExpression.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"
#include "FoldingRules.h"

int main(int argc, char* argv[])
{
  std::cout << "\n  Demonstrating Parser\n "
            << std::string(22,'=') << std::endl;

  // collecting tokens from files, named on the command line

  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }
  // build all the parser's parts
  ConfigParseToConsole configure;
  Parser* pParser = configure.Build();

  // do for each file cited on the command line
  for(int i=1; i<argc; ++i)
  {
    std::cout << "\n  Processing file " << argv[i];
    std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');

    // Now parse each file
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
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
    std::cout << "\n\n";
  }
}

