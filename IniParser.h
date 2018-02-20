#pragma once


#include <fstream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <map>
#include <list>

using namespace std;

class IniParser
{
public:
	// Constructor:
	IniParser(const string&); // constructor taking the path to a *.ini file

	// Destructor:
	~IniParser(void);		  // destructor

	// Public Methods:
	string operator()(const string& group, const string& key); // retrieves the key in group if it exists

private:
	// Class Fields:
	map<string, map<string, string>*>* values; // group => map<key,value> e.g.: { (Configuration, {(IP,x.x.x.x), (port, 1234), ...} ), (Misc, {(n, 4), (k, 19), ...} ), ...}

	// Private Methods:
	void parseFile(const string&); // parses the given *.ini file into values
};