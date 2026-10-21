#include <raylib.h>
#include "gameMain.h"
#include <gameMap.h>
#include <structure.h>
#include <saveMap.h>
#include <iostream>
#include <fstream>
#include <asserts.h>
#include <assetManager.h>
#include <helpers.h>
#include <imgui.h>
#include <rlImGui.h>
#include <../ImGui_Theme/BlackDevil.h>

struct GameData {
	GameMap gameMap;
	Camera2D camera;

	int creativeSelectedBlock = Block::air; // currently selected block

	// These are map points where we select an item on the map
	Vector2 selectionStart = {};
	Vector2 selectionEnd = {};
	Structure copyStructure;

	char saveName[100] = {};

} gameData;

AssetManager assetManager;

static float CAMERA_SPEED = 50;

bool isShowImgui = false;

bool initGame()
{
	assetManager.loadAll();

	gameData.gameMap.create(600, 300);
	
	gameData.gameMap.getBlockUnsafe(0, 0).type = Block::dirt;
	gameData.gameMap.getBlockUnsafe(1, 1).type = Block::grass;
	gameData.gameMap.getBlockUnsafe(2, 2).type = Block::blueRubyBlock;
	gameData.gameMap.getBlockUnsafe(3, 3).type = Block::door;
	gameData.gameMap.getBlockUnsafe(4, 4).type = Block::platform;

	gameData.camera.target = { 0.f, 0.f }; // world-space center of view
	gameData.camera.rotation = 0.f; // no rotation
	gameData.camera.zoom = 100.f; // no zoom

	return true;
}

bool updateGame()
{

	float deltaTime = GetFrameTime();
	if (deltaTime > 1.f / 5) { deltaTime = 1.f / 5; } // limit deltaTime to a maximum of 1/5 seconds (200ms) to avoid large jumps

	gameData.camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f }; // center of the screen

	ClearBackground({ 75, 75, 150, 255 }); // clear the screen with a dark green color

	if (IsKeyPressed(KEY_F10)) {
		isShowImgui = !isShowImgui;
	}

#pragma region camera movement
	if (IsKeyDown(KEY_LEFT)) { gameData.camera.target.x -= 7.f * deltaTime; }
	if (IsKeyDown(KEY_RIGHT)) { gameData.camera.target.x += 7.f * deltaTime; }
	if (IsKeyDown(KEY_UP)) { gameData.camera.target.y -= 7.f * deltaTime; }
	if (IsKeyDown(KEY_DOWN)) { gameData.camera.target.y += 7.f * deltaTime; }
#pragma endregion

	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floor(worldPos.x);
	int blockY = (int)floor(worldPos.y);

	// make sure we cannot select an inexisting block
	if (gameData.creativeSelectedBlock < 0) { gameData.creativeSelectedBlock = 0; }
	if (gameData.creativeSelectedBlock >= Block::BLOCKS_COUNT) { gameData.creativeSelectedBlock = Block::BLOCKS_COUNT - 1; }

	levelDesignInput(blockX, blockY);

	BeginMode2D(gameData.camera);

	for (int y = 0; y < gameData.gameMap.h; y++) {
		for (int x = 0; x < gameData.gameMap.w; x++) {
			auto &b = gameData.gameMap.getBlockUnsafe(x, y);

			if (b.type != Block::air) {
				DrawTexturePro(
					assetManager.textures,
					getTextureAtlas(b.type, 0, 32, 32), //source
					{ (float)x, (float)y, 1, 1 }, //destination
					{ 0,0 }, //origin of block, where it will start drawing from - top left corner
					0.f, // rotation
					WHITE // tint
				);
			}
		}
	}

	// draw selected block (we draw it after the map so that it is displayed on top)
	if (gameData.creativeSelectedBlock > 0) {
		DrawTexturePro(
			assetManager.textures,
			getTextureAtlas(gameData.creativeSelectedBlock, 0, 32, 32),
			{ (float)blockX, (float)blockY,1,1 },
			{ 0,0 },
			0.f,
			WHITE
		);
	}
	else {

		DrawTexturePro(
			assetManager.frame,
			{0,0,(float)assetManager.frame.width, (float)assetManager.frame.height},
			{(float)blockX, (float)blockY,1,1},
			{0,0},
			0.f,
			WHITE
		);
	}

	if (isShowImgui) {
		
		Rectangle rect;
		rect.x = gameData.selectionStart.x;
		rect.y = gameData.selectionStart.y;

		rect.width = gameData.selectionEnd.x - gameData.selectionStart.x;
		rect.height = gameData.selectionEnd.y - gameData.selectionStart.y;

		rect.width++;
		rect.height++;

		DrawRectangleLinesEx(rect, 0.1, { 20, 101, 250, 145 });
	}
	
	EndMode2D();

	showImgui(isShowImgui);

	DrawFPS(10, 10);

	return true;
}

