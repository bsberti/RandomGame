#ifndef _cLuaBrain_HG_
#define _cLuaBrain_HG_

extern "C" {
#include <Lua5.4.4/lua.h>
#include <Lua5.4.4/lualib.h>
#include <Lua5.4.4/lauxlib.h>
}


#include <string>
#include <vector>
//#include "cGameObject.h"
#include <map>

// Forward declaration for cyclical reference
class cMeshObject;

class cLuaBrain
{
public:
	// Init Lua and set callback functions
	cLuaBrain();
	~cLuaBrain();
	void LoadScript( std::string scriptName, 
					 std::string scriptSource );
	void DeleteScript( std::string scriptName );
	// Passes a pointer to the game object vector
	void SetObjectVector(std::map< std::string, cMeshObject* >* p_vecGOs );
	// Call all the active scripts that are loaded
	void Update(float deltaTime);

	// Runs a script, but doesn't save it (originally used to set the ObjectID)
	void RunScriptImmediately(std::string script);
	// Called by Lua
	// Passes object ID, new velocity, etc.
	// Returns valid (true or false)
	// Passes: 
	// - position (xyz)
	// - velocity (xyz)
	// called "setObjectState" in lua
	// Something like this: "setObjectState( 24, 0, 0, 0, 0, 0, 0 )"
	static int Lua_UpdateObject( lua_State *L );
	// Passes object ID
	// Returns valid (true or false)
	// - position (xyz)
	// - velocity (xyz)
	// called "getObjectState" in lua
	static int Lua_GetObjectState( lua_State *L );
//	"lauchDropShip()"
//	static int Lua_DropShipLaunch(lua_State* L);

	// Update Object's name
	// Lua script name is "updateObjectName"
	// Example: updateObjectName( ID, newName );
	static int Lua_UpdateObjectName( lua_State* L );

private:
	std::map< std::string /*scriptName*/, 
			  std::string /*scriptSource*/ > m_mapScripts;

	// Note that this is a POINTER to the vector of game object pointers
	static std::map< std::string, cMeshObject* >* m_p_vecGOs;
	// returns nullptr if not found
	static cMeshObject* m_findObjectByID( int ID );

	lua_State* m_pLuaState;

	std::string m_decodeLuaErrorToString( int error );
};

#endif
