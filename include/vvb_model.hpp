#pragma once
// vulkan base
#include "vvb_buffer.hpp"
#include "vvb_texture.hpp"

//std
#include <array>
#include <memory>

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

class VvbModel
{
public:
	
	struct Vertex {
		glm::vec3 position{};
		glm::vec3 color{};
		glm::vec2 texCoord;

		bool operator==(const Vertex& other) const;

		static VkVertexInputBindingDescription getBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
	};

	VvbModel(VvbDevice& vvbDevice, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const std::string textureFile);
	VvbModel(VvbDevice& vvbDevice, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	VvbModel(VvbDevice& vvbDevice, std::vector<Vertex>& vertices);
	VvbModel(VvbDevice& vvbDevice, const std::string modelFile, const std::string textureFile);
	~VvbModel();

	// delete copy constructors
	VvbModel(const VvbModel&) = delete;
	VvbModel& operator=(const VvbModel&) = delete;

	void createVertexBuffer(std::vector<Vertex>& vertices);
	void createIndexBuffer(std::vector<uint32_t>& indices);
	void createTexture(const std::string textureFile);

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);


	// test
	VkImageView getTextureImageView() const { return texture->getTextureImageView(); }
	VkSampler getTextureSampler() const { return texture->getTextureSampler(); }


private:
	// vulkan base ref
	VvbDevice& vvbDevice;

	std::unique_ptr<VvbBuffer> vertexBuffer;
	uint32_t vertexCount;

	bool hasIndexBuffer = false;
	std::unique_ptr<VvbBuffer> indexBuffer;
	uint32_t indexCount;

	bool hasTexture = false;
	std::unique_ptr<VvbTexture> texture;

	void loadModel(const std::string modelFile);
};

// implementation of hash calculation for Vertex
namespace std
{
	template<> struct hash<VvbModel::Vertex>
	{
		size_t operator()(VvbModel::Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.position) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}