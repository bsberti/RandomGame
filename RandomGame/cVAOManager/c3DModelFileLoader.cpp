#include "c3DModelFileLoader.h"

#include <fstream>

bool c3DModelFileLoader::LoadPLYFile_Format_XYZ_N_RGBA_UV(std::string filename, 
                                                          sModelDrawInfo& modelDrawInfo,
                                                          std::string& errorText)
{
    errorText = "";     // Clear the error (i.e. no error)

    struct sVertex_XYZ_N_RGBA_UV
    {
        float x, y, z;
        float nx, ny, nz;
        //uchar red, green, blue, alpha;
        float red, green, blue, alpha;
        float texture_u, texture_v;
    };

    struct sTrianglePLY
    {
        // The 3 vertex index values from the ply file
        unsigned int vertexIndices[3];
    };

    // Array we will load into
    sVertex_XYZ_N_RGBA_UV* pTheModelArray = NULL;   // NULL, 0, nullptr
    sTrianglePLY* pTheModelTriangleArray = NULL;
 //    unsigned int numberOfvertices = 0;
 //    unsigned int numberOfTriangles = 0;

     // Start loading the file

    std::ifstream theFile(filename);
    if (!theFile.is_open())
    {
//        std::cout << "Didn't open it!" << std::endl;
        errorText = "Can't open the file.";
        return false;
    }

    // Read an entire line
    char buffer[10000];
    theFile.getline(buffer, 10000);

    std::string theNextToken;

    // Scan for the word "vertex"
    while (theFile >> theNextToken)
    {
        if (theNextToken == "vertex")
        {
            break;
        }
    }
    // 
    theFile >> modelDrawInfo.numberOfVertices;

    // Scan for the word "face"
    while (theFile >> theNextToken)
    {
        if (theNextToken == "face")
        {
            break;
        }
    }
    // 
    theFile >> modelDrawInfo.numberOfTriangles;

    // Scan for the word "end_header"
    while (theFile >> theNextToken)
    {
        if (theNextToken == "end_header")
        {
            break;
        }
    }

    // Now we load the vertices
    // -0.036872 0.127727 0.00440925 0.2438786 0.9638011 -0.1077533 127 127 127 255 0.337485 0.8899501

    pTheModelArray = new sVertex_XYZ_N_RGBA_UV[modelDrawInfo.numberOfVertices];

//    std::cout << "Loading";
    for (unsigned int count = 0; count != modelDrawInfo.numberOfVertices; count++)
    {
        theFile >> pTheModelArray[count].x;
        theFile >> pTheModelArray[count].y;
        theFile >> pTheModelArray[count].z;

        theFile >> pTheModelArray[count].nx;
        theFile >> pTheModelArray[count].ny;
        theFile >> pTheModelArray[count].nz;

        theFile >> pTheModelArray[count].red;
        theFile >> pTheModelArray[count].green;
        theFile >> pTheModelArray[count].blue;
        theFile >> pTheModelArray[count].alpha;

        theFile >> pTheModelArray[count].texture_u;
        theFile >> pTheModelArray[count].texture_v;

//        if (count % 10000 == 0)
//        {
//            std::cout << ".";
//        }
    }
//    std::cout << "done" << std::endl;





    // Load the faces (or triangles)
    pTheModelTriangleArray = new sTrianglePLY[modelDrawInfo.numberOfTriangles];

    for (unsigned int count = 0; count != modelDrawInfo.numberOfTriangles; count++)
    {
        // 3 15393 15394 15395 
        unsigned int discard = 0;
        theFile >> discard;

        theFile >> pTheModelTriangleArray[count].vertexIndices[0];
        theFile >> pTheModelTriangleArray[count].vertexIndices[1];
        theFile >> pTheModelTriangleArray[count].vertexIndices[2];
    }

    theFile.close();

    // This is now different because the vertex layout in the shader is different
    modelDrawInfo.pVertices = new sVertex_RGBA_XYZ_N_UV_T_BiN_Bones[modelDrawInfo.numberOfVertices];
//    modelDrawInfo.pVertices = new sVertex[modelDrawInfo.numberOfVertices];

    // Now copy the information from the PLY infomation to the model draw info structure
    for (unsigned int index = 0; index != modelDrawInfo.numberOfVertices; index++)
    {
        // To The Shader                        From the file
        modelDrawInfo.pVertices[index].x = pTheModelArray[index].x;
        modelDrawInfo.pVertices[index].y = pTheModelArray[index].y;
        modelDrawInfo.pVertices[index].z = pTheModelArray[index].z;

        modelDrawInfo.pVertices[index].r = pTheModelArray[index].red;
        modelDrawInfo.pVertices[index].g = pTheModelArray[index].green;
        modelDrawInfo.pVertices[index].b = pTheModelArray[index].blue;

        // Copy the normal information we loaded, too! :)
        modelDrawInfo.pVertices[index].nx = pTheModelArray[index].nx;
        modelDrawInfo.pVertices[index].ny = pTheModelArray[index].ny;
        modelDrawInfo.pVertices[index].nz = pTheModelArray[index].nz;

    }

    modelDrawInfo.numberOfIndices = modelDrawInfo.numberOfTriangles * 3;

    // This is the "index" (or element) buffer
    modelDrawInfo.pIndices = new unsigned int[modelDrawInfo.numberOfIndices];

    unsigned int vertex_element_index_index = 0;

    for (unsigned int triangleIndex = 0; triangleIndex != modelDrawInfo.numberOfTriangles; triangleIndex++)
    {
        modelDrawInfo.pIndices[vertex_element_index_index + 0] = pTheModelTriangleArray[triangleIndex].vertexIndices[0];
        modelDrawInfo.pIndices[vertex_element_index_index + 1] = pTheModelTriangleArray[triangleIndex].vertexIndices[1];
        modelDrawInfo.pIndices[vertex_element_index_index + 2] = pTheModelTriangleArray[triangleIndex].vertexIndices[2];

        // Each +1 of the triangle index moves the "vertex element index" by 3
        // (3 vertices per triangle)
        vertex_element_index_index += 3;
    }

    delete[] pTheModelArray;
    delete[] pTheModelTriangleArray;


    return true;
}

