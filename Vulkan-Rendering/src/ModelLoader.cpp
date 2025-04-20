#include "ModelLoader.h"

#include <stdexcept>
#include <unordered_map>
#include <tiny_obj_loader.h>

#include "Pipelines.h"
#include "VulkanCore.h"

ModelLoader::ModelLoader()
{

}

void ModelLoader::LoadModel(const std::string modelPath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos =
            {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord =
            {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = {1.0f, 1.0f, 1.0f};

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(VulkanCore::GetVertexBuffer()->GetVertices().size());
                VulkanCore::GetVertexBuffer()->GetVertices().push_back(vertex);
            }
            
            VulkanCore::GetPipeline()->GetIndices().push_back(uniqueVertices[vertex]);
        }
    }
}