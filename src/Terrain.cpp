
#include "Terrain.h"
#include "BulletCollision/CollisionDispatch/btCollisionObjectWrapper.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/NarrowPhaseCollision/btManifoldPoint.h"
#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"
#include "BulletCollision/CollisionDispatch/btManifoldResult.h"
#include "utils.h"


//TODO move this somewhere, there can be only one callback for all types of objects
// but nothing except terrain needs custom callback atm

bool CustomMaterialCombinerCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, 
    const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
{
    // btAdjustInternalEdgeContacts(cp, colObj1Wrap, colObj0Wrap, partId1, index1);

    if (colObj1Wrap->getCollisionObject()->getCollisionShape()->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
    {
        CollisionObject::CollisionObjectOwner* ownerPtr = static_cast<CollisionObject*>(colObj1Wrap->getCollisionObject()->getUserPointer())->getOwnerPtr();
        if (ownerPtr) 
        {
            Terrain::TerrainChunk* chunk = dynamic_cast<Terrain::TerrainChunk*>(ownerPtr);
            if (chunk)
            {
                cp.m_normalWorldOnB = toBtVec3(chunk->getPartNormal(index1));
            }
        }
    }

    // honestly I have no clue what the value returned by this callback is supposed to represent
    // it is ignored by bullet internally in this one specific case we have here
    return false;
}

void Terrain::init()
{
    gContactAddedCallback = CustomMaterialCombinerCallback;

    TerrainCube::init();
    TerrainChunk::init();
}

void Terrain::TerrainCube::init()
{
    cubeObjects.reserve(chunkSize);
    for (int i = 1; i < chunkSize; i++)
    {
        float half = static_cast<float>(i) / 2;
        cubeObjects.emplace_back(nullptr, new btBoxShape(btVector3(half, half, half)), glm::vec3(0.f, 0.f, 0.f));
    }
}

Terrain::Terrain(World& world) : world(world)
{
    //TODO reading from vector
    for (int x = 0; x < 10 * chunkSize; x += chunkSize)
        for (int z = 0; z < 10 * chunkSize; z += chunkSize)
            chunks.emplace(getVecInt3(x, 0, z), new TerrainChunk({x, 0, z}, this, true));
    chunks.emplace(getVecInt3(0, chunkSize, 0), new TerrainChunk({0, chunkSize, 0}, this, true));
}

VecInt3 Terrain::getChunkFromPoint(VecInt3 pos)
{
    for (auto& i : pos)
    {
        if (i < 0)
            i -= chunkSize - 1;
        i /= chunkSize;
        i *= chunkSize;
    }
    return pos;
}

void Terrain::draw(Window *window) const
{
    for (auto& i : chunks)
        i.second->draw(window);
}

void Terrain::updateBuffers()
{
    for (auto i : toRegen)
    {
        auto it = chunks.find(i);
        if (it == chunks.end())
            it = chunks.emplace(i, new TerrainChunk(i, (Terrain*)this, false)).first;
        it->second->updateBuffers();
    }
    toRegen.clear();
}

void Terrain::TerrainCube::addPoints(int d)
{
    if (size == 1)
        chunk->terrain->marchingPointsAdd(intPos, d);
    else
    {
        for (int x = 0; x < size; x++)
            for (int y = 0; y < size; y++)
            {
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(x, y, 0), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(x, y, size - 1), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(x, y, 1), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(x, y, size - 2), d);
            }
        for (int z = 1; z < size - 1; z++)
        {
            for (int other = 0; other < size - 1; other++)
            {
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(other + 1, 0, z), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(other + 1, 1, z), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(other, size - 1, z), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(other, size - 2, z), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(0, other, z), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(1, other, z), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(size - 1, other + 1, z), d);
                chunk->terrain->marchingPointsAdd(intPos + getVecInt3(size - 2, other + 1, z), d);
            }
        }
    }
}

void Terrain::TerrainCube::genBuffers()
{
    if (whole)
        chunk->drawAtPos(size, intPos);
    else
    {
        for (auto& i : childs)
        {
            if (i)
            {
                if (i->childCount == 0)
                    i.reset();
                else
                    i->genBuffers();
            }
        }
    }
}

void Terrain::TerrainChunk::collideWith(CollisionObject* collObj)
{
    if (rootTerrainCube)
    {
        rootTerrainCube->collideWith(collObj);
        if (rootTerrainCube->empty())
        {
            rootTerrainCube.reset(nullptr);
            ghostObject.reset(nullptr);
        }
    }
}

void Terrain::TerrainChunk::updateBuffers()
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

    for (size_t i = 0; i < texCoords.size() / 6; i++)
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
        shape->setMargin(0.1f);
        // btTriangleInfoMap* triangleInfoMap = new btTriangleInfoMap();
        // btGenerateInternalEdgeInfo(shape.get(), triangleInfoMap);

        rigidBody.reset(new RigidBody(&(terrain->world), shape.get(), 0, glm::vec3(0.f, 0.f, 0.f)));
        rigidBody->setOwnerPtr(this);
        rigidBody->getRawBtCollisionObjPtr()->setCollisionFlags(rigidBody->getRawBtCollisionObjPtr()->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    }

    vertexArray.load(vertices, normals, texCoords, tangents, bitangents);
}