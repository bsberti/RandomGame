#include "../globalOpenGL.h"

#include "cVAOManager.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <sstream>

sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = NULL;		// or 0 or nullptr
	this->pIndices = NULL;		// or 0 or nullptr

	this->minX = this->minY = this->minZ = 0.0f;
	this->maxX = this->maxY = this->maxZ = 0.0f;
	this->extentX = this->extentY = this->extentZ = 0.0f;
	this->maxExtent = 0.0f;

	return;
}

void sModelDrawInfo::CalculateExtents(void)
{
	// Do we even have an array?
	if ( this->pVertices )		// same as != NULL
	{
		// Assume that the 1st vertex is both the min and max
		this->minX = this->maxX = this->pVertices[0].x;
		this->minY = this->maxY = this->pVertices[0].y;
		this->minZ = this->maxZ = this->pVertices[0].z;

		for (unsigned int index = 0; index != this->numberOfVertices; index++)
		{
			// See if "this" vertex is smaller than the min
			if (this->pVertices[index].x < this->minX) { this->minX = this->pVertices[index].x;	}
			if (this->pVertices[index].y < this->minY) { this->minY = this->pVertices[index].y; }
			if (this->pVertices[index].z < this->minZ) { this->minZ = this->pVertices[index].z; }

			// See if "this" vertex is larger than the max
			if (this->pVertices[index].x > this->maxX) { this->maxX = this->pVertices[index].x;	}
			if (this->pVertices[index].y > this->maxY) { this->maxY = this->pVertices[index].y; }
			if (this->pVertices[index].z > this->maxZ) { this->maxZ = this->pVertices[index].z; }
		}//for (unsigned int index = 0...
	}//if ( this->pVertices )

	// Update the extents
	this->extentX = this->maxX - this->minX;
	this->extentY = this->maxY - this->minY;
	this->extentZ = this->maxZ - this->minZ;

	// What's the largest of the three extents
	this->maxExtent = this->extentX;
	if (this->extentY > this->maxExtent) { this->maxExtent = this->extentY; }
	if (this->extentZ > this->maxExtent) { this->maxExtent = this->extentZ; }

	return;
}



bool cVAOManager::LoadModelIntoVAO(
		std::string fileName, 
		sModelDrawInfo &drawInfo,
	    unsigned int shaderProgramID)

{
	// Load the model from file
	// (We do this here, since if we can't load it, there's 
	//	no point in doing anything else, right?)

	drawInfo.meshName = fileName;

	// Calculate the extents of this model
	drawInfo.CalculateExtents();


	// TODO: Load the model from file

	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	// sVert vertices[3]
	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
				  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
				  GL_STATIC_DRAW );


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
	              sizeof( unsigned int ) * drawInfo.numberOfIndices, 
	              (GLvoid*) drawInfo.pIndices,
                  GL_STATIC_DRAW );


//   __     __        _              _                            _   
//   \ \   / /__ _ __| |_ _____  __ | |    __ _ _   _  ___  _   _| |_ 
//    \ \ / / _ \ '__| __/ _ \ \/ / | |   / _` | | | |/ _ \| | | | __|
//     \ V /  __/ |  | ||  __/>  <  | |__| (_| | |_| | (_) | |_| | |_ 
//      \_/ \___|_|   \__\___/_/\_\ |_____\__,_|\__, |\___/ \__,_|\__|
//                                              |___/                 

	// in vec4 vColour;
	GLint vColour_location = glGetAttribLocation(shaderProgramID, "vColour");	
	glEnableVertexAttribArray(vColour_location);	
	glVertexAttribPointer(vColour_location, 
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, r) );		// Offset the member variable

	//in vec4 vPosition;			
	GLint vPosition_location = glGetAttribLocation(shaderProgramID, "vPosition");
	glEnableVertexAttribArray(vPosition_location);
	glVertexAttribPointer(vPosition_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, x) );		// Offset the member variable

	//in vec4 vNormal;			
	GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");
	glEnableVertexAttribArray(vNormal_location);
	glVertexAttribPointer(vNormal_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, nx) );		// Offset the member variable

	//in vec4 vUVx2;			
	GLint vUVx2_location = glGetAttribLocation(shaderProgramID, "vUVx2");
	glEnableVertexAttribArray(vUVx2_location);
	glVertexAttribPointer(vUVx2_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, u0) );		// Offset the member variable

	//in vec4 vTangent;			
	GLint vTangent_location = glGetAttribLocation(shaderProgramID, "vTangent");
	glEnableVertexAttribArray(vTangent_location);
	glVertexAttribPointer(vTangent_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, tx) );		// Offset the member variable

	//in vec4 vBiNormal;		
	GLint vBiNormal_location = glGetAttribLocation(shaderProgramID, "vBiNormal");
	glEnableVertexAttribArray(vBiNormal_location);
	glVertexAttribPointer(vBiNormal_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, bx) );		// Offset the member variable

	//in vec4 vBoneID;			
	GLint vBoneID_location = glGetAttribLocation(shaderProgramID, "vBoneID");
	glEnableVertexAttribArray(vBoneID_location);
	glVertexAttribPointer(vBoneID_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, vBoneID[0]) );		// Offset the member variable

	//in vec4 vBoneWeight;		
	GLint vBoneWeight_location = glGetAttribLocation(shaderProgramID, "vBoneWeight");
	glEnableVertexAttribArray(vBoneWeight_location);
	glVertexAttribPointer(vBoneWeight_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, vBoneWeight[0]) );		// Offset the member variable




	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	glDisableVertexAttribArray(vColour_location);
	glDisableVertexAttribArray(vPosition_location);
	glDisableVertexAttribArray(vNormal_location);
	glDisableVertexAttribArray(vUVx2_location);
	glDisableVertexAttribArray(vTangent_location);
	glDisableVertexAttribArray(vBiNormal_location);
	glDisableVertexAttribArray(vBoneID_location);
	glDisableVertexAttribArray(vBoneWeight_location);

	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[ drawInfo.meshName ] = drawInfo;


	return true;
}


// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}

