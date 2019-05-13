#include "FileIO.h"
#include <assert.h>
#include <sstream>

#include <fstream>

EResult FileIO::LoadStaticMeshDataFromFile(const char* fileName, FStaticMeshData* staticMeshOutput)
{
        using namespace std;

        assert(staticMeshOutput != nullptr);
        EResult output;
        output.m_Flags = ERESULT_FLAG::INVALID;

        std::ostringstream filePathStream;
        filePathStream << "../Models/" << fileName << ".mesh";

        ifstream myfile;
        myfile.open(filePathStream.str(), ios::in | ios::binary);

        if (myfile.is_open())
        {
                uint32_t vertCount;
                uint32_t indCount;
                myfile.seekg(0, ios::beg);
                myfile.read((char*)staticMeshOutput->materialName.data(), staticMeshOutput->materialName.size());
                myfile.read((char*)&vertCount, sizeof(uint32_t));
                myfile.read((char*)&indCount, sizeof(uint32_t));
                //staticMeshOutput->vertices.resize(vertCount);
                staticMeshOutput->indices.resize(indCount);
                //myfile.read((char*)staticMeshOutput->vertices.data(), sizeof(Vertex) * vertCount);
                myfile.read((char*)staticMeshOutput->indices.data(), sizeof(uint32_t) * indCount);
                staticMeshOutput->name = fileName;

                myfile.close();

                output.m_Flags = ERESULT_FLAG::SUCCESS;
        }
        return output;
}

EResult FileIO::LoadSkeletalMeshDataFromFile(const char* fileName, FSkeletalMeshData* skeletalMeshOutput)
{
        using namespace std;

        assert(skeletalMeshOutput != nullptr);
        EResult output;
        output.m_Flags = ERESULT_FLAG::INVALID;

        std::ostringstream filePathStream;
        filePathStream << "../Models/" << fileName << ".mesh";

        ifstream myfile;
        myfile.open(filePathStream.str(), ios::in | ios::binary);

        if (myfile.is_open())
        {
                uint32_t vertCount;
                uint32_t indCount;
                myfile.seekg(0, ios::beg);
                //myfile.read((char*)skeletalMeshOutput->materialName.data(), skeletalMeshOutput->materialName.size());
                myfile.read((char*)&vertCount, sizeof(uint32_t));
                myfile.read((char*)&indCount, sizeof(uint32_t));
                // staticMeshOutput->vertices.resize(vertCount);
                skeletalMeshOutput->indices.resize(indCount);
                // myfile.read((char*)staticMeshOutput->vertices.data(), sizeof(Vertex) * vertCount);
                myfile.read((char*)skeletalMeshOutput->indices.data(), sizeof(uint32_t) * indCount);
                skeletalMeshOutput->name = fileName;

                myfile.close();

                output.m_Flags = ERESULT_FLAG::SUCCESS;
        }
        return output;
}

EResult FileIO::LoadMaterialDataFromFile(const char* fileName, FMaterialData* materialOutput)
{
        using namespace std;

        assert(materialOutput != nullptr);
        EResult output;
        output.m_Flags = ERESULT_FLAG::INVALID;

        std::ostringstream filePathStream;
        filePathStream << "../Materials/" << fileName << ".material";

        ifstream myfile;
        myfile.open(filePathStream.str(), ios::in | ios::binary);

        if (myfile.is_open())
        {
                FMaterialData inMat;
                //myfile.read((char*)&inMat.surfaceProperties, sizeof(inMat.surfaceProperties));
                //myfile.read((char*)&inMat.surfaceType, sizeof(inMat.surfaceType));

                uint32_t descCount;
                myfile.read((char*)&descCount, sizeof(uint32_t));
                for (int i = 0; i < descCount; ++i)
                {
                        FTextureDesc desc{};
                        //myfile.read((char*)&desc.textureType, sizeof(ETextureType));
                        myfile.read(desc.filePath.data(), desc.filePath.size());
                        inMat.textureDescs.push_back(desc);
                }
                myfile.close();


                output.m_Flags = ERESULT_FLAG::SUCCESS;
        }
        return output;
}
