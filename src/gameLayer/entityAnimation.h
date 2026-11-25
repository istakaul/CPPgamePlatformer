#pragma once

struct EntityAnimation {
	// these are the coordinates of the starting frame
	int positionX = 0;
	int positionY = 0;

	// timer to move between frames. When it reaches 0 we move to the next frame
	float timer = 0;

	void update(float deltaTime, float frameDuration, int framesCount) {
		timer -= deltaTime;

		if (timer <= 0) {
			timer += frameDuration;
			positionX++;
		}

		positionX %= framesCount;
	}

	void setAnimation(int animation) {
		// reset the frame to 0, when changing the animation (we can call this method every frame, because it changes sth only if we change animation)
		if (positionY != animation) {
			positionX = 0;
			positionY = animation;
		}
	}
};