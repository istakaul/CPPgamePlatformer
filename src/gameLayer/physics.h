#pragma once
#include <raylib.h>
#include <raymath.h>
#include <cmath>


#pragma region Vector2 operator overloads
// Vector2 operator overloads
inline Vector2 operator+(const Vector2 &a, const Vector2 &b)
{
	return { a.x + b.x, a.y + b.y };
}

inline Vector2 operator-(const Vector2 &a, const Vector2 &b)
{
	return { a.x - b.x, a.y - b.y };
}

inline Vector2 operator*(const Vector2 &a, float scalar)
{
	return { a.x * scalar, a.y * scalar };
}

inline Vector2 operator/(const Vector2 &a, float scalar)
{
	return { a.x / scalar, a.y / scalar };
}


inline Vector2 &operator*=(Vector2 &a, float scalar)
{
	a.x *= scalar;
	a.y *= scalar;
	return a;
}

inline Vector2 &operator/=(Vector2 &a, float scalar)
{
	a.x /= scalar;
	a.y /= scalar;
	return a;
}

inline Vector2 &operator+=(Vector2 &a, float scalar)
{
	a.x += scalar;
	a.y += scalar;
	return a;
}

inline Vector2 &operator-=(Vector2 &a, float scalar)
{
	a.x -= scalar;
	a.y -= scalar;
	return a;
}


inline bool operator==(const Vector2 &a, const Vector2 &b)
{
	return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const Vector2 &a, const Vector2 &b)
{
	return !(a == b);
}

inline Vector2 &operator+=(Vector2 &a, const Vector2 &b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

inline Vector2 &operator-=(Vector2 &a, const Vector2 &b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

inline Vector2 &operator*=(Vector2 &a, const Vector2 &b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}

inline Vector2 &operator/=(Vector2 &a, const Vector2 &b)
{
	a.x /= b.x;
	a.y /= b.y;
	return a;
}

#pragma endregion

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

// forward declaration (we tell the compiler that GameMap already exists, and then include the GameMap.h in the physics.cpp where it will be used)
// You can do a forward declaration when you use a struct/class in a header file only as a reference/pointer (we use a reference of GameMap in the performCollisionOnOneAxis() method)
struct GameMap; 

struct PhysicalEntity {
	Transform2D transform;
	Vector2 lastPosition = {};

	Vector2 velocity = {};
	Vector2 acceleration = {};

	bool upTouch = 0;
	bool downTouch = 0;
	bool leftTouch = 0;
	bool rightTouch = 0;

	void teleport(Vector2 pos) {
		transform.pos = pos;
		lastPosition = pos;
	}

	void updateForces(float deltaTime) {
		velocity += acceleration * deltaTime; // h taxuthta einai sunarthsh ths allaghs ths epitaxunshs mesa sto xrono
		transform.pos += velocity * deltaTime; // h 8esh einai sunarthsh ths allaghs ths taxuthtas mesa sto xrono.

		// adraneia (friction)
		//velocity -= velocity * 0.1 * deltaTime;
		Vector2 dragVector = Vector2{ velocity.x * std::abs(velocity.x), velocity.y * std::abs(velocity.y) };
		float drag = 0.01f;

		// #include <raymath.h>
		if (Vector2Length(dragVector) * drag * deltaTime > Vector2Length(velocity)) {
			velocity = {};
		}
		else {
			velocity -= dragVector * drag * deltaTime;
		}

		if (Vector2Length(velocity) < 0.01) {
			velocity = {};
		}

		acceleration = {}; // we apply acceleration only when moving the object.

	}

	// called at the end of the frame for every entity
	// (we will use the lastPosition to substract it from the currentPosition in order to get the direction the entity is looking at)
	void updateFinal() {
		lastPosition = { transform.pos.x, transform.pos.y };
	}

	void applyGravity(){
		acceleration += {0, 20.0};
	}

	void jump(float force) {
		// we don't use acceleration because that is influenced by delta time,
		// we directly change the velocity with no delta time because we
		// want the force to happen in one frame only
		if (downTouch) {
			velocity.y = -force;
		}
	}

	Vector2 &getPosition() {
		return transform.pos;
	}

	// function to resolve the collisions
	void resolveConstraints(GameMap &mapData);

	void checkCollisionOnce(Vector2 &pos, GameMap &mapData);

	Vector2 performCollisionOnOneAxis(GameMap &mapData, Vector2 pos, Vector2 delta); // with the &mapData we get a reference to the original mapData and it's tile, whereas if it wasn't a reference (& was omitted) we would have got a copy 
};
