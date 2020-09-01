
#include "BulletCollision/CollisionDispatch/btCollisionObjectWrapper.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/NarrowPhaseCollision/btManifoldPoint.h"
#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"
#include "BulletCollision/CollisionDispatch/btManifoldResult.h"

#include "utils.h"
#include "Terrain.h"
#include "CollisionObject.h"

void Terrain::CustomMaterialCombinerCallback(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap, int partId0, int index0,
                                    const btCollisionObjectWrapper *colObj1Wrap, int partId1, int index1)
{
    if (colObj1Wrap->getCollisionObject()->getCollisionShape()->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
    {
        CollisionObject::CollisionObjectOwner *ownerPtr = static_cast<CollisionObject *>(colObj1Wrap->getCollisionObject()->getUserPointer())->getOwnerPtr();
        if (ownerPtr)
        {
            Terrain::Chunk *chunk = dynamic_cast<Terrain::Chunk *>(ownerPtr);
            if (chunk)
            {
                glm::vec3 normal = chunk->getPartNormal(index1);
                cp.m_normalWorldOnB = toBtVec3(normal);
            }
        }
    }
}

void Terrain::init()
{
    CollisionObject::addGlobalMaterialCombinerCallback(CustomMaterialCombinerCallback);

    OctreeNode::init();
    Chunk::init();
}

void Terrain::OctreeNode::init()
{
    cubeObjects.reserve(chunkSize);
    for (int i = 1; i < chunkSize; i++)
    {
        float half = static_cast<float>(i) / 2;
        cubeObjects.emplace_back(nullptr, new btBoxShape(btVector3(half, half, half)), glm::vec3(0.f, 0.f, 0.f));
    }
}

Terrain::Terrain(World &world) : world(world)
{
    //TODO reading from vector
    for (int x = 0; x < 10 * chunkSize; x += chunkSize)
        for (int z = 0; z < 10 * chunkSize; z += chunkSize)
            chunks.emplace(getVecInt3(x, 0, z), new Chunk({x, 0, z}, this, true));
    chunks.emplace(getVecInt3(0, chunkSize, 0), new Chunk({0, chunkSize, 0}, this, true));
}

VecInt3 Terrain::getChunkFromPoint(VecInt3 pos)
{
    for (auto &i : pos)
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
    for (auto &i : chunks)
        i.second->draw(window);
}

void Terrain::updateBuffers()
{
    if (!toRegen.empty())
    {
        std::thread thread([&]() {
            std::lock_guard<std::mutex> lock(mutex);
            for (auto& i : toRegen)
            {
                auto it = chunks.find(i);
                if (it == chunks.end())
                    it = chunks.emplace(i, new Chunk(i, (Terrain *)this, false)).first;
                it->second->prepareBuffers();
                toReload.insert(i);
            }
            toRegen = std::set<VecInt3>();
        });
        thread.detach();
    }
}

void Terrain::loadBuffers()
{
    std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
    if (lock.try_lock())
    {
        for (auto& i : toReload)
        {
            auto it = chunks.find(i);
            if (it != chunks.end())
                it->second->loadBuffers();
        }
        toReload.clear();
    }
}

void Terrain::OctreeNode::addPoints(int d)
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

void Terrain::OctreeNode::genBuffers()
{
    if (whole)
        chunk->drawAtPos(size, intPos);
    else
    {
        for (auto &i : childs)
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

void Terrain::Chunk::collideWith(CollisionObject *collObj)
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

void Terrain::Chunk::prepareBuffers()
{
    vertices.clear();
    normals.clear();
    newTriangleMesh.reset(new btTriangleMesh());
    drawAtPos(chunkSize - 1, intPos + getVecInt3(1, 1, 1));
    if (rootTerrainCube)
        rootTerrainCube->genBuffers();
    texCoords.resize(vertices.size() / 3 * 2);
    tangents.resize(vertices.size());
    bitangents.resize(vertices.size());
    constexpr glm::vec3 farPoint(10000.f, 0.f, 0.f);

    #pragma omp parallel for
    for (size_t i = 0; i < texCoords.size() / 6; i++)
    {
        glm::vec3 a(vertices[9 * i], vertices[9 * i + 1], vertices[9 * i + 2]);
        glm::vec3 b(vertices[9 * i + 3], vertices[9 * i + 4], vertices[9 * i + 5]);
        glm::vec3 c(vertices[9 * i + 6], vertices[9 * i + 7], vertices[9 * i + 8]);
        glm::vec3 normal(normals[9 * i + 6], normals[9 * i + 7], normals[9 * i + 8]);
        glm::vec3 tangent;
        if (normal.y == 0.f)
        {
            tangent = glm::vec3(normal.z, -normal.x, 0.f);
        }
        else
        {
            glm::vec3 center((a.x + b.x + c.x) / 3.f, (a.y + b.y + c.y) / 3.f, (a.z + b.z + c.z) / 3.f);
            tangent = farPoint - center;
            tangent.y = (-normal.z * (tangent.z - center.z) - normal.x * (tangent.x - center.x)) / normal.y + center.y;
            tangent = glm::normalize(tangent);
        }
        glm::vec3 bitangent = VertexArray::computeBitangent(tangent, normal);
        glm::mat3 tbn(glm::transpose(glm::mat3(tangent, normal, bitangent)));
        glm::vec3 texCoord;
        texCoord = tbn * a;
        texCoords[6 * i] = texCoord.x;
        texCoords[6 * i + 1] = texCoord.z;

        texCoord = tbn * b;
        texCoords[6 * i + 2] = texCoord.x;
        texCoords[6 * i + 3] = texCoord.z;
        texCoord = tbn * c;
        texCoords[6 * i + 4] = texCoord.x;
        texCoords[6 * i + 5] = texCoord.z;
        //if (i < 10)
        //    std::cout << tbn * center << std::endl;
        tangents[9 * i] = tangent.x, tangents[9 * i + 1] = tangent.y, tangents[9 * i + 2] = tangent.z;
        tangents[9 * i + 3] = tangent.x, tangents[9 * i + 4] = tangent.y, tangents[9 * i + 5] = tangent.z;
        tangents[9 * i + 6] = tangent.x, tangents[9 * i + 7] = tangent.y, tangents[9 * i + 8] = tangent.z;
        bitangents[9 * i] = bitangent.x, bitangents[9 * i + 1] = bitangent.y, bitangents[9 * i + 2] = bitangent.z;
        bitangents[9 * i + 3] = bitangent.x, bitangents[9 * i + 4] = bitangent.y, bitangents[9 * i + 5] = bitangent.z;
        bitangents[9 * i + 6] = bitangent.x, bitangents[9 * i + 7] = bitangent.y, bitangents[9 * i + 8] = bitangent.z;
    }

    if (newTriangleMesh->getNumTriangles() != 0)
    {
        newShape.reset(new btBvhTriangleMeshShape(newTriangleMesh.get(), true));
        newShape->setMargin(0.f);
    }
    else
        newShape.reset(nullptr);
}


void Terrain::Chunk::loadBuffers()
{
    std::thread thread([this]() {

        rigidBody.reset(nullptr);

        if (newShape)
        {
            triangleMesh = std::move(newTriangleMesh);
            shape = std::move(newShape);

            rigidBody.reset(new RigidBody(&(terrain->world), shape.get(), 0, glm::vec3(0.f, 0.f, 0.f)));
            rigidBody->setOwnerPtr(this);
        }
    });

    vertexArray.loadVerticesNormalsTexCoordsTangentsBitangents(vertices, normals, texCoords, tangents, bitangents);

    thread.join();
}
