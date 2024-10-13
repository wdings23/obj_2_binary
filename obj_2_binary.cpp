

#include <cstdint>
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include <mutex>
#include <map>

#include "vec.h"
#include "LogPrint.h"

std::mutex gMutex;

struct Face
{
    uint32_t                                    miIndex = UINT32_MAX;

    std::vector<uint32_t>                       maiPositions;
    std::vector<uint32_t>                       maiUVs;
    std::vector<uint32_t>                       maiNormals;

    bool operator == (Face const& face)
    {
        return miIndex == face.miIndex;
    }
};

struct OBJMaterialInfo
{
    float4              mDiffuse;
    float4              mSpecular;
    float4              mEmissive;
    std::string         mName;
    std::string         mTexturePath;
};

struct MeshRange
{
    uint32_t            miStart;
    uint32_t            miEnd;
};

struct Vertex
{
    vec4        mPosition;
    vec4        mNormal;
    vec2        mUV;
};

struct MeshExtent
{
    vec3            mMinPosition;
    vec3            mMaxPosition;
};


void loadOBJ(
    std::vector<std::vector<vec3>>& aaPositions,
    std::vector<std::vector<vec3>>& aaNormals,
    std::vector<std::vector<vec2>>& aaUVs,
    std::vector<std::vector<std::vector<uint32_t>>>& aaiFacePositionIndices,
    std::vector<std::vector<std::vector<uint32_t>>>& aaiFaceNormalIndices,
    std::vector<std::vector<std::vector<uint32_t>>>& aaiFaceUVIndices,
    std::vector<OBJMaterialInfo>& aMaterials,
    char const* szFilePath);

void outputCoordinates(
    std::vector<std::vector<vec3>> const& aaPositions,
    std::vector<std::vector<vec3>> const& aaNormals,
    std::vector<std::vector<vec2>> const& aaUVs,
    std::vector<std::vector<std::vector<uint32_t>>> const& aaiFacePositionIndices,
    std::vector<std::vector<std::vector<uint32_t>>> const& aaiFaceNormalIndices,
    std::vector<std::vector<std::vector<uint32_t>>> const& aaiFaceUVIndices,
    std::string const& directory,
    std::string const& baseName);

void makeVertices(
    std::vector<Vertex>& aTotalVertices,
    std::vector<std::vector<uint32_t>>& aaiTriangleVertexIndices,
    std::vector<MeshExtent>& aExtents,
    std::vector<vec3> const& aTotalPositions,
    std::vector<vec3> const& aTotalNormals,
    std::vector<vec2> const& aTotalUVs,
    std::vector<std::vector<std::vector<uint32_t>>> const& aaiFacePositionIndices,
    std::vector<std::vector<std::vector<uint32_t>>> const& aaiFaceNormalIndices,
    std::vector<std::vector<std::vector<uint32_t>>> const& aaiFaceUVIndices);

void outputVerticesAndTriangles(
    std::vector<Vertex> const& aTotalVertices,
    std::vector<std::vector<uint32_t>> const& aaiTriangleVertexIndices,
    std::vector<MeshExtent> const& aMeshExtents,
    std::string const& directory,
    std::string const& baseName);

void test(
    std::vector<Vertex>& aTotalVertices,
    std::vector<std::vector<uint32_t>>& aaiTriangleIndices,
    std::vector<MeshRange>& aMeshRanges,
    std::vector<MeshExtent>& aMeshExtents,
    std::string const& fullPath);

void saveOBJ(
    std::string const& fullPath,
    std::vector<Vertex> const& aTotalVertices,
    std::vector<std::vector<uint32_t>> const& aaiTriangleVertexIndices,
    std::vector<uint32_t> const& aiMeshes);

