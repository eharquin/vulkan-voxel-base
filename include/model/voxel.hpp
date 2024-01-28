#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

struct Voxel
{
	enum class Type : uint16_t
	{
		air = 0,
		dirt = 1,
		grass = 2,
		stone = 3,
		sand = 5,
		NUM_TYPES
	};

	uint16_t id;
	Voxel(uint16_t id = (uint16_t)Type::dirt);
};

struct Chunk
{
	std::vector<Voxel> voxels;
	bool isLoaded;
	bool isSetup;
	bool shouldRender;

	Chunk();

	void load();
	void setup();
	void rebuild();
	void unload();

	static const int ChunkSize = 1;
};

class World
{
public:

	std::vector<Chunk> chunks;
	std::vector<Chunk> renderList;
	static const int WorldSize = 2;
	
	World();
	void update(float dt, glm::vec3 cameraPos, glm::vec3 cameraView);

private:
	glm::vec3 _cameraPos;
	glm::vec3 _cameraView;

	std::vector<Chunk> _loadList;
	std::vector<Chunk> _setupList;
	std::vector<Chunk> _rebuildList;
	std::vector<Chunk> _flagsList;
	std::vector<Chunk> _unloadList;
	std::vector<Chunk> _visibilityList;

	bool _forceVisibilityUpdate;

	void updateAsyncChunker();
	void updateLoadList();
	void updateSetupList();
	void updateRebuildList();
	void updateFlagsList();
	void updateUnloadList();
	void updateVisibilityList(glm::vec3 cameraPos);

	void updateRenderList();
};