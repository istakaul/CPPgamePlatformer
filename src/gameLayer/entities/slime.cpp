#include "slime.h"
#include <assetManager.h>
#include "helpers.h"
#include <randomStuff.h>

void Slime::render(AssetManager &assetManager)
{
	auto aabb = getRectangleForEntity(physics.transform, 1, 1);

	DrawTexturePro(
		assetManager.slime,
		getTextureAtlas(0, 0, 32, 32),
		aabb, // dest
		{0,0}, // origin
		0.0f, // rotation
		WHITE // tint
	);
}

void Slime::update(float deltaTime, std::ranlux24_base rng, Vector2 playerPosition)
{
	changeStateTimer -= deltaTime;

	if (changeStateTimer < 0) {
		changeStateTimer = getRandomFloat(rng, 1, 7);

		float distanceToPlayer = Vector2Distance(playerPosition, getPosition());

		if (distanceToPlayer < 20) {
			if (getRandomChance(rng, 0.8)) {
				currentState = STATE_CHASING;
			}
			else {
				currentState = STATE_WANDERING;
			}
		}
		else {
			currentState = STATE_WANDERING;
		}
	}

	if (physics.downTouch) {
		moveSpeed = 0; // we set that to 0 when we touch the ground, so that it doesn't move
	}

	jumpTimer -= deltaTime;

	switch (currentState) {
		case STATE_WANDERING: {

			// if we just move around we just pick a random direction (moveSpeed) and reset the jump timer
			if (jumpTimer < 0) {
				jumpTimer = getRandomFloat(rng, 3, 12);
				physics.jump(10);
				moveSpeed = getRandomFloat(rng, -7, 7);
			}
		}
		break; 

		case STATE_CHASING: {
			if (jumpTimer < 0) {
				jumpTimer = getRandomFloat(rng, 2, 7); // timer set to a random value so that we jump more often and catch out the player
				physics.jump(10);

				if (playerPosition.x > getPosition().x) {
					moveSpeed = getRandomFloat(rng, 1, 7);
				}
				else {
					moveSpeed = -getRandomFloat(rng, 1, 7);
				}
			}
		}
		break;
	}

	// if the move speed of the variable is still set, that means we are still in the air and we use it to move
	if (moveSpeed) {
		getPosition().x += deltaTime * moveSpeed;
	}
}
