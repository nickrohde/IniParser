#include "IniParser.hpp"

using namespace std;

void IniParser::parse(const std::string & s_fileName)
{
	instantiate();
	parseFile(s_fileName);
} // end method parse


inline bool IniParser::empty(void)
{
	if (isValid())
	{
		return values->empty();
	} // end if

	return true;
} // end method empty


inline size_t IniParser::size(void)
{
	if (isValid())
	{
		return values->size();
	} // end if

	return 0;
} // end method size


inline size_t IniParser::groupSize(const std::string & s_group)
{
	if (isValid())
	{
		if (contains(s_group))
		{
			return (*values)[s_group]->size();
		} // end if
	} // end if

	return 0;
} // end method groupSize


void IniParser::parseFile(const string& s_file)
{
	ifstream file(s_file.c_str());

	string	s_line = "",
			s_groupName  = "",
			s_newGroup = "" ;

	list<KeyPair> keyPairs;

	bool b_isKeyPair = false,
		 b_isGroup = false;

	KeyPair currentPair;


	if (file.is_open() && !file.bad())
	{
		while (getline(file, s_line))
		{
			if (!s_line.empty())
			{
				b_isGroup = extractGroupName(s_line, s_newGroup);
				
				if (b_isGroup && !keyPairs.empty())
				{
					addKey(s_groupName, keyPairs);
					keyPairs.clear();
				} // end else

				if (!b_isGroup)
				{
					b_isKeyPair = extractKeyPair(s_line, currentPair);

					if (b_isKeyPair)
					{
						keyPairs.push_front(currentPair);
					} // end if (b_keyPair)
				} // end if (!b_group)
				else
				{
					s_groupName = s_newGroup;
				} // end else				
			} // end if
		} // end while

		if (!keyPairs.empty()) // add last key if it has anything in it
		{
			addKey(s_groupName, keyPairs);
			keyPairs.clear();
		} // end if
	} // end if 

	file.close();
} // end method parseFile


inline void IniParser::instantiate(void)
{
	if (!isValid())
	{
		values = new map<string, map<string, string>*>();

		if (values == nullptr)
		{
			throw runtime_error("IniParser could not instantiate memory.");
		} // end else
	} // end if
} // end method instantiate


void IniParser::addKey(const std::string s_group, std::list<KeyPair>& pairs)
{
	addGroup(s_group);

	// add all values from this group to map
	for (auto& keyPair : pairs)
	{
		addKey(s_group, keyPair);
	} // end for	

} // end method addKey


void IniParser::addKey(const std::string s_group, KeyPair& keyPair)
{
	instantiate();

	(*values)[s_group]->insert((*values)[s_group]->begin(), pair<string, string>(keyPair.key, keyPair.value));
} // end method addKey


void IniParser::addGroup(const std::string s_group)
{
	instantiate();

	if (!contains(s_group))
	{
		(*values)[s_group] = new map<string, string>(); // make a new map for this group
	} // end if
} // end method addGroup


bool IniParser::extractGroupName(const std::string& s_line, std::string& s_group)
{
	size_t	ui_start = s_line.find("["),   // start of group name
			ui_end = s_line.find("]"),	   // end of group name
			ui_comment = s_line.find(";"); // check if the line has a comment on it

	bool b_out = false;

	if (ui_comment != 0) // check if the whole line is a comment
	{
		if ((ui_start != std::string::npos)) // check if '[' is in the string
		{
			if (ui_end == std::string::npos) // check if ']' is in the string
			{
				stringstream ss;
				
				ss << "The group name '" << s_line << "' is invalid. No closing bracket ']' found!";

				throw invalid_argument(ss.str());
			} // end if
			else
			{
				s_group = s_line.substr(ui_start + 1, ui_end - 1);
				b_out = true;
			} // end else
		} // end if
	} // end if

	return b_out;
} // end method extractGroupName                                                                      


bool IniParser::extractKeyPair(const std::string& s_line, KeyPair& keyPair)
{
	bool b_out = false;

	size_t	ui_equalSign = s_line.find("="),	// start of group name
			ui_comment = s_line.find(";");		// comment index
			

	if (ui_comment != 0 && ui_equalSign != std::string::npos) // check if the whole line is a comment and ensure there is an equal sign
	{
		size_t	ui_startKey = 0,
				ui_endKey = ui_equalSign - 1,
				ui_startValue = ui_equalSign + 1,
				ui_endValue = ui_startValue;

		// find the first character in the string
		while (ui_startKey < s_line.length() && !isalpha(s_line.at(ui_startKey)))
		{
			ui_startKey++;
		} // end while

		// find the first character of the value
		while (ui_startValue < s_line.length() && !isalnum(s_line.at(ui_startValue)))
		{
			ui_startValue++;
		} // end while

		while (ui_endKey > 0 && !isalnum(s_line.at(ui_endKey)))
		{
			ui_endKey--;
		} // end while

		ui_endValue = ui_startValue;

		// find the end of the value component
		while (ui_endValue < s_line.length() && s_line.at(ui_endValue) != ';')
		{
			ui_endValue++;
		} // end while

		// ensure indices are valid
		if (ui_startKey < ui_endKey && ui_startValue < ui_endValue && ui_endKey < ui_endValue)
		{
			keyPair.key = s_line.substr(ui_startKey, (ui_endKey - ui_startKey) + 1);
			keyPair.value = s_line.substr(ui_startValue, (ui_endValue - ui_startValue));

			b_out = true;
		} // end if (ui_startKey < ui_endKey && ui_startValue < ui_endValue)
	} // end if (ui_comment != 0 && ui_equalSign != std::string::npos)

	return b_out;
} // end method extractKeyPair																							                 
