#pragma once


#include <fstream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <map>
#include <list>


class IniParser
{
public:
	// Constructors:
	/// <summary>Creates an empty parser object in invalid state.</summary>
	IniParser(void) { /* do nothing Constructor */ }

	/// <summary>Constructor that will parse the file given in <paramref name="s_fileName"/>.</summary>
	/// <param name="s_fileName">The path of the file to parse.</param>
	IniParser(const std::string& s_fileName) { parse(s_fileName); }

	// Destructor:
	/// <summary>Releases all dynamic memory owned by this object.</summary>
	~IniParser(void) { clear(); }

	// Public Methods:	
	// Mutators:
	/// <summary>Parses the file <paramref name="s_fileName"/> and adds all groups and their keys to the parser.</summary>
	/// <param name="s_fileName">The path of the file to parse.</param>
	void parse(const std::string& s_fileName);

	/// <summary>Adds the key <paramref name="s_key"/> with the value <paramref name="value"/> to the group <paramref name="s_group"/>.</summary>
	/// <typeparam name="T">The type of the value to be added.</typeparam>
	/// <param name="s_group">The group to which the key should be added.</param>
	/// <param name="s_key">The key to add.</param>
	/// <param name="value">The value that should be associated with <paramref name="s_key"/>.</param>
	/// <returns>True on success, false otherwise.</returns>
	template <typename T>
	inline bool addKeyPair(const std::string& s_group, const std::string& s_key, const T value)
	{
		std::stringstream stream;

		stream << value;

		KeyPair pair(s_key, stream.str());

		addKey(s_group, pair);

		return contains(s_group, s_key);
	} // end template addKey

	// Accessors:
	/// <summary>Accessor for the key <paramref name="s_key"/> at in the group <paramref name="s_group"/>.</summary>
	/// <typeparam name="T">The type that the key value should be converted to.</typeparam>
	/// <param name="s_group">The group containing the key.</param>
	/// <param name="s_key">The key to retrieve.</param>
	/// <returns>The value of the specified key converted to type T.</returns>
	/// <exception name="std::invalid_argument">Thrown if <paramref name="s_group"/> or <paramref name="s_key"/> do not exist.</exception>
	template <typename T>
	T getKeyAs(const std::string& s_group, const std::string& s_key) // retrieves the key in group if it exists
	{
		std::stringstream stream;
		std::string trash;
		T t;

		if (!isValid() || !contains(s_group))
		{
			std::stringstream error;

			error << "The group '" << s_group << "' does not exists!";

			throw std::invalid_argument(error.str());
		} // end if
		if (!contains(s_group, s_key))
		{
			std::stringstream error;

			error << "The group '" << s_group << "' does not contain the key '" << s_key << "'!";

			throw std::invalid_argument(error.str());
		} // end elif

		stream << (*(*values)[s_group])[s_key];

		if (!(stream >> t) || (stream >> trash)) // check if conversion is successful, and check if there is anything left in the stream afterwards
		{										 // if there is anything left in the stream, conversion must've failed
			std::stringstream error;

			error << "IniParser could not convert the value of key '" << s_key << "' in group '" << s_group << "' to the specified type.\nValue is: " 
				  << stream.str() << trash << std::endl;

			throw std::invalid_argument(error.str());
		} // end if

		return t;
	} // end template getKeyAs

	/// <summary>Deletes all groups and keys stored in the parser object.</summary>
	inline void clear(void)
	{
		if (isValid())
		{
			values->clear(); // destroy inner maps
			delete values;
		} // end if
	} // end method clear

	/// <summary>Checks if there are any groups in the parser.</summary>
	/// <returns>True if the parser contains at least 1 group, false otherwise.</returns>
	inline bool empty(void);

	/// <summary>Checks if the parser is currently in a valid state.</summary>
	/// <returns>True if the parser is valid, false otherwise.</returns>
	inline bool isValid(void)
	{
		return values != nullptr;
	} // end method isValid

