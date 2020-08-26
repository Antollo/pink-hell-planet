#ifndef LOADER_H_
#define LOADER_H_

#include <tiny_obj_loader.h>
#include "error.h"

inline void loadObjFile(const std::string &modelFilename, std::vector<float> &vertices, std::vector<float> &colors, std::vector<float> &normals, std::vector<float> &texCoords, const float& scale = 1.f)
{
    const std::vector<float> ones(3, 1);
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelFilename.c_str());

    if (!warn.empty())
        error("Warning: ", warn);
    if (!err.empty())
        error("Error: ", err);
    if (!ret)
        errorAndExit("Model not loaded");
    std::cout << "Shapes: " << shapes.size() << std::endl;
    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;
        std::cout << "    Faces: " << shapes[s].mesh.num_face_vertices.size() << std::endl;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = shapes[s].mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                vertices.insert(vertices.end(), attrib.vertices.begin() + 3 * idx.vertex_index, attrib.vertices.begin() + 3 * idx.vertex_index + 3);
                vertices[vertices.size() - 1] /= scale;
                vertices[vertices.size() - 2] /= scale;
                vertices[vertices.size() - 3] /= scale;
                normals.insert(normals.end(), attrib.normals.begin() + 3 * idx.normal_index, attrib.normals.begin() + 3 * idx.normal_index + 3);
                if (3 * idx.vertex_index + 3 < int(attrib.colors.size()))
                    colors.insert(colors.end(), attrib.colors.begin() + 3 * idx.vertex_index, attrib.colors.begin() + 3 * idx.vertex_index + 3);
                //else
                //    colors.insert(colors.end(), ones.begin(), ones.end());
                texCoords.insert(texCoords.end(), attrib.texcoords.begin() + 2 * idx.texcoord_index, attrib.texcoords.begin() + 2 * idx.texcoord_index + 2);
                //std::cout << attrib.texcoords[2 * idx.texcoord_index] << std::endl;
            }
            index_offset += fv;
        }
    }
}

#endif /* !LOADER_H_ */
