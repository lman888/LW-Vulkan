#include "ModelLoader.h"

#include <stdexcept>
#include <unordered_map>
#include <tiny_obj_loader.h>

//Project Includes
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Texture.h"

ModelLoader::ModelLoader()
{
    m_modelIndices = new IndexBuffer();
    m_modelVertex = new VertexBuffer();
    m_modelTexture = new Texture();
}

void ModelLoader::LoadModel(const std::string modelPath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str()))
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
                uniqueVertices[vertex] = static_cast<uint32_t>(GetModelVertex().GetVertices().size());
                GetModelVertex().GetVertices().push_back(vertex);
            }
            
            GetModelIndex().GetIndices().push_back(uniqueVertices[vertex]);
        }
    }
}

void ModelLoader::CreateModelBuffers() const
{
    CreateModelVertexBuffer();
    CreateModelIndexBuffer();
}

void ModelLoader::CleanUp()
{
    m_modelVertex->CleanUp();
    m_modelIndices->CleanUp();
    m_modelTexture->CleanUp();

    m_modelVertex = nullptr;
    m_modelIndices = nullptr;
    m_modelTexture = nullptr;
}

VertexBuffer& ModelLoader::GetModelVertex()
{
    return *m_modelVertex;
}

IndexBuffer& ModelLoader::GetModelIndex()
{
    return *m_modelIndices;
}

Texture& ModelLoader::GetModelTexture()
{
    return *m_modelTexture;
}

void ModelLoader::CreateModelVertexBuffer() const
{
    m_modelVertex->CreateVertexBuffer();
}

void ModelLoader::CreateModelIndexBuffer() const
{
    m_modelIndices->CreateIndexBuffer();
}