	/// <summary>Accessor for the number of groups in the parser.</summary>
	/// <returns>The number of groups in the parser.</returns>
	inline size_t size(void);

	/// <summary>Accessor for the number of keys in the specified group.</summary>
	/// <param name="s_group">The group containing the key.</param>
	/// <returns>The number of keys in the specified group, if the group exists.</returns>
	inline size_t groupSize(const std::string& s_group);

	/// <summary>Checks if the given group exists.</summary>
	/// <param name="s_group">The group to check.</param>
	/// <returns>True if the group exists, false otherwise.</returns>
	inline bool contains(const std::string& s_group)
	{
		if (isValid())
		{
			return values->count(s_group) != 0;
		} // end if

		return false;
	} // end method contains

	/// <summary>Checks if the given group contains the specified key.</summary>
	/// <param name="s_group">The group containing the key.</param>
	/// <param name="s_key">The key to check.</param>
	/// <returns>True if the group contains the specified key, false otherwise.</returns>
	inline bool contains(const std::string& s_group, const std::string& s_key)
	{
		if (isValid())
		{
			return (*values)[s_group]->count(s_key) != 0;
		} // end if

		return false;
	} // end method contains
private:
	struct KeyPair
	{
		std::string key;
		std::string value;
	}; // end struct KeyPair

	// Class Fields:
	/// <summary>Data structure containing the parsed keys.</summary>
	std::map<std::string, std::map<std::string, std::string>*>* values; // group => map<key,value> e.g.: { (Configuration, {(IP,x.x.x.x), (port, 1234), ...} ), (Misc, {(n, 4), (k, 19), ...} ), ...}

	// Private Methods:
	/// <summary>Parses the file <paramref name="s_fileName"/> into the parser object.</summary>
	/// <param name="s_fileName">The path of the file to parse.</param>
	void parseFile(const std::string& s_fileName); // parses the given *.ini file into values

	/// <summary>Instantiates the values data structure.</summary>
	inline void instantiate(void);

	/// <summary>Adds the key-value pairs <paramref name="pairs"/> to the group <paramref name="s_group"/>.</summary>
	/// <param name="s_group">The group to which the key should be added.</param>
	/// <param name="pairs">The <see cref="KeyPair"/>s to add.</param>
	void addKey(const std::string s_group, std::list<KeyPair>& pairs);

	/// <summary>Adds the key-value pair <paramref name="keyPair"/> to the group <paramref name="s_group"/>.</summary>
	/// <param name="s_group">The group to which the key should be added.</param>
	/// <param name="keyPair">The <see cref="KeyPair"/>s to add.</param>
	void addKey(const std::string s_group, KeyPair& keyPair);

	/// <summary>Adds the the group <paramref name="s_group"/> to the parser.</summary>
	/// <param name="s_group">The group to add.</param>
	void addGroup(const std::string s_group);

	/// <summary>Extracts a group name from the string <paramref name="s_line"/> if possible.</summary>
	/// <param name="s_line">The string to parse for a group name.</param>
	/// <param name="s_group">Output parameter for group name extracted from string.</param>
	/// <returns>True if the string contained a group name, otherwise false. Upon success, the value of <paramref name="s_group"/>
	///			 will be changed to the name that was extracted; otherwise it will remain unchanged.</returns>
	bool extractGroupName(const std::string& s_line, std::string& s_group);

	/// <summary>Extracts a key pair from the string <paramref name="s_line"/> if possible.</summary>
	/// <param name="s_line">The string to parse for a key pair name.</param>
	/// <param name="keyPair">Output parameter for the keypair extracted from string.</param>
	/// <returns>True if the string contained a key pair, otherwise false. Upon success, the value of <paramref name="keyPair"/>
	///			 will be changed to the key pair that was extracted; otherwise it will remain unchanged.</returns>
	bool extractKeyPair(const std::string& s_line, KeyPair& keyPair);
}; // end Class IniParser