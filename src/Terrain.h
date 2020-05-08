#ifndef TERRAIN_H_
#define TERRAIN_H_

#include "debug.h"
#include "DrawableObject.h"
#include "marchingCubesTable.h"
#include "RigidBody.h"
#include "VecInt3.h"

#include <vector>
#include <set>
#include <iomanip>
#include <iostream>
#include <cmath>


class Terrain : public Drawable
{
public:
    Terrain(World& world);
    void draw(Window* window) const override;
    void updateBuffers();
    
    void marchingPointsAdd(VecInt3 v, int d)
    {
        if (d == 1)
        {
            marchingPoints.insert(v);
        }
        else
        {
            marchingPoints.erase(v);
        }
        for (auto i : cubeVer)
            toRegen.insert(getChunkFromPoint(v - i));
    }

private:
    class TerrainChunk;
    class TerrainCube
    {
    public:
        TerrainCube(int size, VecInt3 pos, Terrain::TerrainChunk* chunkPtr) : size(size), intPos(pos), chunk(chunkPtr)
        {
            addPoints(1);
            //TODO delete this
            if (pos == getVecInt3(0, 0, 0))
                makeChilds();
        }

        ~TerrainCube()
        {
            if (whole)
                addPoints(-1);
        }

        void genBuffers();

        void makeChilds()
        {
            addPoints(-1);
            whole = false;
            if (size == 1)
                return;
            int halfSize = size / 2;
            for (int i = 0; i < 8; i++)
                childs[i] = std::make_unique<TerrainCube>(halfSize, intPos + cubeVer[i] * halfSize, chunk);
        }

    private:
        void addPoints(int d);

        bool whole = true;
        std::array<std::unique_ptr<TerrainCube>, 8> childs;
        int childCount = 8;
        int size;
        VecInt3 intPos;
        TerrainChunk* chunk;
    };

    class TerrainChunk : public DrawableObject
    {
    public:
        TerrainChunk(VecInt3 intPos, Terrain* terrainPtr, bool fill = false) : terrain(terrainPtr), intPos(intPos)
        {
            if (fill) {
                rootTerrainCube = std::make_unique<TerrainCube>(chunkSize, intPos, this);
            }
        }

        void updateBuffers()
        {
            vertices.clear();
            colors.clear();
            normals.clear();
            rigidBodies.clear();

            drawAtPos(chunkSize - 1, intPos + getVecInt3(1, 1, 1));
            if (rootTerrainCube)
                rootTerrainCube->genBuffers();

            colors = std::vector<float>(vertices.size(), 1.f);
            texCoords = std::vector<float>(vertices.size() / 3 * 2, 0.f);
            
            vertexArray.load(vertices, colors, normals, texCoords);
        }

        void drawAtPos(int size, VecInt3 pos)
        {
            for (int x = -1; x < size; x++)
                for (int y = -1; y < size; y++)
                    {
                        getOneCube(pos + getVecInt3(x, y, -1));
                        getOneCube(pos + getVecInt3(x, y, size - 1));
                    }
            for (int z = 0; z < size - 1; z++)
                for (int other = -1; other < size - 1; other++)
                {
                    getOneCube(pos + getVecInt3(other + 1, -1, z));
                    getOneCube(pos + getVecInt3(other, size - 1, z));
                    getOneCube(pos + getVecInt3(-1, other, z));
                    getOneCube(pos + getVecInt3(size - 1, other + 1, z));
                }
        }

    private:
        Terrain* terrain;

        inline static const int chunkSize = 4;

        VecInt3 intPos = {0, 0, 0};

        void getOneCube(VecInt3 pos)
        {
            if (!inChunk(pos))
                return;
            int mask = 0;
            for (int b = 0; b < 8; b++)
                if (terrain->marchingPoints.count(pos + cubeVer[b]))
                    mask |= 1 << b;

            glm::vec3 glmPos = VecInt3ToVec3(pos);
            for (auto i : marchingCubesVertices[mask])
            {
                glm::vec3 x = i + glmPos;
                vertices.insert(vertices.end(), glm::value_ptr(x), glm::value_ptr(x) + 3);
            }
            for (auto i : marchingCubesNormals[mask])
            {
                normals.insert(normals.end(), glm::value_ptr(i), glm::value_ptr(i) + 3);
            }
            for (auto i : marchingCubesShapes[mask])
            {
                rigidBodies.emplace_back(new RigidBody(terrain->world, i, 0, glmPos));
            }
        }

        bool inChunk(VecInt3 pos)
        {
            VecInt3 diff = pos - intPos;
            for (auto i : diff)
                if (i < 0 || i >= chunkSize)
                    return false;
            return true;
        }

        std::unique_ptr<TerrainCube> rootTerrainCube;
        
        std::vector<float> vertices;
        std::vector<float> colors;
        std::vector<float> normals;
        std::vector<float> texCoords;

        std::vector<std::unique_ptr<RigidBody>> rigidBodies;

        const VertexArray& getVertexArray() const override { return vertexArray; }

        VertexArray vertexArray;
        friend class TerrainCube;
        friend class Terrain;
    };

    std::set<VecInt3> toRegen;
    std::set<VecInt3> marchingPoints;
    static VecInt3 getChunkFromPoint(VecInt3 pos);
    std::map<VecInt3, std::unique_ptr<TerrainChunk>> chunks;

    World& world;
};

#endif