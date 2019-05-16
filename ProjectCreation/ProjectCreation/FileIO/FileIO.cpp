#include "FileIO.h"
#include <assert.h>
#include <string.h>
#include <fstream>
#include <sstream>

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
                staticMeshOutput->vertices.resize(vertCount);
                staticMeshOutput->indices.resize(indCount);
                myfile.read((char*)staticMeshOutput->vertices.data(), sizeof(FVertex) * vertCount);
                myfile.read((char*)staticMeshOutput->indices.data(), sizeof(uint32_t) * indCount);
                staticMeshOutput->name = fileName;

                myfile.close();

                output.m_Flags = ERESULT_FLAG::SUCCESS;
        }
        assert(output.m_Flags != ERESULT_FLAG::INVALID);

        return output;
}

EResult FileIO::LoadSkeletalMeshDataFromFile(const char* fileName, FSkeletalMeshData* skeletalMeshOutput)
{
        using namespace std;

        assert(skeletalMeshOutput != nullptr);
        EResult output;
        output.m_Flags = ERESULT_FLAG::INVALID;

        std::ostringstream filePathStream;
        filePathStream << "../Models/" << fileName << ".skel";

        ifstream myfile;
        myfile.open(filePathStream.str(), ios::in | ios::binary);

        if (myfile.is_open())
        {

                uint32_t jointCount;
                myfile.seekg(0, ios::beg);
                myfile.read((char*)&jointCount, sizeof(jointCount));
                skeletalMeshOutput->joints.resize(jointCount);
                skeletalMeshOutput->inverseJoints.resize(jointCount);
                myfile.read((char*)skeletalMeshOutput->joints.data(), sizeof(Animation::FJoint) * jointCount);
                myfile.read((char*)skeletalMeshOutput->inverseJoints.data(), sizeof(Animation::FJoint) * jointCount);


                uint32_t vertCount;
                uint32_t indCount;
                myfile.read((char*)skeletalMeshOutput->materialName.data(), skeletalMeshOutput->materialName.size());
                myfile.read((char*)&vertCount, sizeof(uint32_t));
                myfile.read((char*)&indCount, sizeof(uint32_t));
                skeletalMeshOutput->vertices.resize(vertCount);
                skeletalMeshOutput->indices.resize(indCount);
                myfile.read((char*)skeletalMeshOutput->vertices.data(), sizeof(FSkinnedVertex) * vertCount);
                myfile.read((char*)skeletalMeshOutput->indices.data(), sizeof(uint32_t) * indCount);
                skeletalMeshOutput->name = fileName;

                myfile.close();

                output.m_Flags = ERESULT_FLAG::SUCCESS;
        }
        assert(output.m_Flags != ERESULT_FLAG::INVALID);

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
                myfile.read((char*)&inMat.surfaceProperties, sizeof(inMat.surfaceProperties));
                myfile.read((char*)&inMat.surfaceType, sizeof(inMat.surfaceType));

                uint32_t descCount;
                myfile.read((char*)&descCount, sizeof(uint32_t));
                for (int i = 0; i < (int)descCount; ++i)
                {
                        FTextureDesc desc{};
                        myfile.read((char*)&desc.textureType, sizeof(ETexture2DType));
                        myfile.read(desc.filePath.data(), desc.filePath.size());
                        inMat.textureDescs.push_back(desc);
                }
                myfile.close();


                output.m_Flags = ERESULT_FLAG::SUCCESS;
        }

        assert(output.m_Flags != ERESULT_FLAG::INVALID);

        return output;
}

EResult FileIO::LoadShaderDataFromFile(const char* fileName, const char* suffix, FShaderData* shaderDataOutput)
{
        assert(shaderDataOutput != nullptr);
        EResult output;
        output.m_Flags = ERESULT_FLAG::INVALID;

        std::ostringstream filePathStream;
        filePathStream << "../Shaders/" << fileName << suffix << ".cso";

        std::basic_ifstream<char> stream(filePathStream.str(), std::ios::binary);

        if (stream.is_open())
        {
                shaderDataOutput->bytes =
                    std::vector<char>((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

                stream.close();

                output.m_Flags = ERESULT_FLAG::SUCCESS;
        }

		#if defined(_CRT_SECURE_NO_WARNINGS) && defined(_DEBUG)
		//  stores this value for debugging purposes if the following assertion fails
        std::string error_message = std::strerror(errno);
		#endif

        assert(output.m_Flags != ERESULT_FLAG::INVALID);

        return output;
}

EResult FileIO::ImportAnimClipData(const char* fileName, Animation::FAnimClip& animClip, const Animation::FSkeleton& skeleton)
{
        EResult output;
        output.m_Flags = ERESULT_FLAG::INVALID;

        std::ostringstream filePathStream;
        filePathStream << "../Animations/" << fileName << ".anim";

        std::ifstream myfile;
        myfile.open(filePathStream.str(), std::ios::in | std::ios::binary);

        int jointCount = skeleton.jointTransforms.size();

        if (myfile.is_open())
        {
                myfile.read((char*)&animClip.duration, sizeof(animClip.duration));
                uint32_t frameCount;
                myfile.read((char*)&frameCount, sizeof(uint32_t));
                animClip.frames.resize(frameCount);

                for (int i = 0; i < frameCount; ++i)
                {
                        animClip.frames[i].joints.resize(jointCount);
                        myfile.read((char*)&animClip.frames[i].time, sizeof(animClip.frames[i].time));
                        myfile.read((char*)animClip.frames[i].joints.data(), sizeof(FTransform) * jointCount);
                }

                animClip.frames.push_back(animClip.frames.front());
                animClip.duration += 1.0f / 24.0f;
                animClip.frames.back().time = animClip.duration;


                myfile.close();

                output.m_Flags = ERESULT_FLAG::SUCCESS;
        }
        assert(output.m_Flags != ERESULT_FLAG::INVALID);

        return output;
}
