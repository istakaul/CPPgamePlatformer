#include <entities/droppedItem.h>
#include <assetManager.h>
#include <helpers.h>
#include <entityIdHolder.h>

void DroppedItem::render(AssetManager &assetManager) {
	
	auto aabb = getRectangleForEntity(physics.transform, 1, 1);

	DrawTexturePro(
		assetManager.textures,
		getTextureAtlas(itemType, 4, 32, 32),
		aabb, // dest
		{0,0}, // origin (top-left corner)
		0.0f, // rotation
		WHITE // tint
	);
}

bool DroppedItem::update(float deltaTime, EntityUpdateData entityUpdateData) {

	for (auto &e : entityUpdateData.entityHolder.entities) { // we iterate all entities ...
		if (e.first != entityUpdateData.ownId) { // that are not the current entity ...
			if (e.second->getEntityType() == EntityType::EntityType_DroppedItem) { // and that are also other items
				DroppedItem *other = reinterpret_cast<DroppedItem *>(e.second.get());

				if (itemType == other->itemType) { // if the current entity stores the same item type as the other entity ...
					if (Vector2Distance(getPosition(), other->getPosition()) < 0.7) { // and also they are close ...
						other->itemCounter += itemCounter; // we donate our items to it;
						return 0; // and we delete the current entity
					}

				}
			}
		}
	}

	return true;
}
