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
#include <glm/gtx/closest_point.hpp>


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
            normals.clear();
            rigidBody.reset();
            shape.reset();
            triangleMesh.reset(new btTriangleMesh());

            drawAtPos(chunkSize - 1, intPos + getVecInt3(1, 1, 1));
            if (rootTerrainCube)
                rootTerrainCube->genBuffers();

            texCoords.resize(vertices.size() / 3 * 2);
            tangents.resize(vertices.size());
            bitangents.resize(vertices.size());

            for (int i = 0; i < texCoords.size() / 6; i++)
            {
                glm::vec3 a(vertices[9 * i], vertices[9 * i + 1], vertices[9 * i + 2]);
                glm::vec3 b(vertices[9 * i + 3], vertices[9 * i + 4], vertices[9 * i + 5]);
                glm::vec3 c(vertices[9 * i + 6], vertices[9 * i + 7], vertices[9 * i + 8]);

                glm::vec3 minVec(glm::min(a.x, glm::min(b.x, c.x)), glm::min(a.y, glm::min(b.y, c.y)), glm::min(a.z, glm::min(b.z, c.z)));
                glm::vec3 maxVec(glm::max(a.x, glm::max(b.x, c.x)), glm::max(a.y, glm::max(b.y, c.y)), glm::max(a.z, glm::max(b.z, c.z)));
                glm::vec3 distances(glm::distance(minVec.x, maxVec.x), glm::distance(minVec.y, maxVec.y), glm::distance(minVec.z, maxVec.z));
                float maxDistance(glm::max(distances.x, glm::max(distances.y, distances.z)));

                glm::vec3 e1(b - a);
                glm::vec3 e2(c - a);

                if (a.y == b.y && a.y == c.y)
                {
                    texCoords[6 * i] = (a.x - minVec.x) / maxDistance;
                    texCoords[6 * i + 1] = (a.z - minVec.z) / maxDistance;

                    texCoords[6 * i + 2] = (b.x - minVec.x) / maxDistance;
                    texCoords[6 * i + 3] = (b.z - minVec.z) / maxDistance;

                    texCoords[6 * i + 4] = (c.x - minVec.x) / maxDistance;
                    texCoords[6 * i + 5] = (c.z - minVec.z) / maxDistance;

                    glm::vec2 t1(glm::vec2(texCoords[6 * i + 2], texCoords[6 * i + 3]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));
	                glm::vec2 t2(glm::vec2(texCoords[6 * i + 4], texCoords[6 * i + 5]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));

                    glm::vec3 tangent(VertexArray::computeTangent(e1, e2, t1, t2));

                    glm::vec3 bitangentA(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i], normals[9 * i + 1], normals[9 * i + 2])));
                    glm::vec3 bitangentB(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 3], normals[9 * i + 4], normals[9 * i + 5])));
                    glm::vec3 bitangentC(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 6], normals[9 * i + 7], normals[9 * i + 8])));

                    tangents[9 * i] = tangent.x, tangents[9 * i + 1] = tangent.y, tangents[9 * i + 2] = tangent.z;
                    tangents[9 * i + 3] = tangent.x, tangents[9 * i + 4] = tangent.y, tangents[9 * i + 5] = tangent.z;
                    tangents[9 * i + 6] = tangent.x, tangents[9 * i + 7] = tangent.y, tangents[9 * i + 8] = tangent.z;

                    bitangents[9 * i] = bitangentA.x, bitangents[9 * i + 1] = bitangentA.y, bitangents[9 * i + 2] = bitangentA.z;
                    bitangents[9 * i + 3] = bitangentB.x, bitangents[9 * i + 4] = bitangentB.y, bitangents[9 * i + 5] = bitangentB.z;
                    bitangents[9 * i + 6] = bitangentC.x, bitangents[9 * i + 7] = bitangentC.y, bitangents[9 * i + 8] = bitangentC.z;
                }
                else if (a.x == b.x && a.x == c.x)
                {
                    texCoords[6 * i] = (a.y - minVec.y) / maxDistance;
                    texCoords[6 * i + 1] = (a.z - minVec.z) / maxDistance;

                    texCoords[6 * i + 2] = (b.y - minVec.y) / maxDistance;
                    texCoords[6 * i + 3] = (b.z - minVec.z) / maxDistance;

                    texCoords[6 * i + 4] = (c.y - minVec.y) / maxDistance;
                    texCoords[6 * i + 5] = (c.z - minVec.z) / maxDistance;

                    glm::vec2 t1(glm::vec2(texCoords[6 * i + 2], texCoords[6 * i + 3]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));
	                glm::vec2 t2(glm::vec2(texCoords[6 * i + 4], texCoords[6 * i + 5]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));

                    glm::vec3 tangent(VertexArray::computeTangent(e1, e2, t1, t2));

                    glm::vec3 bitangentA(VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i], normals[9 * i + 1], normals[9 * i + 2])));
                    glm::vec3 bitangentB(VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 3], normals[9 * i + 4], normals[9 * i + 5])));
                    glm::vec3 bitangentC(VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 6], normals[9 * i + 7], normals[9 * i + 8])));

                    tangents[9 * i] = tangent.x, tangents[9 * i + 1] = tangent.y, tangents[9 * i + 2] = tangent.z;
                    tangents[9 * i + 3] = tangent.x, tangents[9 * i + 4] = tangent.y, tangents[9 * i + 5] = tangent.z;
                    tangents[9 * i + 6] = tangent.x, tangents[9 * i + 7] = tangent.y, tangents[9 * i + 8] = tangent.z;

                    bitangents[9 * i] = bitangentA.x, bitangents[9 * i + 1] = bitangentA.y, bitangents[9 * i + 2] = bitangentA.z;
                    bitangents[9 * i + 3] = bitangentB.x, bitangents[9 * i + 4] = bitangentB.y, bitangents[9 * i + 5] = bitangentB.z;
                    bitangents[9 * i + 6] = bitangentC.x, bitangents[9 * i + 7] = bitangentC.y, bitangents[9 * i + 8] = bitangentC.z;
                }
                else if (a.z == b.z && a.z == c.z)
                {
                    texCoords[6 * i] = (a.x - minVec.x) / maxDistance;
                    texCoords[6 * i + 1] = -(a.y - minVec.y) / maxDistance;

                    texCoords[6 * i + 2] = (b.x - minVec.x) / maxDistance;
                    texCoords[6 * i + 3] = -(b.y - minVec.y) / maxDistance;

                    texCoords[6 * i + 4] = (c.x - minVec.x) / maxDistance;
                    texCoords[6 * i + 5] = -(c.y - minVec.y) / maxDistance;

                    glm::vec2 t1(glm::vec2(texCoords[6 * i + 2], texCoords[6 * i + 3]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));
	                glm::vec2 t2(glm::vec2(texCoords[6 * i + 4], texCoords[6 * i + 5]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));

                    glm::vec3 tangent(VertexArray::computeTangent(e1, e2, t1, t2));

                    glm::vec3 bitangentA(VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i], normals[9 * i + 1], normals[9 * i + 2])));
                    glm::vec3 bitangentB(VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 3], normals[9 * i + 4], normals[9 * i + 5])));
                    glm::vec3 bitangentC(VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 6], normals[9 * i + 7], normals[9 * i + 8])));

                    tangents[9 * i] = tangent.x, tangents[9 * i + 1] = tangent.y, tangents[9 * i + 2] = tangent.z;
                    tangents[9 * i + 3] = tangent.x, tangents[9 * i + 4] = tangent.y, tangents[9 * i + 5] = tangent.z;
                    tangents[9 * i + 6] = tangent.x, tangents[9 * i + 7] = tangent.y, tangents[9 * i + 8] = tangent.z;

                    bitangents[9 * i] = bitangentA.x, bitangents[9 * i + 1] = bitangentA.y, bitangents[9 * i + 2] = bitangentA.z;
                    bitangents[9 * i + 3] = bitangentB.x, bitangents[9 * i + 4] = bitangentB.y, bitangents[9 * i + 5] = bitangentB.z;
                    bitangents[9 * i + 6] = bitangentC.x, bitangents[9 * i + 7] = bitangentC.y, bitangents[9 * i + 8] = bitangentC.z;
                }
                else if (a.y == b.y)
                {
                    glm::vec3 x = glm::closestPointOnLine(c, a, b);
                    float d = glm::distance(a, b);
                    float h = glm::distance(x, c);
                    if(c.y < a.y)
                    {
                        float l = glm::distance(x, b);
                        maxDistance = glm::max(d, glm::max(h, l));

                        texCoords[6 * i] = 0.f;
                        texCoords[6 * i + 1] = d / maxDistance;

                        texCoords[6 * i + 2] = 0.f;
                        texCoords[6 * i + 3] = 0.f;

                        texCoords[6 * i + 4] = h / maxDistance;
                        texCoords[6 * i + 5] = l / maxDistance;
                    }
                    else
                    {
                        float l = glm::distance(x, a);

                        texCoords[6 * i] = h / maxDistance;
                        texCoords[6 * i + 1] = 0.f;

                        texCoords[6 * i + 2] = h / maxDistance;
                        texCoords[6 * i + 3] = d / maxDistance;

                        texCoords[6 * i + 4] = 0.f;
                        texCoords[6 * i + 5] = l / maxDistance;
                    }

                    glm::vec2 t1(glm::vec2(texCoords[6 * i + 2], texCoords[6 * i + 3]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));
	                glm::vec2 t2(glm::vec2(texCoords[6 * i + 4], texCoords[6 * i + 5]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));

                    glm::vec3 tangent(VertexArray::computeTangent(e1, e2, t1, t2));

                    glm::vec3 bitangentA(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i], normals[9 * i + 1], normals[9 * i + 2])));
                    glm::vec3 bitangentB(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 3], normals[9 * i + 4], normals[9 * i + 5])));
                    glm::vec3 bitangentC(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 6], normals[9 * i + 7], normals[9 * i + 8])));

                    tangents[9 * i] = tangent.x, tangents[9 * i + 1] = tangent.y, tangents[9 * i + 2] = tangent.z;
                    tangents[9 * i + 3] = tangent.x, tangents[9 * i + 4] = tangent.y, tangents[9 * i + 5] = tangent.z;
                    tangents[9 * i + 6] = tangent.x, tangents[9 * i + 7] = tangent.y, tangents[9 * i + 8] = tangent.z;

                    bitangents[9 * i] = bitangentA.x, bitangents[9 * i + 1] = bitangentA.y, bitangents[9 * i + 2] = bitangentA.z;
                    bitangents[9 * i + 3] = bitangentB.x, bitangents[9 * i + 4] = bitangentB.y, bitangents[9 * i + 5] = bitangentB.z;
                    bitangents[9 * i + 6] = bitangentC.x, bitangents[9 * i + 7] = bitangentC.y, bitangents[9 * i + 8] = bitangentC.z;
                    
                }
                else if (a.y == c.y)
                {
                    glm::vec3 x = glm::closestPointOnLine(b, a, c);
                    float d = glm::distance(a, c);
                    float h = glm::distance(x, b);
                    if(b.y < a.y)
                    {
                        float l = glm::distance(x, a);
                        maxDistance = glm::max(d, glm::max(h, l));

                        texCoords[6 * i] = 0.f;
                        texCoords[6 * i + 1] = 0.f;

                        texCoords[6 * i + 2] = h / maxDistance; 
                        texCoords[6 * i + 3] = l / maxDistance; 

                        texCoords[6 * i + 4] = 0.f;
                        texCoords[6 * i + 5] = d / maxDistance;
                    }
                    else
                    {
                        float l = glm::distance(x, c);

                        texCoords[6 * i] = h / maxDistance;
                        texCoords[6 * i + 1] = d / maxDistance;

                        texCoords[6 * i + 2] = 0.f;
                        texCoords[6 * i + 3] = l / maxDistance;

                        texCoords[6 * i + 4] = h / maxDistance;
                        texCoords[6 * i + 5] = 0.f;
                    }

                    glm::vec2 t1(glm::vec2(texCoords[6 * i + 2], texCoords[6 * i + 3]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));
	                glm::vec2 t2(glm::vec2(texCoords[6 * i + 4], texCoords[6 * i + 5]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));

                    glm::vec3 tangent(VertexArray::computeTangent(e1, e2, t1, t2));

                    glm::vec3 bitangentA(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i], normals[9 * i + 1], normals[9 * i + 2])));
                    glm::vec3 bitangentB(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 3], normals[9 * i + 4], normals[9 * i + 5])));
                    glm::vec3 bitangentC(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 6], normals[9 * i + 7], normals[9 * i + 8])));

                    tangents[9 * i] = tangent.x, tangents[9 * i + 1] = tangent.y, tangents[9 * i + 2] = tangent.z;
                    tangents[9 * i + 3] = tangent.x, tangents[9 * i + 4] = tangent.y, tangents[9 * i + 5] = tangent.z;
                    tangents[9 * i + 6] = tangent.x, tangents[9 * i + 7] = tangent.y, tangents[9 * i + 8] = tangent.z;

                    bitangents[9 * i] = bitangentA.x, bitangents[9 * i + 1] = bitangentA.y, bitangents[9 * i + 2] = bitangentA.z;
                    bitangents[9 * i + 3] = bitangentB.x, bitangents[9 * i + 4] = bitangentB.y, bitangents[9 * i + 5] = bitangentB.z;
                    bitangents[9 * i + 6] = bitangentC.x, bitangents[9 * i + 7] = bitangentC.y, bitangents[9 * i + 8] = bitangentC.z;
                }
                else if (b.y == c.y)
                {
                    glm::vec3 x = glm::closestPointOnLine(c, b, c);
                    float d = glm::distance(b, c);
                    float h = glm::distance(x, a);
                    if(a.y < b.y)
                    {
                        float l = glm::distance(x, c);
                        maxDistance = glm::max(d, glm::max(h, l));

                        texCoords[6 * i] = h / maxDistance;
                        texCoords[6 * i + 1] = l / maxDistance;

                        texCoords[6 * i + 2] = 0.f;
                        texCoords[6 * i + 3] = d / maxDistance;

                        texCoords[6 * i + 4] = 0.f;
                        texCoords[6 * i + 5] = 0.f;
                    }
                    else
                    {
                        float l = glm::distance(x, b);

                        texCoords[6 * i] = 0.f;
                        texCoords[6 * i + 1] = l / maxDistance;

                        texCoords[6 * i + 2] = h / maxDistance;
                        texCoords[6 * i + 3] = 0.f;

                        texCoords[6 * i + 4] = h / maxDistance;
                        texCoords[6 * i + 5] = d / maxDistance;
                    }

                    glm::vec2 t1(glm::vec2(texCoords[6 * i + 2], texCoords[6 * i + 3]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));
	                glm::vec2 t2(glm::vec2(texCoords[6 * i + 4], texCoords[6 * i + 5]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));

                    glm::vec3 tangent(VertexArray::computeTangent(e1, e2, t1, t2));

                    glm::vec3 bitangentA(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i], normals[9 * i + 1], normals[9 * i + 2])));
                    glm::vec3 bitangentB(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 3], normals[9 * i + 4], normals[9 * i + 5])));
                    glm::vec3 bitangentC(-VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 6], normals[9 * i + 7], normals[9 * i + 8])));

                    tangents[9 * i] = tangent.x, tangents[9 * i + 1] = tangent.y, tangents[9 * i + 2] = tangent.z;
                    tangents[9 * i + 3] = tangent.x, tangents[9 * i + 4] = tangent.y, tangents[9 * i + 5] = tangent.z;
                    tangents[9 * i + 6] = tangent.x, tangents[9 * i + 7] = tangent.y, tangents[9 * i + 8] = tangent.z;

                    bitangents[9 * i] = bitangentA.x, bitangents[9 * i + 1] = bitangentA.y, bitangents[9 * i + 2] = bitangentA.z;
                    bitangents[9 * i + 3] = bitangentB.x, bitangents[9 * i + 4] = bitangentB.y, bitangents[9 * i + 5] = bitangentB.z;
                    bitangents[9 * i + 6] = bitangentC.x, bitangents[9 * i + 7] = bitangentC.y, bitangents[9 * i + 8] = bitangentC.z;
                    
                }
                else
                {
                    glm::vec3 ba = b - a;
                    glm::vec3 ca = c - a;
                    float angle = glm::abs(glm::acos(glm::dot(glm::normalize(ba), glm::normalize(ca))));

                    texCoords[6 * i] = 0;
                    texCoords[6 * i + 1] = 0;

                    texCoords[6 * i + 2] = glm::length(ba);
                    texCoords[6 * i + 3] = 0;

                    float caLength = glm::length(ca);
                    texCoords[6 * i + 4] = glm::cos(angle) * caLength;
                    texCoords[6 * i + 5] = glm::sin(angle) * caLength;

                    float maxLength = glm::max(texCoords[6 * i + 2], glm::max(texCoords[6 * i + 4], texCoords[6 * i + 5]));

                    texCoords[6 * i + 2] /= maxLength;
                    texCoords[6 * i + 4] /= maxLength;
                    texCoords[6 * i + 5] /= maxLength;

                    glm::vec2 t1(glm::vec2(texCoords[6 * i + 2], texCoords[6 * i + 3]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));
	                glm::vec2 t2(glm::vec2(texCoords[6 * i + 4], texCoords[6 * i + 5]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));

                    glm::vec3 tangent(VertexArray::computeTangent(e1, e2, t1, t2));

                    glm::vec3 bitangentA(VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i], normals[9 * i + 1], normals[9 * i + 2])));
                    glm::vec3 bitangentB(VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 3], normals[9 * i + 4], normals[9 * i + 5])));
                    glm::vec3 bitangentC(VertexArray::computeBitangent(tangent, glm::vec3(normals[9 * i + 6], normals[9 * i + 7], normals[9 * i + 8])));

                    tangents[9 * i] = tangent.x, tangents[9 * i + 1] = tangent.y, tangents[9 * i + 2] = tangent.z;
                    tangents[9 * i + 3] = tangent.x, tangents[9 * i + 4] = tangent.y, tangents[9 * i + 5] = tangent.z;
                    tangents[9 * i + 6] = tangent.x, tangents[9 * i + 7] = tangent.y, tangents[9 * i + 8] = tangent.z;

                    bitangents[9 * i] = bitangentA.x, bitangents[9 * i + 1] = bitangentA.y, bitangents[9 * i + 2] = bitangentA.z;
                    bitangents[9 * i + 3] = bitangentB.x, bitangents[9 * i + 4] = bitangentB.y, bitangents[9 * i + 5] = bitangentB.z;
                    bitangents[9 * i + 6] = bitangentC.x, bitangents[9 * i + 7] = bitangentC.y, bitangents[9 * i + 8] = bitangentC.z;
                }
            }
            
            if (!vertices.empty())
            {
                shape.reset(new btBvhTriangleMeshShape(triangleMesh.get(), true));
                rigidBody.reset(new RigidBody(&(terrain->world), shape.get(), 0, glm::vec3(0.f, 0.f, 0.f)));
            }

            vertexArray.load(vertices, normals, texCoords, tangents, bitangents);
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
        std::vector<float> normals;
        std::vector<float> texCoords;
        std::vector<float> tangents;
        std::vector<float> bitangents;


        const VertexArray& getVertexArray() const override { return vertexArray; }

        VertexArray vertexArray;
        friend class TerrainCube;
        friend class Terrain;

        static void init()
        {
            texture0.load("Slime_DIFF.png");
            texture1.load("Slime_SPEC.png");
            texture2.load("Slime_NRM.png");
            texture3.load("Slime_OCC.png");
        }
        static inline Texture texture0, texture1, texture2, texture3;
    protected:
        const ShaderProgram &getShaderProgram() const override { return defaultTextureShaderProgram; }
        const Texture &getTexture0() const override { return texture0; }
        const Texture &getTexture1() const override { return texture1; }
        const Texture &getTexture2() const override { return texture2; }
        const Texture &getTexture3() const override { return texture3; }

    };

    World& world;
    World privateWorld;

    std::set<VecInt3> toRegen;
    std::set<VecInt3> marchingPoints;
    static VecInt3 getChunkFromPoint(VecInt3 pos);
    std::map<VecInt3, std::unique_ptr<TerrainChunk>> chunks;
};

#endif