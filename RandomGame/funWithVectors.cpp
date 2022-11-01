#include <vector>
#include <list>
#include <iostream>
#include <string>
#include "cMeshObject.h"
#include <algorithm>

// Sort predicate function signature
bool SortByMeshNameAscending(const cMeshObject& A, const cMeshObject& B);

// find_if predicate function signature
bool FindHelloKitty(const cMeshObject& theObject);


void vectorStuffFunction(void)
{
	// array
	int myArray[10];	// 10 integers
	myArray[0] = 3;
	myArray[5] = 13;
//	myArray[999] = 23;		// YIKES!!!

	std::cout << myArray[5];

//	int myArray1[10];

	std::vector<int> myVector;

	// Allows for familiar "array" type syntax.
	// i.e. it's writen to pretend to be like an array.
//	myVector[0] = 3;		// YIKES!!!
//	myVector[5] = 13;
//	myVector[999] = 23;		
//
//	std::cout << myVector[5];

	// "Pushed" or "added" something on the "back" of the vector
	// Or it's been added to the "end" of the vector
	myVector.push_back(3);	// 1st item, so at index 0, so now: myVector[0] = 3
	myVector.push_back(5423);	// 1st item, so at index 0, so now: myVector[0] = 3
	myVector.push_back(-234);	// 1st item, so at index 0, so now: myVector[0] = 3

	std::cout << myVector[0];		// 3
	std::cout << myVector[1];		// 5423
	std::cout << myVector[2];		// -234

	for ( unsigned int index = 0; index != 3; index++ )
	{
		std::cout << myVector[index] << std::endl;
	}

	for ( unsigned int index = 0; index != myVector.size(); index++ )
	{
		std::cout << myVector[index] << std::endl;
	}


	std::vector<std::string> myVectorOfStrings;
	std::vector<double> myVectorOfDoubles;

	std::vector< std::vector<int> > myVectorOfVectorsOfInts;	//AKA a 2D array

	std::vector<cMeshObject> vecOfMeshObjects;

	cMeshObject myArrayOfMeshes[10];


//	cMeshObject* myArrayOfMeshesPointers[10];
//	std::vector< cMeshObject* > vecOfPointersToMeshObjects;

	return;
}

template <class HEY>
int DoThis(HEY x, HEY y)
{
	return x + y;
}


void UselessFunction(void)
{
	// vector of int
	std::vector< int > myVectorOfInts;

	std::vector< std::string > myVectorOfStrings;

	std::vector< cMeshObject > myObjects;



	for ( unsigned int index = 0; index != myObjects.size(); index++ )
	{
		// Do stuff
		myObjects[index].position.x += 1.0f;
	}

	std::list< cMeshObject > myListOfObjects;
	for ( unsigned int index = 0; index != myListOfObjects.size(); index++ )
	{
		// Do stuff
		//myObjects[index].position.x += 1.0f;
	}


	for (unsigned int index = 0; index != myObjects.size(); index++)
	{
		// Do stuff
		myObjects[index].position.x += 1.0f;
	}


	// std::vector< cMeshObject > myObjects;
	for ( std::vector< cMeshObject >::iterator itObjects = myObjects.begin(); 
		  itObjects != myObjects.end();
		  itObjects++)
	{
		// Do stuff
		cMeshObject* pThisThing;
		pThisThing->position;

		//myObjects[index].position.x += 1.0f;

		itObjects->position.x += 1.0f;
	}


	for (std::vector< cMeshObject >::iterator itObjects = myObjects.begin();
		 itObjects != myObjects.end();
		 itObjects++)
	{
		// 
	}

	// This WILL sort...Or will it??? 
	// 
	// This will ONLY WORK *IF* the thing inside the container 
	//	has an < operator
	//std::sort(myObjects.begin(), myObjects.end());

	std::vector<std::string> myInts;
	std::sort(myInts.begin(), myInts.end());


	// Sort by Meshname Ascending
	std::sort(myObjects.begin(), myObjects.end(), SortByMeshNameAscending);

	// This find will only work if the "==" is set.
	//std::find(myObjects.begin(), myObjects.end(),  )

	std::vector< cMeshObject >::iterator itHelloKitty 
		= std::find_if(myObjects.begin(), myObjects.end(), FindHelloKitty);

	if ( itHelloKitty == myObjects.end() )
	{
		// DIDN'T find it.
	}
	if (itHelloKitty != myObjects.end())
	{
		std::cout << itHelloKitty->friendlyName;
	}

	return;
}

// A "predicate function"
bool SortByMeshNameAscending(const cMeshObject &A, const cMeshObject &B)
{
	if ( A.meshName < B.meshName )
	{
		return true;
	}
	return false;
}

// A "predicate function"
bool FindHelloKitty(const cMeshObject& theObject)
{
	if (theObject.friendlyName == "Hello Kitty")
	{
		return true;
	}
	return false;
}




void Lambda(void)
{

	std::vector< cMeshObject > myObjects;


//	std::vector< cMeshObject >::iterator itHelloKitty
//		= std::find_if(myObjects.begin(), myObjects.end(), 
//					   
//		bool FindHelloKitty(const cMeshObject & theObject)
//		{
//			if (theObject.friendlyName == "Hello Kitty")
//			{
//				return true;
//			}
//			return false;
//		}
//					   
//	);



}