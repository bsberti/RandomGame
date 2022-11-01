#pragma once

#include <string>

// Here's our new structure for the vertices (from October 24th and onward)
// This is the layout that the vertex shaders are expecting
struct sVertex_RGBA_XYZ_N_UV_T_BiN_Bones
{
	float r, g, b, a;		//	in vec4 vColour;			// Was vec3
	float x, y, z, w;		//	in vec4 vPosition;			// Was vec3
	float nx, ny, nz, nw;	//	in vec4 vNormal;			// Vertex normal
	float u0, v0, u1, v1;	//	in vec4 vUVx2;				// 2 x Texture coords
	float tx, ty, yz, tw;	//	in vec4 vTangent;			// For bump mapping
	float bx, by, bz, bw;	//	in vec4 vBiNormal;			// For bump mapping
	float vBoneID[4];		//	in vec4 vBoneID;		// For skinned mesh (FBX)
	float vBoneWeight[4];	//	in vec4 vBoneWeight;		// For skinned mesh (FBX)
};

struct sModelDrawInfo {
	sModelDrawInfo();

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	void CalculateExtents(void);

	// You could store the max and min values of the 
	//  vertices here (determined when you load them):
	float maxX, maxY, maxZ;
	float minX, minY, minZ;
	// These are the lengths of the bounding box that holds the model, 
	//	so extentX = maxX - minX, etc. 
	float extentX, extentY, extentZ;

//  scale = 1.5/maxExtent --> 1.5 x 1.5 x 1.5
//	scale = 5.0/maxExtent --> 5 x 5 x 5
	float maxExtent;

	// The "local" (i.e. "CPU side" temporary array)
	sVertex_RGBA_XYZ_N_UV_T_BiN_Bones* pVertices;	//  = 0;
	// The index buffer (CPU side)
	unsigned int* pIndices;
};
