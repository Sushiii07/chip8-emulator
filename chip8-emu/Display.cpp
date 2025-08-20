#include "Display.hpp"
#include <SDL3/SDL.h>


bool Display::init() {
	bool success{ true };

	if (SDL_Init(SDL_INIT_VIDEO) == false) {
		SDL_Log("SD could not initialize! SDL error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		gWindow = SDL_CreateWindow("CHIP-8 Emulator", WIDTH * SCALE, HEIGHT * SCALE, 0);
		if (gWindow == nullptr) {
			SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			gRenderer = SDL_CreateRenderer(gWindow, nullptr);
			gTexture = SDL_CreateTexture(
				gRenderer,
				SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_STREAMING,
				WIDTH,
				HEIGHT
			);
			SDL_SetTextureScaleMode(gTexture, SDL_SCALEMODE_NEAREST);
		}
	}
	return success;
}

void Display::close() {
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gRenderer = nullptr;
	gWindow = nullptr;

	SDL_Quit();
}

bool Display::processInput() {
	SDL_Event e;
	SDL_zero(e);
	while (SDL_PollEvent(&e) == true) {
		if (e.type == SDL_EVENT_QUIT) {
			return true;
		}
	}
	return false;
}

void Display::updateDisplay(uint8_t displayBuffer[64 * 32]) {
	uint32_t* pixels;
	int pitch;
	SDL_LockTexture(gTexture, NULL, (void**)&pixels, &pitch);

	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			int index = i * 64 + j;
			uint32_t color = displayBuffer[index] ? 0xFFFFFFFF : 0xFF000000;
			pixels[i * (pitch / 4) + j] = color;
		}
	}

	SDL_UnlockTexture(gTexture);

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);

	SDL_FRect dstRect = { 0, 0, WIDTH * SCALE, HEIGHT * SCALE };
	SDL_RenderTexture(gRenderer, gTexture, NULL, &dstRect);

	SDL_RenderPresent(gRenderer);
}