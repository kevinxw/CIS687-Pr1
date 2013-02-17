/////////////////////////////////////////////////////////////////////
//  XmlParserPrototype.cpp - Some parts for an XML Parser          //
//  ver 1.2                                                        //
//                                                                 //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module shows how to begin building an XML parser.  It uses
  the services of the Tokenizer and XmlElementParts modules to carry
  out all its currently defined activities.  Note that this is just
  a vehicle to get you started.

  The file scope walker, demonstrated in test stub of SemiExpression.cpp, 
  shows you how to do a recursive descent parsing that you need to 
  build the XML document tree, required for this project.

  Build Process:
  ==============
  Required files
    - XmlParserPrototype, XmlElementParts.h, XmlElementParts.cpp,
      Tokenizer.h, Tokenizer.cpp
  Build commands (either one)
    - devenv Project1HelpS09.sln
    - cl /EHsc XmlParserPrototype.cpp XmlElementParts.cpp Tokenizer.cpp \
         /link setargv.obj

  Maintenance History:
  ====================
  ver 1.0 : 18 Jan 09
  - first release

*/

#include "XmlElementParts.h"

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing XmlParserPrototype\n "
            << std::string(28,'=') << std::endl;

  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }

  for(int i=1; i<argc; ++i)
  {
    std::cout << "\n  Processing file " << argv[i];
    std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-') << "\n\n";
    try
    {
      Toker toker(argv[i]);
      toker.setMode(Toker::xml);
      XmlParts parts(&toker);
      while(parts.get())
        std::cout << parts.show().c_str() << std::endl;
      std::cout << "\n\n";
    }
    catch(std::exception ex)
    {
      std::cout << "\n  " << ex.what() << "\n\n";
    }
  }
}