int main()
{
    std::vector<std::vector<vec3>> aaPositions;
    std::vector<std::vector<vec3>> aaNormals;
    std::vector<std::vector<vec2>> aaUVs;
    
    std::vector<std::vector<std::vector<uint32_t>>> aaiFacePositionIndices;
    std::vector<std::vector<std::vector<uint32_t>>> aaiFaceNormalIndices;
    std::vector<std::vector<std::vector<uint32_t>>> aaiFaceUVIndices;

    std::vector<OBJMaterialInfo> aMaterials;

    std::string fullPath = "d:\\Downloads\\Bistro_v4\\bistro.obj";
    auto iter = fullPath.rfind("\\");
    std::string directory = fullPath.substr(0, iter);
    std::string fileName = fullPath.substr(iter + 1);

    auto extensionIter = fileName.rfind(".obj");
    std::string baseName = fileName.substr(0, extensionIter);

    loadOBJ(
        aaPositions,
        aaNormals,
        aaUVs,
        aaiFacePositionIndices,
        aaiFaceNormalIndices,
        aaiFaceUVIndices,
        aMaterials,
        fullPath.c_str());

    // one big list for positions, normals, and uvs
    std::vector<vec3> aTotalPositions;
    for(auto const& aPositions : aaPositions)
    {
        aTotalPositions.insert(aTotalPositions.end(), aPositions.begin(), aPositions.end());
    }
    std::vector<vec3> aTotalNormals;
    for(auto const& aNormals : aaNormals)
    {
        aTotalNormals.insert(aTotalNormals.end(), aNormals.begin(), aNormals.end());
    }
    std::vector<vec2> aTotalUVs;
    for(auto const& aUVs : aaUVs)
    {
        aTotalUVs.insert(aTotalUVs.end(), aUVs.begin(), aUVs.end());
    }

    std::vector<Vertex> aTotalVertices;
    std::vector<std::vector<uint32_t>> aaiTriangleVertexIndices;
    std::vector<MeshExtent> aMeshExtents;
    makeVertices(
        aTotalVertices,
        aaiTriangleVertexIndices,
        aMeshExtents,
        aTotalPositions,
        aTotalNormals,
        aTotalUVs,
        aaiFacePositionIndices,
        aaiFaceNormalIndices,
        aaiFaceUVIndices);

    outputVerticesAndTriangles(
        aTotalVertices,
        aaiTriangleVertexIndices,
        aMeshExtents,
        directory,
        baseName);

    std::vector<Vertex> aTestTotalVertices;
    std::vector<std::vector<uint32_t>> aaiTriangleIndices;
    std::vector<MeshRange> aMeshRanges;
    std::vector<MeshExtent> aTestMeshExtents;
    test(
        aTestTotalVertices,
        aaiTriangleIndices,
        aMeshRanges,
        aTestMeshExtents,
        "d:\\Downloads\\Bistro_v4\\bistro-triangles.bin");
}

/*
**
*/
std::vector<std::string> split(const char* str, char c = ' ')
{
    std::vector<std::string> result;
    do
    {
        const char* begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(std::string(begin, str));
    } while(0 != *str++);

    return result;
}

/*
**
*/
void getPositionRange(
    std::vector<uint32_t>& ret,
    std::vector<std::vector<vec3>> const& aaPositions)
{
    uint32_t iPositionLength = aaPositions.size();
    ret.resize(2);
    ret[0] = ret[1] = 0;

    uint32_t iNumPositions = 0;
    for(uint32_t i = 0; i < iPositionLength; i++)
    {
        iNumPositions += (uint32_t)aaPositions[i].size();
        if(i == iPositionLength - 2)
        {
            ret[0] = iNumPositions;
        }
    }

    ret[1] = iNumPositions;
}

