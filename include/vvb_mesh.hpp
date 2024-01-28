#pragma once

// vulkan base
#include "vvb_buffer.hpp"
#include "vvb_texture.hpp"
#include "model/voxel.hpp"

//std
#include <array>
#include <memory>

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoord;

	bool operator==(const Vertex& other) const;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

class VvbMesh
{
public:
	VvbMesh(VvbDevice& device, Chunk chunk);
	~VvbMesh();

	// delete copy constructors
	VvbMesh(const VvbMesh&) = delete;
	VvbMesh& operator=(const VvbMesh&) = delete;

	void generateGeometry(Chunk chunk, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

	void createVertexBuffer(std::vector<Vertex>& vertices);
	void createIndexBuffer(std::vector<uint32_t>& indices);

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);

private:
	// vulkan base ref
	VvbDevice& vvbDevice;

	std::unique_ptr<VvbBuffer> vertexBuffer;
	uint32_t vertexCount = 0;

	bool hasIndexBuffer = false;
	std::unique_ptr<VvbBuffer> indexBuffer;
	uint32_t indexCount = 0;
};

// implementation of hash calculation for Vertex
namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.position) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}