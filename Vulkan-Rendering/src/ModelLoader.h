#pragma once

#include <string>

//Project Includes
class VertexBuffer;
class IndexBuffer;
class Texture;

class ModelLoader
{
public:
    ModelLoader();
    ~ModelLoader() = default;

    void LoadModel(const std::string modelPath);

    void CreateModelBuffers() const;

    void CleanUp();

    VertexBuffer& GetModelVertex();
    IndexBuffer& GetModelIndex();
    Texture& GetModelTexture();

private:

    void CreateModelVertexBuffer() const;

    void CreateModelIndexBuffer() const;
    
    VertexBuffer* m_modelVertex;
    IndexBuffer* m_modelIndices;
    Texture* m_modelTexture;
};