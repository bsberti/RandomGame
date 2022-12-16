#include "cLuaBrain.h"

#include <iostream>

#include "cMeshObject.h"

cLuaBrain::cLuaBrain() {
	this->m_p_vecGOs = nullptr;

	// Create new Lua state.
	// NOTE: this is common to ALL script in this case
	this->m_pLuaState = luaL_newstate();

	luaL_openlibs(this->m_pLuaState);	/* Lua 5.3.3 */

	// this is where you tell Lua what function to call when the script name is called
	// Here, we set "setObjectState" to call the Lua_UpdateObject() function or static method
	lua_pushcfunction( this->m_pLuaState, cLuaBrain::Lua_UpdateObject );
	lua_setglobal( this->m_pLuaState, "setObjectState" );

	// Same deal, but "getObjectState" calls Lua_GetObjectState();
	lua_pushcfunction( this->m_pLuaState, cLuaBrain::Lua_GetObjectState );
	lua_setglobal( this->m_pLuaState, "getObjectState" );

	lua_pushcfunction( this->m_pLuaState, cLuaBrain::Lua_UpdateObjectName );
	lua_setglobal(this->m_pLuaState, "updateObjectName");

	return;
}

cLuaBrain::~cLuaBrain() {
	lua_close(this->m_pLuaState);
	return;
}

// Saves (and overwrites) any script
// scriptName is just so we can delete them later
void cLuaBrain::LoadScript( std::string scriptName, 
					        std::string scriptSource ) {
	this->m_mapScripts[scriptName] = scriptSource;
	return;
}

void cLuaBrain::DeleteScript( std::string scriptName) {
	this->m_mapScripts.erase(scriptName);
	return;
}

// Passes a pointer to the game object vector
void cLuaBrain::SetObjectVector(std::map< std::string, cMeshObject* >* p_vecGOs) {
	this->m_p_vecGOs = p_vecGOs;
	return;
}

// Call all the active scripts that are loaded
void cLuaBrain::Update(float deltaTime) {
	//std::cout << "cLuaBrain::Update() called" << std::endl;
	for( std::map< std::string /*name*/, std::string /*source*/>::iterator itScript = 
		 this->m_mapScripts.begin(); itScript != this->m_mapScripts.end(); itScript++ )
	{
		// Pass the script into Lua and update
		//int error = luaL_loadbuffer(L, buffer, strlen(buffer), "line");

		std::string curLuaScript = itScript->second;

		int error = luaL_loadstring( this->m_pLuaState, 
									 curLuaScript.c_str() );

		if ( error != 0 /*no error*/)	
		{
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			continue;
		}

		// execute funtion in "protected mode", where problems are 
		//  caught and placed on the stack for investigation
		error = lua_pcall(this->m_pLuaState,	/* lua state */
						  0,	/* nargs: number of arguments pushed onto the lua stack */
						  0,	/* nresults: number of results that should be on stack at end*/
						  0);	/* errfunc: location, in stack, of error function. 
								   if 0, results are on top of stack. */
		if ( error != 0 /*no error*/)	
		{
			std::cout << "Lua: There was an error..." << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

			std::string luaError;
			// Get error information from top of stack (-1 is top)
			luaError.append( lua_tostring(this->m_pLuaState, -1) );

			// Make error message a little more clear
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << luaError << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			// We passed zero (0) as errfunc, so error is on stack)
			lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */

			continue;
		}

	}

	return;
}

// Runs a script, but doesn't save it (originally used to set the ObjectID)
void cLuaBrain::RunScriptImmediately(std::string script) {
	int error = luaL_loadstring( this->m_pLuaState, 
								 script.c_str() );

	if ( error != 0 /*no error*/) {
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		return;
	}

	// execute funtion in "protected mode", where problems are 
	//  caught and placed on the stack for investigation
	error = lua_pcall(this->m_pLuaState,	/* lua state */
						0,	/* nargs: number of arguments pushed onto the lua stack */
						0,	/* nresults: number of results that should be on stack at end*/
						0);	/* errfunc: location, in stack, of error function. 
								if 0, results are on top of stack. */\

	if ( error != 0 /*no error*/) {
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

		std::string luaError;
		// Get error information from top of stack (-1 is top)
		luaError.append( lua_tostring(this->m_pLuaState, -1) );

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack)
		lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */
	}

	return;
}

