#include "IniParser.h"

using namespace std;

IniParser::IniParser(const string & s_fileName)
{
	values = new map<string, map<string,string>*>();
	parseFile(s_fileName);
} // end Constructor


IniParser::~IniParser(void)
{
	values->clear();
	delete values;
} // end Destructor


string IniParser::operator()(const string& s_group, const string& s_key)
{
	return (*(*values)[s_group])[s_key];
} // end operator()


void IniParser::parseFile(const string& s_file)
{
	ifstream file(s_file.c_str());

	string s_line = "";
	string s_key  = "";

	list<string> ls_left;  // items to the left of =
	list<string> ls_right; // items to the right of =

	bool b_first = true;

	if (file.is_open() && !file.bad())
	{
		while (getline(file, s_line))
		{
			if (s_line.length() > 0 && s_line.at(0) == '[')
			{
				if (!b_first) // prevent adding first key before parsing the whole group
				{
					(*values)[s_key] = new map<string, string>(); // make a new map for this group

					list<string>::const_iterator rightItem = ls_right.begin();

					// add all values from this group to map
					for (list<string>::const_iterator leftItem = ls_left.begin(), end = ls_left.end(); leftItem != end; leftItem++, rightItem++)
					{
						(*values)[s_key]->insert((*values)[s_key]->begin(), pair<string, string>(*leftItem, *rightItem));
					} // end for
				} // end if
				else
				{
					b_first = false; // first group added when we find the next one or finish
				} // end else

				s_key = s_line.substr(1, s_line.length() - 2);
			} // end if
			else if (s_line.length() > 0 &&  s_line != "") // member of a group
			{
				int i = 0;

				while (s_line.at(i) != '=') // find index of = sign
				{
					i++;
				} // end while

				i++; // above puts i right before the = sign

				ls_left.push_back(s_line.substr(0, i-1)); // everything left of = sign
				ls_right.push_back(s_line.substr(i, s_line.length() - i)); // everything right of = sign
			} // end elif
		} // end while
	} // end if 

	// last group was not added to values yet
	(*values)[s_key] = new map<string, string>();

	list<string>::const_iterator rightItem = ls_right.begin();

	for (list<string>::const_iterator leftItem = ls_left.begin(), end = ls_left.end(); leftItem != end; leftItem++, rightItem++)
	{
		(*values)[s_key]->insert((*values)[s_key]->begin(), pair<string, string>(*leftItem, *rightItem));
	} // end for
} // end method parseFile
