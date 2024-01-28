#include "model/voxel.hpp"

Voxel::Voxel(uint16_t id)
	: id(id)
{
}

Chunk::Chunk()
{
	voxels.resize(ChunkSize * ChunkSize * ChunkSize);
}

void Chunk::load()
{
	isLoaded = true;
}

void Chunk::setup()
{
	isSetup = true;
}

void Chunk::rebuild()
{

}

void Chunk::unload()
{
	isLoaded = false;
}

World::World()
{
	chunks.resize(WorldSize * WorldSize * WorldSize);
}

void World::update(float dt, glm::vec3 cameraPos, glm::vec3 cameraView)
{
	updateAsyncChunker();
	updateLoadList();
	updateSetupList();
	updateRebuildList();
	updateFlagsList();
	updateUnloadList();
	updateVisibilityList(cameraPos);

	if(cameraPos != _cameraPos || cameraView != _cameraView)
		updateRenderList();
	
	_cameraPos = cameraPos;
	_cameraView = cameraView;
}

void World::updateAsyncChunker()
{

}

// iterate over the pending load chunk list and load chunks
// loadList is cleared every frame and gets re-updated in the visibility update step
// TODO : add additionnal code to limit the number of chunks loaded per frame
void World::updateLoadList()
{
	int chunkLoadedCount = 0;
	for (int i = 0; i < _loadList.size(); i++)
	{
		Chunk& chunk = _loadList[i];
		if (!chunk.isLoaded)
		{
			chunk.load();
			chunkLoadedCount++;
			_forceVisibilityUpdate = true;
		}
	}
	_loadList.clear();
}

// iterate over the pending setup chunk list and setup chunks
// setupList is cleared every frame and gets re-updated in the visibility update step
// TODO : add additionnal code to limit the number of chunks loaded per frame
void World::updateSetupList()
{
	for (int i = 0; i < _setupList.size(); i++)
	{
		Chunk& chunk = _setupList[i];
		if (chunk.isLoaded && !chunk.isSetup)
		{
			chunk.setup();

			if(chunk.isSetup)
				_forceVisibilityUpdate = true;
		}
	}
	_setupList.clear();
}

// iterate over the pending rebuild chunk list and rebuild chunks
// rebuildList is cleared every frame and gets re-updated in the visibility update step
// TODO : add additionnal code to limit the number of chunks loaded per frame
// TODO : add neighbors to flagsList for update their state
void World::updateRebuildList()
{
	int chunkRebuiltCount = 0;
	for (int i = 0; i < _rebuildList.size(); i++)
	{
		Chunk& chunk = _rebuildList[i];
		if (chunk.isLoaded && chunk.isSetup)
		{
			chunk.rebuild();
			_flagsList.push_back(chunk);

			// add neighbors to flag list for update too
			// ...
			
			chunkRebuiltCount++;
			_forceVisibilityUpdate = true;
		}
	}
	_rebuildList.clear();
}

//
void World::updateFlagsList()
{
	
}

// iterate over the pending unload chunk list and unload chunks
// unloadList is cleared every frame and gets re-updated in the visibility update step
void World::updateUnloadList()
{
	for (int i = 0; i < _unloadList.size(); i++)
	{
		Chunk& chunk = _unloadList[i];
		if (chunk.isLoaded)
		{
			chunk.unload();
			_forceVisibilityUpdate = true;
		}
	}
	_unloadList.clear();
}

void World::updateVisibilityList(glm::vec3 cameraPos)
{
	const static int radius = 4;

	// update visibility list
	if (_forceVisibilityUpdate)
	{
		for (int chunkIndex = 0; chunkIndex < chunks.size(); chunkIndex++)
		{
			glm::vec3 chunkPosition(chunkIndex % WorldSize,
								   (chunkIndex / WorldSize) % WorldSize,
								   (chunkIndex / (WorldSize * WorldSize)) % WorldSize);

			Chunk& chunk = chunks[chunkIndex];

			if (glm::distance(chunkPosition, cameraPos) < radius)
			{
				if(!chunk.isLoaded)
					_loadList.push_back(chunk);
				else if(!chunk.isSetup)
					_setupList.push_back(chunk);
				else
					_visibilityList.push_back(chunk);
			}
		}
	}
}

void World::updateRenderList()
{
	renderList.clear();
	for (int i = 0; i < _visibilityList.size(); i++)
	{
		Chunk& chunk = _visibilityList[i];
		if (chunk.isLoaded && chunk.isSetup && chunk.shouldRender)
		{
			// TODO : check if chunk is visible (Frustrum culling)
			renderList.push_back(chunk);
		}
	}
	_unloadList.clear();
}
