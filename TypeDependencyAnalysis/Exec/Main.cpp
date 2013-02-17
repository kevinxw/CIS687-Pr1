
//////////////////////////////////////////////////////////////////////////
//	Main.cpp - then entry point of this program.						//
//																		//
//	ver 0.1																//
//	Language:		Visual C++ 2012, SP1								//
//	Platform:		Dell Studio 1558, Windows 7 Pro x64 Sp1				//
//	Application:	Type Dependency Analysis, CIS687 Pr#1, 2013 Spring	//
//	Author:			Kevin Wang, Syracuse University						//
//					kevixw@gmail.com									//
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "Tokenizer.h"
#include "SemiExpression.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ActionsAndRulesExtended.h"
#include "ConfigureParser.h"
#include "FoldingRules.h"
#include "Graph.h"
#include "StringHelper.h"
#include "FileSystem.h"

///////////////////////////////////////////////////////////////
// parse file, push the result into graph

void parseFile(GraphLib::Graph<std::string, std::string>* pGraph, const std::string & path) {
	std::cout << "\n  Processing file " << path;

	ConfigParseToConsole configure(pGraph);
	Parser* pParser = configure.Build();
	try {
		if(pParser) {
			if(!configure.Attach(path)) {
				std::cout << "\n  could not open file " << path << std::endl;
				return;
			}
		}
		else {
			std::cout << "\n\n  Parser not built\n\n";
			return;
		}
		// now that parser is built, use it
		while(pParser->next())
			pParser->parse();
	}
	catch(std::exception& ex)
	{
		std::cout << "\n\n    " << ex.what() << "\n\n";
	}
}

///////////////////////////////////////////////////////////////
// save a string to a file

void saveToFile(const std::string& outpath, const std::string& str) {
	std::ofstream outf(outpath);
	if(outf.good()) {
		outf<< str;
		std::cout << "\n\nFile saved to:\n"<<outpath<<"\n\n";
	}
	outf.close();
}

///////////////////////////////////////////////////////////////
// get string from a file

std::string getFromFile(const std::string& inpath) {
	std::ifstream inf(inpath);
	std::string str;
	while (inf.good()) {
		std::string buff;
		inf>>buff;
		str+= " "+buff;
	}
	inf.close();
	return str;
}

///////////////////////////////////////////////////////////////
// get a list of file path under one specific directory

std::vector<std::string> getFileList(std::string path, bool isRecursive=true, std::string pattern="*.h") {
	if (path[path.length()-1]!='/' && path[path.length()-1]!='\\')
		path += "/";
	std::vector<std::string> files = FileSystem::Directory::getFiles(path, pattern);
	for (size_t i=0;i<files.size();i++)
		files[i]=path+files[i];
	if (isRecursive) {
		// get files from sub-directories
		std::vector<std::string> subDirectories = FileSystem::Directory::getDirectories(path);
		for (size_t i=0;i<subDirectories.size();i++) {
			if (subDirectories[i]=="." || subDirectories[i]=="..")
				continue;
			else {
				std::vector<std::string> _files = getFileList(path+subDirectories[i], isRecursive, pattern);
				files.insert(files.end(), _files.begin(), _files.end());	// copy the files from sub-directories to current file list
			}
		}
	}
	return files;
}

///////////////////////////////////////////////////////////////
// main entry point of the program

int main(int argc, char* argv[])
{
	std::cout << "\nType dependency analysis starts! - by Kevin Wang"<< std::string(10,' ')<<"\nfor Spring 2013, Pr#1"<< std::endl;
	std::string outpath("./output.xml"),		// default output path
		_xml;
	bool isRecursive = false;
	if (argc>1)
		isRecursive=(std::string(argv[1])=="\\R");
	// see if command arguments are valid
	if(argc < 2 || (isRecursive && argc<3)) {
		std::cout<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}
	std::vector<std::string> files;
	std::string path = isRecursive ? argv[2] : argv[1];
	// the command line should be :  [option] path [file pattern] [file_pattern]
	if (argc < (isRecursive ? 4 : 3)) {
		// if there is no file pattern
		files = getFileList(path, isRecursive);
	}
	else {
		for(int i=isRecursive ? 3 : 2; i<argc; ++i) {
			std::vector<std::string> _files = getFileList(path, isRecursive, argv[i]);
			files.insert(files.end(), _files.begin(), _files.end());
		}
	}
	std::cout<<"\n Now start analyzing types.. Please be patient..\n";
	// create graph instance
	GraphLib::Graph<std::string, std::string>* pGraph = new GraphLib::Graph<std::string, std::string>();
	for (size_t i=0;i<files.size();i++)
		parseFile(pGraph, files[i]);

	if (files.size()<1) {
		std::cout<<"\n No file is found under the specific directory!!";
		return 1;
	}
	// clean the undeclared types (includes user-defined types only)
	pGraph->clean();
	
	std::cout<<"\n Vertex map (using functor)\n";
	// a simple test for functor
	GraphLib::showVertex<std::string, std::string> showV;
	pGraph->dfs((*pGraph)[0],showV);
	// save xml to file
	saveToFile(outpath, pGraph->xml());

	std::cout<<"\n\n Done!! All relationship has been analyzed";
	return 0;
}