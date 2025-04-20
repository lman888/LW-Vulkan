#pragma once

#include <string>
#include <vector>

//Project Includes
#include "VertexBuffer.h"

class ModelLoader
{
public:
    ModelLoader();
    ~ModelLoader() = default;

    void LoadModel(const std::string modelPath);

private:
    
};