// Called by Lua
// Passes object ID, new velocity, etc.
// Returns valid (true or false)
// Passes: 
// - position (xyz)
// - velocity (xyz)
// called "setObjectState" in lua
/*static*/ int cLuaBrain::Lua_UpdateObject( lua_State *L ) {
	int objectID = (int)lua_tonumber(L, 1);	/* get argument */
	
	// Exist? 
	cMeshObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if ( pGO == nullptr ) {	// No, it's invalid
		lua_pushboolean( L,  false );
		// I pushed 1 thing on stack, so return 1;
		return 1;	
	}

	// Object ID is valid
	//printf("pGO->m_position.x = %d \n", pGO->m_position.x);

	pGO->position.x = (float)lua_tonumber(L, 2);	/* get argument */
	pGO->position.y = (float)lua_tonumber(L, 3);	/* get argument */
	pGO->position.z = (float)lua_tonumber(L, 4);	/* get argument */
	float vx = (float)lua_tonumber(L, 5);	/* get argument */
	float vy = (float)lua_tonumber(L, 6);	/* get argument */
	float vz = (float)lua_tonumber(L, 7);	/* get argument */
	/*glm::vec3 addToRotation = glm::vec3(
		(float)lua_tonumber(L, 8), 
		(float)lua_tonumber(L, 9), 
		(float)lua_tonumber(L, 10));
	pGO->setRotationFromEuler(addToRotation);*/
	// We want set or add here?
	pGO->moving = lua_tonumber(L, 8);

	lua_pushboolean( L, true );	// index is OK
	
	return 1; // There were 8 things on the stack
}

// Passes object ID
// Returns valid (true or false)
// - position (xyz)
// - velocity (xyz)
// called "getObjectState" in lua
/*static*/ int cLuaBrain::Lua_GetObjectState( lua_State *L )
{
	int objectID = (int)lua_tonumber(L, 1);	/* get argument */
	
	// Exist? 
	cMeshObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if ( pGO == nullptr )
	{	// No, it's invalid
		lua_pushboolean( L,  false );
//		lua_pushstring(L, "Didn't work. So sorry");
		// I pushed 1 thing on stack, so return 1;
		return 1;	
	}

	// Object ID is valid

	lua_pushboolean( L, true );	// index is OK		// 1st item on stack
	lua_pushnumber( L, pGO->position.x );			// 2nd item on stack
	lua_pushnumber( L, pGO->position.y );			// 3rd item
	lua_pushnumber( L, pGO->position.z );			// and so on
	lua_pushnumber( L, 0 );
	lua_pushnumber( L, 0 );
	lua_pushnumber( L, 0 );
	//lua_pushnumber( L, pGO->rotation.x);
	//lua_pushnumber( L, pGO->rotation.y);
	//lua_pushnumber( L, pGO->rotation.z);
	lua_pushnumber( L, pGO->moving);
	
	return 8;		// There were 7 things on the stack
}

/*static*/ 
std::map< std::string, cMeshObject* >* cLuaBrain::m_p_vecGOs;


// returns nullptr if not found
/*static*/ cMeshObject* cLuaBrain::m_findObjectByID( int ID ) {
	for (std::map< std::string, cMeshObject* >::iterator itGO = cLuaBrain::m_p_vecGOs->begin();
		  itGO != cLuaBrain::m_p_vecGOs->end(); itGO++ )
	{
		if ( (*itGO->second).getID() == ID )
		{	// Found it!
			return itGO->second;
		}
	}//for ( std::vector<cGameObject*>::iterator itGO...
	// Didn't find it
	return nullptr;
}


std::string cLuaBrain::m_decodeLuaErrorToString( int error ) {
	switch ( error ) {
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error"; 
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	} //switch ( error )

	// Who knows what this error is?
	return "Lua: UNKNOWN error";
}

//static 
int cLuaBrain::Lua_UpdateObjectName(lua_State* L) {
	std::cout << "UpdateObjectName() was called from calling updateObjectName() in Lua" << std::endl;

	int objectID = (int)lua_tonumber(L, 1);	/* get argument */

	// Exist? 
	cMeshObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	// Get the 2nd thing, which is a string.
	// Note that Lua has char arrays, not std::strings
	// Returns a POINTER to a string array
	// Warning: DON'T KEEP THIS AROUND
	char* newName = (char*)lua_tostring(L, 2);

	std::string strNewName;	
	strNewName.append(newName);		// Copies the string array to a nice string

	pGO->meshName = strNewName;

	// Nothing is on the stack, so return 0
	return 0;
}
