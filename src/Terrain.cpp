#include <mutex>

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
        float half = Terrain::scale * static_cast<float>(i) / 2;
        cubeObjects.emplace_back(nullptr, new btBoxShape(btVector3(half, half, half)), glm::vec3(0.f, 0.f, 0.f));
    }
}

Terrain::Terrain(World &world) : world(world)
{
    //TODO reading from vector
    for (int y = 0; y < 5 * chunkSize; y += chunkSize)
        for (int x = 0; x < 10 * chunkSize; x += chunkSize)
            for (int z = 0; z < 10 * chunkSize; z += chunkSize)
                chunks.emplace(getVecInt3(x, y, z), new Chunk({x, y, z}, this, true));
    chunks.emplace(getVecInt3(0, 5 * chunkSize, 0), new Chunk({0, 5 * chunkSize, 0}, this, true));

    updateBuffers();
    waitForThreads();
    loadBuffers();
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
        threads.emplace_back([&]() {
            std::lock_guard<std::mutex> lock(mutex);
            for (auto& i : toRegen)
            {
                auto it = chunks.find(i);
                if (it == chunks.end())
                    it = chunks.emplace(i, new Chunk(i, (Terrain *)this, false)).first;
                it->second->prepareVertexBuffer();
                toReload.insert(i);
            }
            for (auto& i : toRegen)
                chunks[i]->prepareBuffers();
            toRegen = std::set<VecInt3>();
        });
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

void Terrain::OctreeNode::collideWith(CollisionObject *collObj)
{
    if (areCornersIn(collObj))
    {
        toDelete = true;
        return;
    }

    float fHalf = float(size) / 2;

    std::unique_ptr<btBoxShape> boxShape(new btBoxShape(Terrain::scale * btVector3(fHalf, fHalf, fHalf)));
    GhostObject fromscratch(nullptr, boxShape.get(), center);
    if (chunk->terrain->privateWorld.areColliding(fromscratch, *collObj))
    {
        if (whole)
        {
            whole = false;
            addPoints(-1);
            makeChilds(collObj);
        }
        else
        {
            for (auto &i : childs)
                if (i)
                {
                    i->collideWith(collObj);
                    if (i->toDelete)
                    {
                        childCount--;
                        if (childCount == 0)
                            toDelete = true;
                        i.reset(nullptr);
                    }
                }
        }
    }
}

bool Terrain::OctreeNode::areCornersIn(CollisionObject *collObj)
{
    for (size_t i = 0; i < 8; i++)
        if (!chunk->terrain->privateWorld.pointInShape(*collObj, glmPos + Terrain::scale * float(size - 1) * VecInt3ToVec3(cubeVer[i])))
            return false;
    return true;
}

void Terrain::OctreeNode::genBuffers()
{
    if (!toDelete)
    {
        if (whole)
            chunk->verticesAtPos(size, intPos);
        else
        {
            for (auto &i : childs)
                if (i)
                    i->genBuffers();
        }
    }
}

void Terrain::OctreeNode::genNormalBuffer()
{
    if (!toDelete)
    {
        if (whole)
            chunk->normalsAtPos(size, intPos);
        else
        {
            for (auto &i : childs)
                if (i)
                    i->genNormalBuffer();
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

void Terrain::Chunk::prepareVertexBuffer()
{
    vertices.clear();
    normals.clear();
    blockyNormals.clear();
    texCoords.clear();

    newTriangleMesh.reset(new btTriangleMesh());

    for (auto& i : normalsToDelete)
        terrain->pointNormals[i.first] -= i.second;
    normalsToDelete.clear();

    pointsTouched.clear();

    verticesAtPos(chunkSize - 1, intPos + getVecInt3(1, 1, 1));

    if (rootTerrainCube)
        rootTerrainCube->genBuffers();
}

namespace helper {
struct vec3Ref
{
    vec3Ref(float &xRef, float &yRef, float &zRef)
        : x(xRef), y(yRef), z(zRef)
    {
        assert(&xRef + sizeof(float) == &yRef);
        assert(&yRef + sizeof(float) == &zRef);
    }
    float &x;
    float &y;
    float &z;
    operator glm::vec3 &() const
    {
        return *reinterpret_cast<glm::vec3 *>(&x);
    }
    vec3Ref &operator =(const glm::vec3 &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    vec3Ref &operator =(glm::vec3 &&v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
};

template <class A, class N, class T, class B>
void computeTangentAndBitangent(const A &a, const N &normal, T &tangent, B &bitangent)
{
    if (normal.y == 0.f)
    {
        tangent.x = normal.z;
        tangent.y = -normal.x;
        tangent.z = 0.f;
    }
    else
    {
        // tangent = {10000, 0, 0} - a
        tangent.x = 10000.f - a.x;
        tangent.z = -a.z;
        tangent.y = (-normal.z * (tangent.z - a.z) - normal.x * (tangent.x - a.x)) / normal.y + a.y;
        tangent = glm::normalize(static_cast<glm::vec3 &>(tangent));
    }

    bitangent = VertexArray::computeBitangent(tangent, normal);
}
};

void Terrain::Chunk::prepareBuffers()
{
    std::thread thread([this]() {
        if (newTriangleMesh->getNumTriangles() != 0)
        {
            newShape.reset(new btBvhTriangleMeshShape(newTriangleMesh.get(), true));
            newShape->setMargin(0.f);
        }
        else
            newShape.reset(nullptr);
    });

    pointsTouched.clear();

    normalsAtPos(chunkSize - 1, intPos + getVecInt3(1, 1, 1));

    if (rootTerrainCube)
        rootTerrainCube->genNormalBuffer();

    assert(vertices.size() == normals.size());
    assert(newTriangleMesh->getNumTriangles() == (int) vertices.size() / 9);

    tangents.resize(vertices.size());
    bitangents.resize(vertices.size());
    texCoords.resize(vertices.size() / 3 * 2);

    #pragma omp parallel for
    for (int i = 0; i < (int)texCoords.size() / 6; i++)
    {
        using namespace helper;
        vec3Ref a(vertices[9 * i], vertices[9 * i + 1], vertices[9 * i + 2]);
        vec3Ref b(vertices[9 * i + 3], vertices[9 * i + 4], vertices[9 * i + 5]);
        vec3Ref c(vertices[9 * i + 6], vertices[9 * i + 7], vertices[9 * i + 8]);

        vec3Ref blockyNormal(blockyNormals[9 * i], blockyNormals[9 * i + 1], blockyNormals[9 * i + 2]);
        glm::vec3 blockyTangent, blockyBitangent;

        computeTangentAndBitangent(a, blockyNormal, blockyTangent, blockyBitangent);

        glm::mat3 tbn(glm::transpose(glm::mat3(blockyTangent, blockyNormal, blockyBitangent)));

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

        vec3Ref normalA(normals[9 * i], normals[9 * i + 1], normals[9 * i + 2]);
        vec3Ref normalB(normals[9 * i + 3], normals[9 * i + 4], normals[9 * i + 5]);
        vec3Ref normalC(normals[9 * i + 6], normals[9 * i + 7], normals[9 * i + 8]);

        vec3Ref tangentA(tangents[9 * i], tangents[9 * i + 1], tangents[9 * i + 2]);
        vec3Ref tangentB(tangents[9 * i + 3], tangents[9 * i + 4], tangents[9 * i + 5]);
        vec3Ref tangentC(tangents[9 * i + 6], tangents[9 * i + 7], tangents[9 * i + 8]);

        vec3Ref bitangentA(bitangents[9 * i], bitangents[9 * i + 1], bitangents[9 * i + 2]);
        vec3Ref bitangentB(bitangents[9 * i + 3], bitangents[9 * i + 4], bitangents[9 * i + 5]);
        vec3Ref bitangentC(bitangents[9 * i + 6], bitangents[9 * i + 7], bitangents[9 * i + 8]);

        computeTangentAndBitangent(a, normalA, tangentA, bitangentA);
        computeTangentAndBitangent(b, normalB, tangentB, bitangentB);
        computeTangentAndBitangent(c, normalC, tangentC, bitangentC);
    }

    thread.join();
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
