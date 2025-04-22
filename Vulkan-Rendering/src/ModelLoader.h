#pragma once

#include <string>

//Project Includes
class VertexBuffer;
class IndexBuffer;

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

private:

    void CreateModelVertexBuffer() const;

    void CreateModelIndexBuffer() const;
    
    VertexBuffer* m_modelVertex;
    IndexBuffer* m_modelIndices;
};