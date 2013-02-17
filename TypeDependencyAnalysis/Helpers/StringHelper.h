#ifndef STRINGHELPER_H
#define STRINGHELPER_H
//////////////////////////////////////////////////////////////////////////
//	StringHelper.h - This is a helper class for some string processing	//
//	work.																//
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
This is basically a helper for manipulating string type, including encoding
value into HTML style (< to &lt; , > to &gt;), trim space and new line character
from a string etc., which are not provided by the standard library

Maintanence Information:
========================

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv Graph.sln /rebuild debug

Revision History:
-----------------
- ver 0.1 : initial version.

*/

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace Helper {
	// trim from start
	static inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// trim from both ends
	static inline std::string &trim(std::string &s) {
		return ltrim(rtrim(s));
	}

	// is this a built-in (value) type?
	static inline bool isBuiltinType(const std::string & s) {
		const static std::string keys[] = { "auto", "bool", "char", "int", "float", "double", "void", "wchar_t", "size_t", "string" };
		for(int i=0; i<8; ++i)
			if(s == keys[i])
				return true;
		return false;
	}
	
	// is modifier like const, final, etc.
	static bool isModifierKeyword(const std::string& s)
	{
		const static std::string keys[]
		= { "signed", "unsigned", "const", "volatile", "struct", "enum", "friend", "class", "inline", "virtual", "static", "mutable",
			"thread_local", "auto", "register", "extern", "public", "private", "protected" };
		for(int i=0; i<19; ++i)
			if(s == keys[i])
				return true;
		return false;
	}

	// encode XML string value
	static std::string encodeXml(std::string& data) {
		std::string buffer;
		buffer.reserve(data.length());
		for(size_t pos = 0; pos != data.length(); ++pos) {
			switch(data[pos]) {
			case '&':	buffer.append("&amp;");	break;
			case '\"':	buffer.append("&quot;");	break;
			case '\'':	buffer.append("&apos;");	break;
			case '<':	buffer.append("&lt;");	break;
			case '>':	buffer.append("&gt;");	break;
			case '\n':	buffer.append("&#10;");	break;	// replace new line!!
			case '\r':	buffer.append("&#13;");	break;
			case ' ':	buffer.append("&#32;");	break;
			default:	buffer.append(1,data[pos]);	break;
			}
		}
		return buffer;
	}
	
	//----< add root element and XML declaration to XML string >-----
	static std::string xmlWrapper(const std::string& body, const std::string& root) {
		std::string _xml = "<?xml version=\"1.0\"?>";
		_xml += '<' + root + '>';
		_xml += body;
		_xml += "</" + root + '>';
		return _xml;
	}
}

#endif