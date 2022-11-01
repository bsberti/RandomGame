#include <map>
#include <vector>
#include <string>

class cPerson
{
public:
	std::string name;
	unsigned int age;
	float bankBalance;
	unsigned int StudentNumber;	
};

void DoMaps(void)
{
	std::vector< cPerson > vecPeople;

	cPerson Michael;
	Michael.name = "Michael";
	Michael.age = 99999;
	Michael.bankBalance = -1938.0f;
	Michael.StudentNumber = 934846;


	cPerson Dua;
	Dua.name = "Dua";
	Dua.age = 22;
	Dua.bankBalance = 939383.0f;
	Dua.StudentNumber = 12356;

	// Ln(size)
	// 10 --> 2 seconds
	// 100 --> 3 seconds
	// 1000 --> 4 seconds

	vecPeople.push_back(Michael);
	vecPeople.push_back(Dua);		// index 1

	// std::vector< cPerson > vecPeople
	std::vector< cPerson >::iterator itPersonA;

	cPerson I = *itPersonA;

	//itPersonA->

	cPerson A = vecPeople[1];	// get us Dua Lipa 

	//	cPerson A = vecPeople["Dua"];	// OH YEAH!!


	//	std::vector< cPerson > vecPeople;

	std::map< unsigned int, cPerson > mapPeople;
	mapPeople.find(8382);

	mapPeople[12356] = Dua;

	cPerson Z = mapPeople[12356];


	std::map< std::string, cPerson > mapPeopleByName;

	// On a map, this either inserts or overwrites
	mapPeopleByName["Michael"] = Michael;
	mapPeopleByName["Dua Lipa"] = Dua;

	cPerson Mike;
	Mike.name = "Mikey J";
	Mike.age = 27;
	Mike.bankBalance = 4534;
	Mike.StudentNumber = 122345;

	mapPeopleByName["Michael"] = Mike;

	try
	{
		cPerson Y = mapPeopleByName["Frank"];
	}
	catch (...)
	{
		// OH NO!! 
	}

	//	std::map< cPerson*, cPerson > mapPeople;
	//
	//	mapPeople.find( &Dua);

	//	std::map< std::string, cPerson > mapPeopleByName;

	//	cPerson U = 

	std::map< std::string, cPerson >::iterator itPerson
		= mapPeopleByName.find("Dua");

	if (itPerson == mapPeopleByName.end())
	{
		// Didn't find it.
	}
	if (itPerson != mapPeopleByName.end())
	{
		// FOUND IT!
		cPerson DuaLipa = itPerson->second;
	}
	std::string q1 = itPerson->first;
	cPerson DuaLipa = itPerson->second;



	// PHP -> only has maps

	// Name ---> Number
	// Indexed by name.

	// Number --> Name

}