void levelDesignInput(int blockX, int blockY)
{
	if (isShowImgui) {
		if (IsKeyPressed(KEY_ONE)) { gameData.selectionStart = Vector2{ float(blockX), float(blockY) }; }
		if (IsKeyPressed(KEY_TWO)) { gameData.selectionEnd = Vector2{ float(blockX), float(blockY) }; }
		if(IsKeyPressed(KEY_THREE)) {
			gameData.copyStructure.pasteIntoMap(gameData.gameMap, Vector2{ float(blockX), float(blockY) });
		}

		// Make sure the start is smaller than the end
		if (gameData.selectionStart.x > gameData.selectionEnd.x) {
			std::swap(gameData.selectionStart.x, gameData.selectionEnd.x);
		}
		if (gameData.selectionStart.y > gameData.selectionEnd.y) {
			std::swap(gameData.selectionStart.y, gameData.selectionEnd.y);
		}
	}

	if (!isShowImgui) {

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			auto b = gameData.gameMap.getBlockSafe(blockX, blockY);
			if (b) {
				*b = {};
			}
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			auto b = gameData.gameMap.getBlockSafe(blockX, blockY);
			if (b) {
				b->type = gameData.creativeSelectedBlock;
			}
		}
	}
	else if (IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			auto b = gameData.gameMap.getBlockSafe(blockX, blockY);
			if (b) {
				gameData.creativeSelectedBlock = b->type;
			}
		}
}

void showImgui(bool isShowImgui)
{
	if (isShowImgui) {
		ImGui::Begin("Game Control");

		ImGui::SliderFloat("Camera zoom:", &gameData.camera.zoom, 1, 150);
		ImGui::SliderFloat("Camera speed", &CAMERA_SPEED, 5, 30);

		if (ImGui::Button("Copy")) {
			gameData.copyStructure.copyFromMap(gameData.gameMap,
				gameData.selectionStart, gameData.selectionEnd);
		}

		ImGui::InputText("File Name", gameData.saveName, sizeof(gameData.saveName));

		if (ImGui::Button("Save to File")) {
			std::string path = RESOURCES_PATH "structures/";
			path += gameData.saveName;
			path += ".bin";

			saveBlockDataToFile(gameData.copyStructure.mapData,
				gameData.copyStructure.w, gameData.copyStructure.h,
				path.c_str());
		}

		if (ImGui::Button("Load from File")) {
			std::string path = RESOURCES_PATH "structures/";
			path += gameData.saveName;
			path += ".bin";

			loadBlockDataFromFile(gameData.copyStructure.mapData,
				gameData.copyStructure.w, gameData.copyStructure.h,
				path.c_str());
		}


		ImGui::Separator();

		for (int i = 0; i < Block::BLOCKS_COUNT; i++) {
			auto atlas = getTextureAtlas(i, 0, 32, 32);
			atlas.x /= assetManager.textures.width;
			atlas.width /= assetManager.textures.width;
			atlas.y /= assetManager.textures.height;
			atlas.height /= assetManager.textures.height;
	
			ImGui::PushID(i);
			ImTextureID tex = (ImTextureID)(intptr_t)assetManager.textures.id;

			if (ImGui::ImageButton(tex, { 35,35 }, { atlas.x, atlas.y }, { atlas.x + atlas.width, atlas.y + atlas.height })) {
				gameData.creativeSelectedBlock = i;
			}

			ImGui::PopID();
			if (i % 10 != 0) {
				ImGui::SameLine();
			}
		}

		ImGui::InputInt("Select Block", &gameData.creativeSelectedBlock);

		ImGui::End();
	}
}

void closeGame()
{
	std::cout << "\n\nCLOSED!!!!!!!!!\n\n";

	std::ofstream f(RESOURCES_PATH "f.txt");
	f << "\nCLOSED\n";
	f.close();
}
