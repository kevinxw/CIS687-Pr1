
//////////////////////////////////////////////////////////////////////////
//	StringHelper.cpp - A helper class for some string processing		//
//	work.																//
//																		//
//	ver 0.1																//
//	Language:		Visual C++ 2012, SP1								//
//	Platform:		Dell Studio 1558, Windows 7 Pro x64 Sp1				//
//	Application:	Type Dependency Analysis							//
//	Author:			Kevin Wang, Syracuse University						//
//					kevixw@gmail.com									//
//////////////////////////////////////////////////////////////////////////


#ifdef TEST_STRINGHELPER

#include <string>
#include "StringHelper.h"

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing string helper module\n ";

	std::string trim_test=" trimed!! \n";
	std::cout<<"\n ltrim: "<<Helper::ltrim(trim_test);
	std::cout<<"\n rtrim: "<<Helper::rtrim(trim_test);
	std::cout<<"\n trim: "<<Helper::trim(trim_test);

	std::cout<<"\n\n";

	std::string encode_test="<>& ";
	std::cout<<"\n encode: "<<Helper::encodeXml(encode_test);

	std::cout<<"\n XML wrapper:"<< Helper::xmlWrapper("<body/>","root");
}
#endif