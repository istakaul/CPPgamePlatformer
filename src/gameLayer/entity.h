#pragma once

#include <random>
#include <physics.h>
#include <unordered_map>

struct AssetManager;
struct EntityHolder;

enum EntityType {
	EntityType_Player = 0,
	EntityType_Slime,
	EntityType_DroppedItem
};

struct EntityUpdateData {
	Vector2 playerPosition = {};
	std::ranlux24_base &rng;
	EntityHolder &entityHolder;

	std::uint64_t ownId = 0;

};

struct Entity {
	// All entities have physics (like a position, a size and velocity)
	PhysicalEntity physics;

	Vector2 &getPosition() {
		return physics.transform.pos;
	}

	void teleport(Vector2 pos) {
		physics.teleport(pos);
	}

	virtual void render(AssetManager &assetManager) = 0;

	virtual bool update(float deltaTime, EntityUpdateData entityUpdateData) = 0;

	virtual int getEntityType() = 0;
};
