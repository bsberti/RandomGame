// Vertex shader
#version 420

// Original vertex layout
//in vec3 vCol;
//in vec3 vPos;
//in vec3 vNormal;
// New one (which we aren't going to change again! Hazzah!)
in vec4 vColour;			// Was vec3
in vec4 vPosition;			// Was vec3
in vec4 vNormal;			// Vertex normal
in vec4 vUVx2;				// 2 x Texture coords
in vec4 vTangent;			// For bump mapping
in vec4 vBiNormal;			// For bump mapping
in vec4 vBoneID;			// For skinned mesh (FBX)
in vec4 vBoneWeight;		// For skinned mesh (FBX)

// Going "out" to the fragment shader
out vec4 fColour;		// colour;			
out vec4 fNormal;
out vec4 fVertWorldLocation;	// using only matModel
out vec4 fUVx2;
out vec4 fTangent;
out vec4 fBinormal;

//uniform mat4 MVP;
uniform mat4 mModel;
uniform mat4 mModelInverseTranspose;		// mModel with Only Rotation;
uniform mat4 mView;
uniform mat4 mProjection;

void main()
{
	vec3 vertPosition = vPosition.xyz;
	
	
	// Output is in screen space 
	// x & y are in (normalized) screen space, z is the depth from the camera
	mat4 mMVP = mProjection * mView * mModel;
	
	gl_Position = mMVP * vec4(vertPosition, 1.0f);
	
	// The location of the vertex in WORLD SPACE 
	// for the lighting
	fVertWorldLocation.xyz = (mModel * vec4(vertPosition, 1.0f)).xyz;
	fVertWorldLocation.w = 1.0f;
	
	// Send the normals to the fragment shader
	// Rotate the normal if the model is rotated
	fNormal.xyz = normalize(mModelInverseTranspose * vec4(vNormal.xyz, 1.0f)).xyz;
	fNormal.w = 1.0f;
	
	// Copy the rest of the values to the fragment shader
	fColour = vColour;
	fUVx2 = vUVx2;
	fTangent = vTangent;
	fBinormal = vBiNormal;
}


// You can also set the layout with the location keyword
//	layout(location=0) in vec3 vCol;
//	layout(location=1) in vec3 vPos;
//	layout(location=2) in vec3 vNormal;
