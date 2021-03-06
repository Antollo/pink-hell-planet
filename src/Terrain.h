#ifndef TERRAIN_H_
#define TERRAIN_H_

#include "debug.h"
#include "DrawableObject.h"
#include "GhostObject.h"
#include "marchingCubesTable.h"
#include "RigidBody.h"
#include "VecInt3.h"
#include "CollisionObject.h"

#include <vector>
#include <set>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <glm/gtx/closest_point.hpp>
#include <thread>

class Terrain : public Drawable
{
public:
    Terrain(World &world);
    ~Terrain()
    {
        waitForThreads();
    }

    static void init();

    void draw(Window *window) const override;

    void update()
    {
        updateBuffers();
        loadBuffers();
    }

    void forceFullUpdate()
    {
        waitForThreads();
        updateBuffers();
        waitForThreads();
        loadBuffers();
    }

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

    void collideWith(std::unique_ptr<CollisionObject> collObj)
    {
        threads.emplace_back([this, collObj{std::move(collObj)}] {
            std::lock_guard<std::mutex> lock(mutex);
            auto v = privateWorld.getColliding(*collObj);
            for (auto i : v)
            {
                Chunk *tc = static_cast<Chunk *>(i->getOwnerPtr());
                tc->collideWith(collObj.get());
            }
        });
    }

    void waitForThreads()
    {
        for (auto& i : threads)
            i.join();
        threads.clear();
    }

private:
    inline static const int chunkSize = 16; // must be power of 2
    inline static const float scale = 0.3;


    class Chunk;
    class OctreeNode
    {
    public:
        OctreeNode(int size, VecInt3 pos, Terrain::Chunk *chunkPtr, CollisionObject *collsion = nullptr)
            : size(size), intPos(pos), glmPos(Terrain::scale * VecInt3ToVec3(pos)), 
              center(glmPos + glm::vec3(float(size - 1) * Terrain::scale / 2, float(size - 1) * Terrain::scale / 2, float(size - 1) * Terrain::scale/ 2)),
              chunk(chunkPtr)
        {
            if (collsion != nullptr)
                collideWith(collsion);
            if (whole)
            {
                addPoints(1);
            }
        }

        ~OctreeNode()
        {
            if (whole)
                addPoints(-1);
        }

        static void init();

        void genBuffers();
        void genNormalBuffer();

        void makeChilds(CollisionObject *collObj)
        {
            if (size == 1)
                toDelete = true;
            else
            {
                int halfSize = size / 2;
                for (int i = 0; i < 8; i++)
                {
                    childs[i] = std::make_unique<OctreeNode>(halfSize, intPos + cubeVer[i] * halfSize, chunk, collObj);
                    if (childs[i]->empty())
                        childs[i].reset(nullptr);
                }
            }
        }

        void collideWith(CollisionObject *collObj);

        bool empty()
        {
            return toDelete;
        }

    private:
        void addPoints(int d);
        bool areCornersIn(CollisionObject *collObj);

        inline static std::vector<GhostObject> cubeObjects;

        bool whole = true;
        bool toDelete = false;
        std::array<std::unique_ptr<OctreeNode>, 8> childs;
        int childCount = 8;
        int size;
        VecInt3 intPos;
        glm::vec3 glmPos;
        glm::vec3 center;
        Chunk *chunk;
        OctreeNode *parent;
    };

    class Chunk : public DrawableObject, public CollisionObject::CollisionObjectOwner
    {
    public:
        Chunk(VecInt3 intPos, Terrain *terrainPtr, bool fill = false) : terrain(terrainPtr), intPos(intPos)
        {
            if (fill)
            {
                rootTerrainCube = std::make_unique<OctreeNode>(chunkSize, intPos, this);
                static const glm::vec3 halfVector = Terrain::scale * glm::vec3(float(chunkSize) / 2, float(chunkSize) / 2, float(chunkSize) / 2);
                ghostObject = std::make_unique<GhostObject>(&(terrain->privateWorld), &boxShape, Terrain::scale * VecInt3ToVec3(intPos) + halfVector);
                ghostObject->setOwnerPtr(this);
            }
        }

        void prepareVertexBuffer();
        void prepareBuffers();

        void loadBuffers();

        void normalsAtPos(int size, VecInt3 pos)
        {
            drawAtPos(size, pos, [this](VecInt3 a) {
                getOneCubeNormals(a);
            });
        }

        void verticesAtPos(int size, VecInt3 pos)
        {
            drawAtPos(size, pos, [this](VecInt3 a) {
                getOneCube(a);
            });
        }

        void drawAtPos(int size, VecInt3 pos, std::function<void(VecInt3)> f)
        {
            for (int x = -1; x < size; x++)
                for (int y = -1; y < size; y++)
                {
                    f(pos + getVecInt3(x, y, -1));
                    f(pos + getVecInt3(x, y, size - 1));
                }
            for (int z = 0; z < size - 1; z++)
                for (int other = -1; other < size - 1; other++)
                {
                    f(pos + getVecInt3(other + 1, -1, z));
                    f(pos + getVecInt3(other, size - 1, z));
                    f(pos + getVecInt3(-1, other, z));
                    f(pos + getVecInt3(size - 1, other + 1, z));
                }
        }

        void collideWith(CollisionObject *collObj);

        glm::vec3 getPartNormal(int partId)
        {
            partId *= 9;
            return glm::vec3(normals[partId], normals[partId + 1], normals[partId + 2]);
        }

    private:
        Terrain *terrain;

        std::unique_ptr<btTriangleMesh> newTriangleMesh;
        std::unique_ptr<btTriangleMesh> triangleMesh;
        std::unique_ptr<btBvhTriangleMeshShape> shape;
        std::unique_ptr<btBvhTriangleMeshShape> newShape;

