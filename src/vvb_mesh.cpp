// vulkan base
#include "vvb_mesh.hpp"

bool Vertex::operator==(const Vertex& other) const {
    return position == other.position && color == other.color && texCoord == other.texCoord;
}

VkVertexInputBindingDescription Vertex::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions()
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

VvbMesh::VvbMesh(VvbDevice& vvbDevice, Chunk chunk)
    : vvbDevice(vvbDevice)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    generateGeometry(chunk, vertices, indices);

    createVertexBuffer(vertices);
    createIndexBuffer(indices);
}

VvbMesh::~VvbMesh()
{
}

void VvbMesh::generateGeometry(Chunk chunk, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    for (int voxelIndex = 0; voxelIndex < chunk.voxels.size(); voxelIndex++)
    {
		Voxel voxel = chunk.voxels[voxelIndex];
		if(voxel.id == 0)
			continue;

		int x = voxelIndex % Chunk::ChunkSize;
		int y = (voxelIndex / Chunk::ChunkSize) % Chunk::ChunkSize;
		int z = (voxelIndex / (Chunk::ChunkSize * Chunk::ChunkSize)) % Chunk::ChunkSize;

		glm::vec3 position = glm::vec3(x, y, z);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 texCoord = glm::vec2(0.0f, 0.0f);

        // front
		vertices.push_back({ position + glm::vec3(0.0f, 0.0f, 1.0f), color, texCoord });
		vertices.push_back({ position + glm::vec3(1.0f, 0.0f, 1.0f), color, texCoord });
		vertices.push_back({ position + glm::vec3(1.0f, 1.0f, 1.0f), color, texCoord });
		vertices.push_back({ position + glm::vec3(0.0f, 1.0f, 1.0f), color, texCoord });

		indices.push_back(vertexCount + 0);
		indices.push_back(vertexCount + 1);
		indices.push_back(vertexCount + 2);
		indices.push_back(vertexCount + 2);
		indices.push_back(vertexCount + 3);
		indices.push_back(vertexCount + 0);

		vertexCount += 4;
		indexCount += 6;

		// back
        vertices.push_back({ position + glm::vec3(0.0f, 0.0f, 0.0f), color, texCoord });
        vertices.push_back({ position + glm::vec3(1.0f, 0.0f, 0.0f), color, texCoord });
        vertices.push_back({ position + glm::vec3(1.0f, 1.0f, 0.0f), color, texCoord });
        vertices.push_back({ position + glm::vec3(0.0f, 1.0f, 0.0f), color, texCoord });

        indices.push_back(vertexCount + 0);
	    indices.push_back(vertexCount + 1);
	    indices.push_back(vertexCount + 2);
	    indices.push_back(vertexCount + 2);
	    indices.push_back(vertexCount + 3);
	    indices.push_back(vertexCount + 0);

	    vertexCount += 4;
	    indexCount += 6;

		// left
		vertices.push_back({ position + glm::vec3(0.0f, 0.0f, 0.0f), color, texCoord });
		vertices.push_back({ position + glm::vec3(0.0f, 0.0f, 1.0f), color, texCoord });
		vertices.push_back({ position + glm::vec3(0.0f, 1.0f, 1.0f), color, texCoord });
		vertices.push_back({ position + glm::vec3(0.0f, 1.0f, 0.0f), color, texCoord });

		indices.push_back(vertexCount + 0);
	    indices.push_back(vertexCount + 1);
	    indices.push_back(vertexCount + 2);
	    indices.push_back(vertexCount + 2);
	    indices.push_back(vertexCount + 3);
	    indices.push_back(vertexCount + 0);

	    vertexCount += 4;
	    indexCount += 6;

        
        // right
        vertices.push_back({ position + glm::vec3(1.0f, 0.0f, 0.0f), color, texCoord });
        vertices.push_back({ position + glm::vec3(1.0f, 0.0f, 1.0f), color, texCoord });
        vertices.push_back({ position + glm::vec3(1.0f, 1.0f, 1.0f), color, texCoord });
        vertices.push_back({ position + glm::vec3(1.0f, 1.0f, 0.0f), color, texCoord });

        indices.push_back(vertexCount + 0);
	    indices.push_back(vertexCount + 1);
	    indices.push_back(vertexCount + 2);
	    indices.push_back(vertexCount + 2);
	    indices.push_back(vertexCount + 3);
	    indices.push_back(vertexCount + 0);

	    vertexCount += 4;
	    indexCount += 6;

		// top
		vertices.push_back({ position + glm::vec3(0.0f, 1.0f, 0.0f), color, texCoord });
		vertices.push_back({ position + glm::vec3(1.0f, 1.0f, 0.0f), color, texCoord });
		vertices.push_back({ position + glm::vec3(1.0f, 1.0f, 1.0f), color, texCoord });
		vertices.push_back({ position + glm::vec3(0.0f, 1.0f, 1.0f), color, texCoord });

		indices.push_back(vertexCount + 0);
	    indices.push_back(vertexCount + 1);
	    indices.push_back(vertexCount + 2);
	    indices.push_back(vertexCount + 2);
	    indices.push_back(vertexCount + 3);
	    indices.push_back(vertexCount + 0);

	    vertexCount += 4;
	    indexCount += 6;

        // bottom
        vertices.push_back({ position + glm::vec3(0.0f, 0.0f, 0.0f), color, texCoord });
        vertices.push_back({ position + glm::vec3(1.0f, 0.0f, 0.0f), color, texCoord });
        vertices.push_back({ position + glm::vec3(1.0f, 0.0f, 1.0f), color, texCoord });    
        vertices.push_back({ position + glm::vec3(0.0f, 0.0f, 1.0f), color, texCoord });

        indices.push_back(vertexCount + 0);
        indices.push_back(vertexCount + 1);
        indices.push_back(vertexCount + 2);
        indices.push_back(vertexCount + 2);
        indices.push_back(vertexCount + 3);
        indices.push_back(vertexCount + 0);

        vertexCount += 4;
        indexCount += 6;
    }
}

void VvbMesh::createVertexBuffer(std::vector<Vertex>& vertices)
{
    VkDeviceSize vertexSize = sizeof(Vertex);
    VvbBuffer stagingBuffer{
        vvbDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    VkResult res = stagingBuffer.map();
    stagingBuffer.write(vertices.data());

    vertexBuffer = std::make_unique<VvbBuffer>(
        vvbDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    vvbDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), vertexBuffer->getSize());
}


void VvbMesh::createIndexBuffer(std::vector<uint32_t>& indices)
{
    if (indexCount == 0)
        return;

    hasIndexBuffer = true;

    VvbBuffer stagingBuffer{
        vvbDevice,
        sizeof(uint32_t),
        indexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    VkResult res = stagingBuffer.map();
    stagingBuffer.write(indices.data());

    indexBuffer = std::make_unique<VvbBuffer>(
        vvbDevice,
        sizeof(uint32_t),
        indexCount,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    vvbDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), indexBuffer->getSize());
}

void VvbMesh::bind(VkCommandBuffer commandBuffer)
{
    // bind vertex buffer
    VkBuffer buffers[] = { vertexBuffer->getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    // bind index buffer
    if (hasIndexBuffer)
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void VvbMesh::draw(VkCommandBuffer commandBuffer)
{
    if (hasIndexBuffer)
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    else
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}
