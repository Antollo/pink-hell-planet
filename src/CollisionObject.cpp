#include "CollisionObject.h"

#include <algorithm>
#include <iostream>
#include <LinearMath/btTransform.h>
#include "debug.h"

std::unique_ptr<btCollisionShape> CollisionObject::shapeFromVertices(const std::vector<float> &vertices)
{
    btConvexHullShape *shape = new btConvexHullShape(vertices.data(), vertices.size() / 3, 3 * sizeof(float));
    shape->optimizeConvexHull();
    return std::unique_ptr<btCollisionShape>(shape);
}

std::unique_ptr<btCollisionShape> CollisionObject::boundingBoxShape(const std::vector<float> &vertices)
{
    std::array<std::vector<float>, 3> splitVertices;
    for (size_t i = 0; i < vertices.size(); i++)
        splitVertices[i % 3].push_back(vertices[i]);

    std::vector<float> bbVertices;
    std::array<std::array<float, 2>, 3> pairs;
    for (int i = 0; i < 3; i++)
        pairs[i] = {*std::min_element(splitVertices[i].begin(), splitVertices[i].end()), *std::max_element(splitVertices[i].begin(), splitVertices[i].end())};

    float margin = pairs[0][1] - pairs[0][0];
    for (int i = 1; i < 3; i++)
        margin = std::min(margin, pairs[i][1] - pairs[i][0]);

    margin *= 0.1;
    for (int i = 0; i < 3; i++)
    {
        pairs[i][0] += margin;
        pairs[i][1] -= margin;
    }

    btCompoundShape *shape = new btCompoundShape(true, 9);

    for (auto i : pairs[0])
        for (auto j : pairs[1])
            for (auto k : pairs[2])
            {
                bbVertices.push_back(i);
                bbVertices.push_back(j);
                bbVertices.push_back(k);
                shape->addChildShape(btTransform(btQuaternion::getIdentity(), btVector3(i, j, k)), new btSphereShape(margin));
            }
    shape->addChildShape(btTransform::getIdentity(), shapeFromVertices(bbVertices).release());

    return std::unique_ptr<btCollisionShape>(shape);
}

bool CollisionObject::customContactAddedCallback(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap, int partId0, int index0,
                                                 const btCollisionObjectWrapper *colObj1Wrap, int partId1, int index1)
{
    CollisionObject *co1 = static_cast<CollisionObject *>(colObj0Wrap->getCollisionObject()->getUserPointer());
    CollisionObject *co2 = static_cast<CollisionObject *>(colObj1Wrap->getCollisionObject()->getUserPointer());

    co1->contactAddedCallback(co2);
    co2->contactAddedCallback(co1);

    for (auto &callback : globalMaterialCombinerCallbacks)
        callback(cp, colObj0Wrap, partId0, index0, colObj1Wrap, partId1, index1);

    // value returned is ignored by bullet
    return true;
}

extern ContactAddedCallback gContactAddedCallback;

void CollisionObject::init()
{
    gContactAddedCallback = customContactAddedCallback;
}