        std::unique_ptr<RigidBody> rigidBody;
        std::unique_ptr<GhostObject> ghostObject;
        inline static btBoxShape boxShape = btBoxShape(Terrain::scale * btVector3((float)chunkSize - 1 / 2, (float)chunkSize - 1 / 2, (float)chunkSize - 1 / 2));

        VecInt3 intPos = {0, 0, 0};

        void getOneCube(VecInt3 pos)
        {
            if (!inChunk(pos) || !pointsTouched.insert(pos).second)
                return;
            
            int mask = getCubeMask(pos);

            glm::vec3 glmPos = VecInt3ToVec3(pos);
            for (size_t i = 0; i < marchingCubesVertices[mask].size(); i++)
            {
                glm::vec3 x = (marchingCubesVertices[mask][i] + glmPos) * Terrain::scale;
                vertices.insert(vertices.end(), glm::value_ptr(x), glm::value_ptr(x) + 3);
                terrain->pointNormals[x] += marchingCubesNormals[mask][i];
                auto p = glm::value_ptr(marchingCubesNormals[mask][i]);
                blockyNormals.insert(blockyNormals.end(), p, p+3);
                normalsToDelete.emplace_back(x, marchingCubesNormals[mask][i]);
            }

            const std::vector<glm::vec3> &verts = marchingCubesVertices[mask];
            assert(verts.size() % 3 == 0);
            for (size_t i = 0; i < verts.size(); i += 3)
            {
                newTriangleMesh->addTriangle(toBtVec3(verts[i] + glmPos) * Terrain::scale, toBtVec3(verts[i + 1] + glmPos) * Terrain::scale, toBtVec3(verts[i + 2] + glmPos) * Terrain::scale);
            }
        }

        void getOneCubeNormals(VecInt3 pos)
        {
            if (!inChunk(pos) || !pointsTouched.insert(pos).second)
                return;

            int mask = getCubeMask(pos);

            glm::vec3 glmPos = VecInt3ToVec3(pos);
            for (size_t i = 0; i < marchingCubesVertices[mask].size(); i++)
            {
                glm::vec3 x = (marchingCubesVertices[mask][i] + glmPos) * Terrain::scale;
                assert(terrain->pointNormals.find(x) != terrain->pointNormals.end());
                glm::vec3 n = glm::normalize(terrain->pointNormals[x]);
                assert(isVec3Ok(n));
                assert(abs(glm::length(n) - 1) < 1e-6);
                normals.insert(normals.end(), glm::value_ptr(n), glm::value_ptr(n) + 3);
            }
        }

        int getCubeMask(VecInt3 pos)
        {
            int mask = 0;
            for (int b = 0; b < 8; b++)
                if (terrain->marchingPoints.count(pos + cubeVer[b]))
                    mask |= 1 << b;
            return mask;
        }

        bool inChunk(VecInt3 pos)
        {
            VecInt3 diff = pos - intPos;
            for (auto i : diff)
                if (i < 0 || i >= chunkSize)
                    return false;
            return true;
        }

        std::unique_ptr<OctreeNode> rootTerrainCube;
        
        std::vector<std::pair<glm::vec3, glm::vec3>> normalsToDelete;
        std::set<VecInt3> pointsTouched;

        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> blockyNormals;
        std::vector<float> texCoords;
        std::vector<float> tangents;
        std::vector<float> bitangents;

        const VertexArray &getVertexArray() const override { return vertexArray; }

        VertexArray vertexArray;
        friend class OctreeNode;
        friend class Terrain;

        static void init()
        {
            slimeShader.load("shaders/slime_vert.glsl", "shaders/slime_frag.glsl");

            texture0.load("Slime_DIFF", true);
            texture1.load("Slime_SPEC", true);
            texture2.load("Slime_NRM");
            texture3.load("Slime_HGT");
        }

        static inline Texture2d texture0, texture1, texture2, texture3;
        static inline ShaderProgram slimeShader;

    protected:
        const ShaderProgram &getShaderProgram() const override { return slimeShader; }
        const Texture2d &getTexture0() const override { return texture0; }
        const Texture2d &getTexture1() const override { return texture1; }
        const Texture2d &getTexture2() const override { return texture2; }
        const Texture2d &getTexture3() const override { return texture3; }
        void draw(Window *window) const override
        {
            glFrontFace(GL_CCW);
            DrawableObject::draw(window);
            glFrontFace(GL_CW);
        }
    };

    void updateBuffers();
    void loadBuffers();

    World &world;
    World privateWorld;

    std::mutex mutex;

    std::set<VecInt3> toRegen;
    std::set<VecInt3> toReload;
    std::set<VecInt3> marchingPoints;

    struct glmVec3Comp
    {
        static constexpr float eps = 1e-4;
        bool operator()(const glm::vec3& a, const glm::vec3& b) const
        {
            if(a.x + eps < b.x)
                return true;
            if(b.x + eps < a.x)
                return false;
            if(a.y + eps < b.y)
                return true;
            if(b.y + eps < a.y)
                return false;
            return a.z + eps < b.z;
        }
    };
    std::map<glm::vec3, glm::vec3, glmVec3Comp> pointNormals;

    static VecInt3 getChunkFromPoint(VecInt3 pos);
    std::map<VecInt3, std::unique_ptr<Chunk>> chunks;

    std::vector<std::thread> threads;

    static void CustomMaterialCombinerCallback(btManifoldPoint &, const btCollisionObjectWrapper *, int, int, const btCollisionObjectWrapper *, int, int);
};

#endif
