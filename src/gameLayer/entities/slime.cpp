#include "slime.h"
#include <assetManager.h>
#include "helpers.h"
#include <randomStuff.h>

void Slime::render(AssetManager &assetManager)
{
	auto aabb = getRectangleForEntity(physics.transform, 1, 1);

	DrawTexturePro(
		assetManager.slime,
		getTextureAtlas(animation.positionX, animation.positionY, 32, 32),
		aabb, // dest
		{0,0}, // origin
		0.0f, // rotation
		WHITE // tint
	);

	// notice we don't update the animation during rendering because
	// if for some reason we want to render the entity twice we shouldn't update it here as well 
	// or we may want to update the animations even if we don't want to render the entity
}

bool Slime::update(float deltaTime, EntityUpdateData entityUpdateData)
{
	changeStateTimer -= deltaTime;

	if (changeStateTimer < 0) {
		changeStateTimer = getRandomFloat(entityUpdateData.rng, 1, 7);

		float distanceToPlayer = Vector2Distance(entityUpdateData.playerPosition, getPosition());

		if (distanceToPlayer < 20) {
			if (getRandomChance(entityUpdateData.rng, 0.8)) {
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

	// every frame we set the animation, based on whether the slime touches the ground or not.
	if (physics.downTouch) {
		moveSpeed = 0; // we set that to 0 when we touch the ground, so that it doesn't move
		animation.setAnimation(0); // staying
	}
	else {
		animation.setAnimation(1); // in air
	}

	jumpTimer -= deltaTime;

	switch (currentState) {
		case STATE_WANDERING: {

			// if we just move around we just pick a random direction (moveSpeed) and reset the jump timer
			if (jumpTimer < 0) {
				jumpTimer = getRandomFloat(entityUpdateData.rng, 3, 12);
				physics.jump(10);
				moveSpeed = getRandomFloat(entityUpdateData.rng, -7, 7);
			}
		}
		break; 

		case STATE_CHASING: {
			if (jumpTimer < 0) {
				jumpTimer = getRandomFloat(entityUpdateData.rng, 2, 7); // timer set to a random value so that we jump more often and catch out the player
				physics.jump(10);

				if (entityUpdateData.playerPosition.x > getPosition().x) {
					moveSpeed = getRandomFloat(entityUpdateData.rng, 1, 7);
				}
				else {
					moveSpeed = -getRandomFloat(entityUpdateData.rng, 1, 7);
				}
			}
		}
		break;
	}

	// if the move speed of the variable is still set, that means we are still in the air and we use it to move
	if (moveSpeed) {
		getPosition().x += deltaTime * moveSpeed;
	}

	animation.update(deltaTime, 0.08, 7);

	return true;
}