/*
**
*/
void loadOBJ(
    std::vector<std::vector<vec3>>& aaPositions,
    std::vector<std::vector<vec3>>& aaNormals,
    std::vector<std::vector<vec2>>& aaUVs,
    std::vector<std::vector<std::vector<uint32_t>>>& aaiFacePositionIndices,
    std::vector<std::vector<std::vector<uint32_t>>>& aaiFaceNormalIndices,
    std::vector<std::vector<std::vector<uint32_t>>>& aaiFaceUVIndices,
    std::vector<OBJMaterialInfo>& aMaterials,
    char const* szFilePath)
{
    aaPositions.resize(0);
    aaNormals.resize(0);
    aaUVs.resize(0);
    aaiFacePositionIndices.resize(0);
    aaiFaceNormalIndices.resize(0);
    aaiFaceUVIndices.resize(0);
    aMaterials.resize(0);

    std::string filePath(szFilePath);
    auto lastFowardSlash = filePath.find_last_of("/");
    auto lastBackSlash = filePath.find_last_of("\\");
    std::string directory = (lastFowardSlash != std::string::npos && lastFowardSlash > lastBackSlash) ? filePath.substr(0, lastFowardSlash) : filePath.substr(0, lastBackSlash);

    std::vector<std::string> aMeshNames;
    std::vector<vec3> aTotalPositions;
    std::vector<vec3> aTotalNormals;
    std::vector<vec2> aTotalUVs;
    std::vector<uint32_t> aTotalFacePositionIndices;
    std::vector<uint32_t> aTotalFaceNormalIndices;
    std::vector<uint32_t> aTotalFaceUVIndices;

    std::vector<std::vector<uint32_t>> aSeparateMeshPositionRanges;
    std::vector<std::string> aTotalMeshNames;

    std::vector<std::vector<std::vector<uint32_t>>> aaiTotalFacePositionIndices;
    std::vector<std::vector<std::vector<uint32_t>>> aaiTotalFaceNormalIndices;
    std::vector<std::vector<std::vector<uint32_t>>> aaiTotalFaceUVIndices;

    // read file
    FILE* fp = fopen(szFilePath, "rb");
    fseek(fp, 0, SEEK_END);
    uint64_t iFileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    std::vector<char> acFileContent((uint32_t)iFileSize + 1);
    acFileContent[(uint32_t)iFileSize] = 0;
    fread(acFileContent.data(), sizeof(char), (uint32_t)iFileSize, fp);
    fclose(fp);
    std::string fileContent = acFileContent.data();

    uint32_t iCurrMeshIndex = 0;
    uint32_t iCurrTotalMaterialMeshIndex = 0;
    int64_t iCurrPosition = 0;
    for(;;)
    {
        int64_t iEnd = fileContent.find('\n', (uint32_t)iCurrPosition);
        if(iEnd < 0 || iEnd > fileContent.size() || iCurrPosition >= fileContent.size())
        {
            break;
        }

#if 0
        if(aMeshNames.size() >= 12)
        {
            aMeshNames.erase(aMeshNames.end() - 1);
            if(aaPositions[aaPositions.size() - 1].size() <= 0)
            {
                aaPositions.erase(aaPositions.end() - 1);
            }

            if(aaNormals[aaNormals.size() - 1].size() <= 0)
            {
                aaNormals.erase(aaNormals.end() - 1);
            }

            if(aaUVs[aaUVs.size() - 1].size() <= 0)
            {
                aaUVs.erase(aaUVs.end() - 1);
            }

            break;
        }
#endif // #if 0

        std::string line = fileContent.substr((uint32_t)iCurrPosition, (uint32_t)(iEnd - iCurrPosition));
        iCurrPosition = iEnd + 1;

        std::vector<std::string> aTokens = split(line.c_str(), ' ');
        if(aTokens[0] == "v")
        {
            // position
            if(aaPositions.size() <= iCurrMeshIndex)
            {
                aaPositions.resize(aaPositions.size() + 1);
            }

            vec3 pos = vec3(
                (float)atof(aTokens[1].c_str()),
                (float)atof(aTokens[2].c_str()),
                (float)atof(aTokens[3].c_str())
            );

            aaPositions[iCurrMeshIndex].push_back(
                pos
            );

            aTotalPositions.push_back(pos);
        }
        else if(aTokens[0] == "vn")
        {
            // normal
            if(aaNormals.size() <= iCurrMeshIndex)
            {
                aaNormals.resize(aaNormals.size() + 1);
            }

            vec3 pos = vec3(
                (float)atof(aTokens[1].c_str()),
                (float)atof(aTokens[2].c_str()),
                (float)atof(aTokens[3].c_str())
            );

            aaNormals[iCurrMeshIndex].push_back(
                pos
            );

            aTotalNormals.push_back(pos);
        }
        else if(aTokens[0] == "vt")
        {
            // uv
            if(aaUVs.size() <= iCurrMeshIndex)
            {
                aaUVs.resize(aaUVs.size() + 1);
            }

            vec2 pos = vec2(
                (float)atof(aTokens[1].c_str()),
                (float)atof(aTokens[2].c_str())
            );

            aaUVs[iCurrMeshIndex].push_back(
                pos
            );

            aTotalUVs.push_back(pos);
        }
        else if(aTokens[0][0] == 'f')
        {
            // face
            // use the material mesh index instead of just mesh index due to separating out different materials on the same mesh
            if(aaiFacePositionIndices.size() <= iCurrTotalMaterialMeshIndex)
            {
                aaiFacePositionIndices.resize(aaiFacePositionIndices.size() + 1);
                assert(aaiFacePositionIndices.size() > iCurrTotalMaterialMeshIndex);
            }
            if(aaiFaceNormalIndices.size() <= iCurrTotalMaterialMeshIndex)
            {
                aaiFaceNormalIndices.resize(aaiFaceNormalIndices.size() + 1);
                assert(aaiFaceNormalIndices.size() > iCurrTotalMaterialMeshIndex);
            }
            if(aaiFaceUVIndices.size() <= iCurrTotalMaterialMeshIndex)
            {
                aaiFaceUVIndices.resize(aaiFaceUVIndices.size() + 1);
                assert(aaiFaceUVIndices.size() > iCurrTotalMaterialMeshIndex);
            }

            if(aaiFacePositionIndices[aaiFacePositionIndices.size() - 1].size() <= 0)
            {
                DEBUG_PRINTF("\t\t\tstart mesh %d face\n", 
                    iCurrTotalMaterialMeshIndex);
            }

            // face indices
            std::vector<uint32_t> aTrianglePositionIndices;
            std::vector<uint32_t> aTriangleUVIndices;
            std::vector<uint32_t> aTriangleNormalIndices;
            for(uint32_t i = 1; i < 4; i++)
            {
                std::vector<std::string> aPositionUVNorm = split(aTokens[i].c_str(), '/');
                uint32_t iFacePositionIndex = atoi(aPositionUVNorm[0].c_str()) - 1;
                aTrianglePositionIndices.push_back(iFacePositionIndex);

                if(aPositionUVNorm.size() > 1)
                {
                    uint32_t iFaceUVIndex = atoi(aPositionUVNorm[1].c_str()) - 1;
                    aTriangleUVIndices.push_back(iFaceUVIndex);
                }

                if(aPositionUVNorm.size() > 2)
                {
                    uint32_t iFaceNormalIndex = atoi(aPositionUVNorm[2].c_str()) - 1;
                    aTriangleNormalIndices.push_back(iFaceNormalIndex);
                }
            }   // for i = 1 to 4

            // add face indices
            aaiFacePositionIndices[iCurrTotalMaterialMeshIndex].push_back(aTrianglePositionIndices);
            if(aTriangleUVIndices.size() > 0)
            {
                aaiFaceUVIndices[iCurrTotalMaterialMeshIndex].push_back(aTriangleUVIndices);
            }
            if(aTriangleNormalIndices.size() > 0)
            {
                aaiFaceNormalIndices[iCurrTotalMaterialMeshIndex].push_back(aTriangleNormalIndices);
            }

            // append to total indices
            aTotalFacePositionIndices.push_back(aTrianglePositionIndices[0]);
            aTotalFacePositionIndices.push_back(aTrianglePositionIndices[1]);
            aTotalFacePositionIndices.push_back(aTrianglePositionIndices[2]);
            if(aTriangleNormalIndices.size() > 0)
            {
                aTotalFaceNormalIndices.push_back(aTriangleNormalIndices[0]);
                aTotalFaceNormalIndices.push_back(aTriangleNormalIndices[1]);
                aTotalFaceNormalIndices.push_back(aTriangleNormalIndices[2]);
            }
            if(aTriangleUVIndices.size() > 0)
            {
                aTotalFaceUVIndices.push_back(aTriangleUVIndices[0]);
                aTotalFaceUVIndices.push_back(aTriangleUVIndices[1]);
                aTotalFaceUVIndices.push_back(aTriangleUVIndices[2]);
            }

        }   // if token == 'f'
        else if(aTokens[0][0] == 'o')
        {
            aaPositions.resize(aaPositions.size() + 1);
            aMeshNames.push_back(aTokens[1]);
            iCurrMeshIndex = aMeshNames.size() - 1;

            DEBUG_PRINTF("%d %s %s\n", 
                iCurrMeshIndex, 
                aTokens[0].c_str(), 
                aMeshNames[aMeshNames.size()-1].c_str());

        }   // else if token == 'o'
        else if(aTokens[0] == "usemtl")
        {
            // separate mesh for new material
            
            std::vector<uint32_t> aPositionCountRange;
            getPositionRange(aPositionCountRange, aaPositions);
            aSeparateMeshPositionRanges.push_back(aPositionCountRange);

            OBJMaterialInfo material;
            material.mName = aTokens[1];
            auto materialIter = std::find_if(
                aMaterials.begin(),
                aMaterials.end(),
                [material](OBJMaterialInfo const& checkMaterial)
                {
                    return material.mName == checkMaterial.mName;
                }
            );
            if(materialIter == aMaterials.end())
            {
                aMaterials.push_back(material);
            }

            // new mesh material name
            std::string meshMaterialName = aMeshNames[aMeshNames.size() - 1] + "-" + aTokens[1];
            auto iter = std::find(aTotalMeshNames.begin(), aTotalMeshNames.end(), meshMaterialName);
            if(iter != aTotalMeshNames.end())
            {
                uint32_t iIndex = 0;
                std::string oldMeshName = meshMaterialName;
                for(;; iIndex++)
                {
                    std::stringstream oss;
                    oss << meshMaterialName << iIndex;
                    iter = std::find(aTotalMeshNames.begin(), aTotalMeshNames.end(), oss.str());
                    if(iter == aTotalMeshNames.end())
                    {
                        meshMaterialName = oss.str();
                        break;
                    }
                }
            }

            aTotalMeshNames.push_back(meshMaterialName);
            iCurrTotalMaterialMeshIndex = aTotalMeshNames.size() - 1;

            DEBUG_PRINTF("\t%d %s %s\n", 
                iCurrTotalMaterialMeshIndex,
                aTokens[0].c_str(), 
                aTokens[1].c_str());
            DEBUG_PRINTF("\t\t# names: %d, # material names: %d %s\n",
                aMeshNames.size(),
                aTotalMeshNames.size(),
                meshMaterialName.c_str());

        }   // token == "usemtl"

    }   // for ;;

    // last mesh
    std::vector<uint32_t> aPositionCountRange;
    getPositionRange(aPositionCountRange, aaPositions);
    aSeparateMeshPositionRanges.push_back(aPositionCountRange);

    uint32_t iCurrRangeStart = 0;
    std::vector<MeshRange> aTriangleRanges;
    for(auto const& aiFacePositionIndices : aaiFacePositionIndices)
    {
        MeshRange range;
        range.miStart = iCurrRangeStart;
        range.miEnd = range.miStart + (uint32_t)aiFacePositionIndices.size();
        aTriangleRanges.push_back(range);
        iCurrRangeStart += (uint32_t)aiFacePositionIndices.size();
    }

    assert(aMeshNames.size() == aaPositions.size());
    assert(aTotalMeshNames.size() == aaiFacePositionIndices.size());

}

