#ifndef TERRAIN_H_
#define TERRAIN_H_

#include "debug.h"
#include "DrawableObject.h"
#include "GhostObject.h"
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

    static void init();

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

    void collideWith(CollisionObject& collObj)
    {
        auto v = privateWorld.getColliding(collObj);
        for (auto i : v)
        {
            TerrainChunk* tc = static_cast<TerrainChunk*>(i->getUserPtr());
            tc->collideWith(&collObj);
        }
    }

private:
    inline static const int chunkSize = 8; // must be power of 2


    class TerrainChunk;
    class TerrainCube
    {
    public:
        TerrainCube(int size, VecInt3 pos, Terrain::TerrainChunk* chunkPtr, CollisionObject* collsion = nullptr)
            : size(size), intPos(pos), glmPos(VecInt3ToVec3(pos)), center(glmPos + glm::vec3(float(size - 1)/2, float(size - 1)/2, float(size - 1)/2)),
              chunk(chunkPtr)
        {
            if (collsion != nullptr)
                collideWith(collsion);
            if (whole)
            {
                addPoints(1);
            }
        }

        ~TerrainCube()
        {
            if (whole)
                addPoints(-1);
        }

        static void init();

        void genBuffers();

        void makeChilds(CollisionObject* collObj)
        {
            addPoints(-1);
            whole = false;
            if (size == 1)
                toDelete = true;
            else
            {
                int halfSize = size / 2;
                for (int i = 0; i < 8; i++)
                {
                    childs[i] = std::make_unique<TerrainCube>(halfSize, intPos + cubeVer[i] * halfSize, chunk, collObj);
                    if (childs[i]->empty())
                        childs[i].reset(nullptr);
                }
            }
        }

        void collideWith(CollisionObject* collObj)
        {
            float fhalf = float(size) / 2;
            GhostObject fromscratch(nullptr, new btBoxShape(btVector3(fhalf, fhalf, fhalf)), center);
            if (whole)
                {
                    if(chunk->terrain->privateWorld.areColliding(fromscratch, *collObj))
                    {
                        toDelete = true;
                        for (size_t i = 0; i < 8; i++)
                            if (!chunk->terrain->privateWorld.pointInShape(*collObj, glmPos + float(size - 1) * VecInt3ToVec3(cubeVer[i])))
                                // removing '!' from the line above has funny result
                            {
                                toDelete = false;
                                break;
                            }
                        if (!toDelete)
                            makeChilds(collObj);
                    }
                }
            else
                for (auto& i : childs)
                    if (i)
                    {
                        if (i->toDelete)
                        {
                            childCount--;
                            if (childCount == 0)
                                toDelete = true;
                            i.reset(nullptr);
                        }
                        else
                            i->collideWith(collObj);
                    }
        }

        bool empty()
        {
            return toDelete;
        }

    private:
        void addPoints(int d);

        inline static std::vector<GhostObject> cubeObjects;

        bool whole = true;
        bool toDelete = false;
        std::array<std::unique_ptr<TerrainCube>, 8> childs;
        int childCount = 8;
        int size;
        VecInt3 intPos;
        glm::vec3 glmPos;
        glm::vec3 center;
        TerrainChunk* chunk;
        TerrainCube* parent;
    };

    class TerrainChunk : public DrawableObject
    {
    public:
        TerrainChunk(VecInt3 intPos, Terrain* terrainPtr, bool fill = false) : terrain(terrainPtr), intPos(intPos)
        {
            if (fill) {
                rootTerrainCube = std::make_unique<TerrainCube>(chunkSize, intPos, this);
                static const glm::vec3 halfVector = glm::vec3(float(chunkSize) / 2, float(chunkSize) / 2, float(chunkSize) / 2);
                ghostObject = std::make_unique<GhostObject>(&(terrain->privateWorld), &boxShape, VecInt3ToVec3(intPos) + halfVector);
                ghostObject->setUserPtr(this);
            }
        }

        void updateBuffers()
        {
            vertices.clear();
            colors.clear();
            normals.clear();
            rigidBody.reset();
            shape.reset();
            triangleMesh.reset(new btTriangleMesh());

            drawAtPos(chunkSize - 1, intPos + getVecInt3(1, 1, 1));
            if (rootTerrainCube)
                rootTerrainCube->genBuffers();

            colors = std::vector<float>(vertices.size(), 1.f);
            texCoords = std::vector<float>(vertices.size() / 3 * 2, 0.f);
            
            if (!vertices.empty())
            {
                shape.reset(new btBvhTriangleMeshShape(triangleMesh.get(), true));
                rigidBody.reset(new RigidBody(&(terrain->world), shape.get(), 0, glm::vec3(0.f, 0.f, 0.f)));
            }

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

        void collideWith(CollisionObject* collObj);

    private:
        Terrain* terrain;

        std::unique_ptr<btTriangleMesh> triangleMesh;
        std::unique_ptr<btBvhTriangleMeshShape> shape;
        std::unique_ptr<RigidBody> rigidBody;
        std::unique_ptr<GhostObject> ghostObject;
        inline static btBoxShape boxShape = btBoxShape(btVector3((float)chunkSize - 1 / 2, (float)chunkSize - 1 / 2, (float)chunkSize - 1 / 2));

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

            const std::vector<glm::vec3>& verts = marchingCubesVertices[mask];
            for (size_t i = 0; i < verts.size(); i += 3)
                triangleMesh->addTriangle(toBtVec3(verts[i] + glmPos), toBtVec3(verts[i+1] + glmPos), toBtVec3(verts[i+2] + glmPos));
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

        const VertexArray& getVertexArray() const override { return vertexArray; }

        VertexArray vertexArray;
        friend class TerrainCube;
        friend class Terrain;
    };

    World& world;
    World privateWorld;

    std::set<VecInt3> toRegen;
    std::set<VecInt3> marchingPoints;
    static VecInt3 getChunkFromPoint(VecInt3 pos);
    std::map<VecInt3, std::unique_ptr<TerrainChunk>> chunks;
};

#endif