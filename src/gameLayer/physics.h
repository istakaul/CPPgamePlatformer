#pragma once
#include <raylib.h>

struct Transform2D {
	// center of the entity
	Vector2 pos = {}; 

	// size of the entity
	float w = 0; // width
	float h = 0; // height
	
	// helper methods that return other pos around the size of the entity apart from the center
	Vector2 getCenter()		 const { return { pos.x, pos.y }; }
	Vector2 getTop()		 const { return { pos.x, pos.y - h * 0.5f }; }
	Vector2 getBottom()		 const { return { pos.x, pos.y + h * 0.5f }; }
	Vector2 getLeft()		 const { return { pos.x - w * 0.5f, pos.y }; }
	Vector2 getRight()		 const { return { pos.x + w * 0.5f, pos.y }; }
	Vector2 getTopLeft()	 const { return { pos.x - w * 0.5f, pos.y - h * 0.5f }; }
	Vector2 getTopRight()    const { return { pos.x + w * 0.5f, pos.y - h * 0.5f }; }
	Vector2 getBottomLeft()	 const { return { pos.x - w * 0.5f, pos.y + h * 0.5f }; }
	Vector2 getBottomRight() const { return { pos.x + w * 0.5f, pos.y + h * 0.5f }; }

	// Get the "Axis Aligned Bounding Box". Mean Box with no collisions and is used to render entities
	// This will give us the rectangle that raylib wants in its draw function to draw the entity
	Rectangle getAABB() {
		return { pos.x - w * 0.5f, pos.y - h * 0.5f, w, h }; // Start from top left position of the entity, and add the its width and height to get its size
	}

	// Collision detection
	
	// Does a point of another entity intersects with the AABB of the entity ?
	bool intersectPoint(Vector2 point, float delta = 0) {
		Rectangle aabb = getAABB();
		aabb.x -= delta;
		aabb.y -= delta;
		aabb.width += 2 * delta;
		aabb.height += 2 * delta;

		return CheckCollisionPointRec(point, aabb);
	}

	// Does an entity's aabb intersects with this entity's aabb ?
	bool intersectTransform(Transform2D other, float delta = 0) {
		Rectangle a = getAABB();
		Rectangle b = other.getAABB();
		
		a.x -= delta;
		a.y -= delta;
		a.width += 2 * delta;
		a.height += 2 * delta;

		b.x -= delta;
		b.y -= delta;
		b.width += 2 * delta;
		b.height += 2 * delta;

		return CheckCollisionRecs(a, b);
	}
};