/*
**
*/
void getBoundingBoxes(
    std::vector<vec3>& aMinPositions,
    std::vector<vec3>& aMaxPositions,
    std::vector<vec3> const& aTotalPositions,
    std::vector<std::vector<std::vector<uint32_t>>> const& aaiFacePositionIndices)
{
    uint32_t iNumMeshes = (uint32_t)aaiFacePositionIndices.size();
    
    for(auto const& aiFacePositionIndices : aaiFacePositionIndices)
    {
        vec3 minPosition(FLT_MAX, FLT_MAX, FLT_MAX);
        vec3 maxPosition(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for(auto const& indices : aiFacePositionIndices)
        {
            for(auto const& iIndex : indices)
            {
                vec3 const& position = aTotalPositions[iIndex];
                minPosition = fminf(minPosition, position);
                maxPosition = fmaxf(maxPosition, position);
            }
        }
        
        aMinPositions.push_back(minPosition);
        aMaxPositions.push_back(maxPosition);
    }
}

/*
**
*/
void makeVertices(
    std::vector<Vertex>& aTotalVertices,
    std::vector<std::vector<uint32_t>>& aaiTriangleVertexIndices,
    std::vector<MeshExtent>& aExtents,
    std::vector<vec3> const& aTotalPositions,
    std::vector<vec3> const& aTotalNormals,
    std::vector<vec2> const& aTotalUVs,
    std::vector<std::vector<std::vector<uint32_t>>> const& aaiFacePositionIndices,
    std::vector<std::vector<std::vector<uint32_t>>> const& aaiFaceNormalIndices,
    std::vector<std::vector<std::vector<uint32_t>>> const& aaiFaceUVIndices)
{
    DEBUG_PRINTF("start vertex creation\n");

    struct VertexInfo
    {
        Vertex      mVertex;
        uint32_t    miTotalIndex;
    };

    uint32_t iNumMeshes = (uint32_t)aaiFacePositionIndices.size();
    aaiTriangleVertexIndices.resize(iNumMeshes);

    std::vector<std::vector<std::string>> aaFaceVertexKeys;
    std::map<std::string, VertexInfo> aVertexMap;
    for(uint32_t iMesh = 0; iMesh < aaiFacePositionIndices.size(); iMesh++)
    {
        vec3 minPosition(FLT_MAX, FLT_MAX, FLT_MAX);
        vec3 maxPosition(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        std::vector<std::vector<uint32_t>> const& aiFacePositionIndices = aaiFacePositionIndices[iMesh];
        std::vector<std::vector<uint32_t>> const& aiFaceNormalIndices = aaiFaceNormalIndices[iMesh];
        std::vector<std::vector<uint32_t>> const& aiFaceUVIndices = aaiFaceUVIndices[iMesh];

        DEBUG_PRINTF("\tmesh %d (%d) vertices\n", iMesh, iNumMeshes);

        aaFaceVertexKeys.resize(aaFaceVertexKeys.size() + 1);
        for(uint32_t iTriangleIndex = 0; iTriangleIndex < aiFacePositionIndices.size(); iTriangleIndex++)
        {
            assert(aiFacePositionIndices[iTriangleIndex].size() == 3);
            for(uint32_t i = 0; i < 3; i++)
            {
                uint32_t iV = aiFacePositionIndices[iTriangleIndex][i];
                uint32_t iN = aiFaceNormalIndices[iTriangleIndex][i];
                uint32_t iUV = aiFaceUVIndices[iTriangleIndex][i];

                vec3 const& pos = aTotalPositions[iV];
                vec3 const& normal = aTotalNormals[iN];
                vec2 const& uv = aTotalUVs[iUV];

                // check for existing key in map
                Vertex vertex;
                vertex.mPosition = vec4(pos, 1.0f);
                vertex.mNormal = vec4(normal, 1.0f);
                vertex.mUV = uv;
                char szKey[128];
                memset(szKey, 0, sizeof(char) * 128);
                sprintf(szKey, "%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f",
                    pos.x, pos.y, pos.z,
                    normal.x, normal.y, normal.z,
                    uv.x, uv.y);
                std::string key = szKey;

                minPosition = fminf(minPosition, pos);
                maxPosition = fmaxf(maxPosition, pos);

                // add to total vertex list if new, or get the saved total vertex list index
                uint32_t iVertexIndex = 0;
                auto vertexMapIter = aVertexMap.find(key);
                if(vertexMapIter == aVertexMap.end())
                {
                    VertexInfo info;
                    info.mVertex = vertex;
                    info.miTotalIndex = (uint32_t)aTotalVertices.size();
                    aVertexMap[key] = info;

                    iVertexIndex = info.miTotalIndex;
                    aTotalVertices.push_back(vertex);
                }
                else
                {
                    iVertexIndex = vertexMapIter->second.miTotalIndex;
                }
                
                //aaFaceVertexKeys[iMesh].push_back(key);
                aaiTriangleVertexIndices[iMesh].push_back(iVertexIndex);

            }   // for i = 0 to 3

        }   // for position index = 0 to num positions

        MeshExtent extent;
        extent.mMinPosition = minPosition;
        extent.mMaxPosition = maxPosition;
        aExtents.push_back(extent);

    }   // for mesh = 0 to num meshes

    DEBUG_PRINTF("finished vertex creation\n");
}

/*
**
*/
void outputVerticesAndTriangles(
    std::vector<Vertex> const& aTotalVertices,
    std::vector<std::vector<uint32_t>> const& aaiTriangleVertexIndices,
    std::vector<MeshExtent> const& aMeshExtents,
    std::string const& directory,
    std::string const& baseName)
{
    std::string fullPath = directory + "\\" + baseName + "-triangles.bin";

    uint32_t iNumMeshes = (uint32_t)aaiTriangleVertexIndices.size();
    std::vector<MeshRange> aMeshTriangleRanges(iNumMeshes);
    uint32_t iCurrStart = 0;
    for(uint32_t i = 0; i < iNumMeshes; i++)
    {
        MeshRange range;
        range.miStart = iCurrStart;
        iCurrStart += (uint32_t)aaiTriangleVertexIndices[i].size();
        range.miEnd = iCurrStart;
        aMeshTriangleRanges[i] = range;
    }
    uint32_t iTriangleRangeSize = aMeshTriangleRanges.size() * sizeof(MeshRange);

    uint32_t iNumTotalVertices = aTotalVertices.size();
    uint32_t iVertexSize = (uint32_t)sizeof(Vertex);

    uint32_t iNumTotalTriangles = 0;
    for(auto const& aiTriangleVertexIndices : aaiTriangleVertexIndices)
    {
        assert(aiTriangleVertexIndices.size() % 3 == 0);
        uint32_t iNumTriangles = aiTriangleVertexIndices.size() / 3;
        iNumTotalTriangles += iNumTriangles;
    }

    uint32_t iTriangleStartOffset = iNumTotalVertices * iVertexSize + iTriangleRangeSize + sizeof(uint32_t) * 5 + iNumMeshes * sizeof(MeshExtent);

    FILE* fp = fopen(fullPath.c_str(), "wb");
    fwrite(&iNumMeshes, sizeof(uint32_t), 1, fp);
    fwrite(&iNumTotalVertices, sizeof(uint32_t), 1, fp);
    fwrite(&iNumTotalTriangles, sizeof(uint32_t), 1, fp);
    fwrite(&iVertexSize, sizeof(uint32_t), 1, fp);
    fwrite(&iTriangleStartOffset, sizeof(uint32_t), 1, fp);
    fwrite(aMeshTriangleRanges.data(), sizeof(MeshRange), aMeshTriangleRanges.size(), fp);

    // mesh extents
    assert(aMeshExtents.size() == iNumMeshes);
    fwrite(aMeshExtents.data(), sizeof(MeshExtent), iNumMeshes, fp);

    // vertices and triangle indices
    assert(aaiTriangleVertexIndices.size() == iNumMeshes);
    fwrite(aTotalVertices.data(), sizeof(Vertex), aTotalVertices.size(), fp);

    //std::vector<uint32_t> aiTotalTriangleVertexIndices;
    //for(uint32_t i = 0; i < aaiTriangleVertexIndices.size(); i++)
    //{
    //    aiTotalTriangleVertexIndices.insert(aiTotalTriangleVertexIndices.end(), aaiTriangleVertexIndices[i].begin(), aaiTriangleVertexIndices[i].end());
    //}
    //fwrite(aiTotalTriangleVertexIndices.data(), sizeof(uint32_t), aiTotalTriangleVertexIndices.size(), fp);

    for(uint32_t i = 0; i < aaiTriangleVertexIndices.size(); i++)
    {
        fwrite(aaiTriangleVertexIndices[i].data(), sizeof(uint32_t), aaiTriangleVertexIndices[i].size(), fp);
    }

    fclose(fp);

    DEBUG_PRINTF("wrote to %s\n", fullPath.c_str());
}

/*
**
*/
void test(
    std::vector<Vertex>& aTotalVertices,
    std::vector<std::vector<uint32_t>>& aaiTriangleVertexIndices,
    std::vector<MeshRange>& aMeshRanges,
    std::vector<MeshExtent>& aMeshExtents,
    std::string const& fullPath)
{
    uint32_t iNumMeshes = 0;
    uint32_t iNumTotalVertices = 0;
    uint32_t iNumTotalTriangles = 0;
    uint32_t iVertexSize = 0;
    uint32_t iTriangleStartOffset = 0;

    FILE* fp = fopen(fullPath.c_str(), "rb");
    
    uint64_t iFileSize = (uint64_t)fseek(fp, 0, SEEK_END);
    fseek(fp, (long)iFileSize - 8, SEEK_SET);
    float fTest = 0.0f;
    fread(&fTest, sizeof(float), 1, fp);
    fseek(fp, 0, SEEK_SET);

    fread(&iNumMeshes, sizeof(uint32_t), 1, fp);
    fread(&iNumTotalVertices, sizeof(uint32_t), 1, fp);
    fread(&iNumTotalTriangles, sizeof(uint32_t), 1, fp);
    fread(&iVertexSize, sizeof(uint32_t), 1, fp);
    fread(&iTriangleStartOffset, sizeof(uint32_t), 1, fp);

    aMeshRanges.resize(iNumMeshes);
    fread(aMeshRanges.data(), sizeof(MeshRange), iNumMeshes, fp);

    aMeshExtents.resize(iNumMeshes);
    fread(aMeshExtents.data(), sizeof(MeshExtent), iNumMeshes, fp);

    aTotalVertices.resize(iNumTotalVertices);
    fread(aTotalVertices.data(), sizeof(Vertex), iNumTotalVertices, fp);

    for(uint32_t i = 0; i < iNumMeshes; i++)
    {
        MeshRange const& range = aMeshRanges[i];
        uint32_t iNumTriangleIndices = range.miEnd - range.miStart;
        std::vector<uint32_t> aiTriangles(iNumTriangleIndices);
        fread(aiTriangles.data(), sizeof(uint32_t), iNumTriangleIndices, fp);
        aaiTriangleVertexIndices.push_back(aiTriangles);
    }

    fclose(fp);

    std::vector<uint32_t> aiMeshes;
    for(uint32_t i = 0; i < iNumMeshes; i++)
    {
        aiMeshes.push_back(i);
    }
    std::stringstream oss;
    oss << "d:\\Downloads\\Bistro_v4\\test-output\\test-bistro-part-read-from-file.obj";
    saveOBJ(
        oss.str().c_str(),
        aTotalVertices,
        aaiTriangleVertexIndices,
        aiMeshes);
}

/*
**
*/
void saveOBJ(
    std::string const& fullPath,
    std::vector<Vertex> const& aTotalVertices,
    std::vector<std::vector<uint32_t>> const& aaiTriangleVertexIndices,
    std::vector<uint32_t> const& aiMeshes)
{
    FILE* fp = fopen(fullPath.c_str(), "wb");

    for(auto const& vertex : aTotalVertices)
    {
        fprintf(fp, "v %.4f %.4f %.4f\n",
            vertex.mPosition.x,
            vertex.mPosition.y,
            vertex.mPosition.z);
    }

    for(auto const& vertex : aTotalVertices)
    {
        fprintf(fp, "vt %.4f %.4f\n",
            vertex.mUV.x,
            vertex.mUV.y);
    }

    for(auto const& vertex : aTotalVertices)
    {
        fprintf(fp, "vn %.4f %.4f %.4f\n",
            vertex.mNormal.x,
            vertex.mNormal.y,
            vertex.mNormal.z);
    }

    for(auto const& iMesh : aiMeshes)
    {
        fprintf(fp, "o object\n");
        std::vector<uint32_t> const& aiTriangleVertexIndices = aaiTriangleVertexIndices[iMesh];
        assert(aiTriangleVertexIndices.size() % 3 == 0);
        uint32_t iNumTriangles = aiTriangleVertexIndices.size() / 3;
        for(uint32_t iTriangle = 0; iTriangle < iNumTriangles; iTriangle++)
        {
            fprintf(fp, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                aiTriangleVertexIndices[iTriangle * 3] + 1,
                aiTriangleVertexIndices[iTriangle * 3] + 1,
                aiTriangleVertexIndices[iTriangle * 3] + 1,
                aiTriangleVertexIndices[iTriangle * 3 + 1] + 1,
                aiTriangleVertexIndices[iTriangle * 3 + 1] + 1,
                aiTriangleVertexIndices[iTriangle * 3 + 1] + 1,
                aiTriangleVertexIndices[iTriangle * 3 + 2] + 1,
                aiTriangleVertexIndices[iTriangle * 3 + 2] + 1,
                aiTriangleVertexIndices[iTriangle * 3 + 2] + 1);
        }
    }

    fclose(fp);
}