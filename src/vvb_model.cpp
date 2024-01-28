// vulkan base
#include "vvb_model.hpp"

// tiny obj loader
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <unordered_map>

bool VvbModel::Vertex::operator==(const Vertex& other) const {
    return position == other.position && color == other.color && texCoord == other.texCoord;
}

VkVertexInputBindingDescription VvbModel::Vertex::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> VvbModel::Vertex::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
}

VvbModel::VvbModel(VvbDevice& vvbDevice, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const std::string textureFile)
    : vvbDevice(vvbDevice)
{
    createVertexBuffer(vertices);
    createIndexBuffer(indices);
    createTexture(textureFile);
}

VvbModel::VvbModel(VvbDevice& vvbDevice, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
    : vvbDevice(vvbDevice)
{
    createVertexBuffer(vertices);
    createIndexBuffer(indices);
    texture = std::make_unique<VvbTexture>(vvbDevice);
}

VvbModel::VvbModel(VvbDevice& vvbDevice, std::vector<Vertex>& vertices)
    : vvbDevice(vvbDevice)
{
    createVertexBuffer(vertices);
    texture = std::make_unique<VvbTexture>(vvbDevice);
}

VvbModel::VvbModel(VvbDevice& vvbDevice, const std::string modelFile, const std::string textureFile)
    : vvbDevice(vvbDevice)
{
    loadModel(modelFile);
    createTexture(textureFile);
}

VvbModel::~VvbModel()
{
}

void VvbModel::createVertexBuffer(std::vector<Vertex>& vertices)
{
    VkDeviceSize vertexSize = sizeof(vertices[0]);
    vertexCount = static_cast<uint32_t>(vertices.size());

    VvbBuffer stvvbingBuffer{
        vvbDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    VkResult res = stvvbingBuffer.map();
    stvvbingBuffer.write(vertices.data());

    vertexBuffer = std::make_unique<VvbBuffer>(
        vvbDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    vvbDevice.copyBuffer(stvvbingBuffer.getBuffer(), vertexBuffer->getBuffer(), vertexBuffer->getSize());
}

void VvbModel::createIndexBuffer(std::vector<uint32_t>& indices)
{
    indexCount = static_cast<uint32_t>(indices.size());

    if (indexCount == 0)
        return;

    hasIndexBuffer = true;

    VvbBuffer stvvbingBuffer{
        vvbDevice,
        sizeof(uint32_t),
        indexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    VkResult res = stvvbingBuffer.map();
    stvvbingBuffer.write(indices.data());

    indexBuffer = std::make_unique<VvbBuffer>(
        vvbDevice,
        sizeof(uint32_t),
        indexCount,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    vvbDevice.copyBuffer(stvvbingBuffer.getBuffer(), indexBuffer->getBuffer(), indexBuffer->getSize());
}

void VvbModel::createTexture(const std::string textureFile)
{
    texture = std::make_unique<VvbTexture>(vvbDevice, textureFile);
}

void VvbModel::bind(VkCommandBuffer commandBuffer)
{
    // bind vertex buffer
    VkBuffer buffers[] = { vertexBuffer->getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    // bind index buffer
    if (hasIndexBuffer)
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void VvbModel::draw(VkCommandBuffer commandBuffer)
{
    if (hasIndexBuffer)
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);

    else
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

void VvbModel::loadModel(const std::string modelFile)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelFile.c_str())) 
        throw std::runtime_error(warn + err);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};
            
            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
    createVertexBuffer(vertices);
    createIndexBuffer(indices);
}
