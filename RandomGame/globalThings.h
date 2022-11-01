#ifndef _globalThings_HG_
#define _globalThings_HG_

// This is anything that is shared by all (or many) of the files

#include "cMeshObject.h"

#include "cLightManager.h"

// extern means the variable isn't actually here...
// ...it's somewhere else (in a .cpp file somewhere)
extern cLightManager* g_pTheLightManager;

#endif
