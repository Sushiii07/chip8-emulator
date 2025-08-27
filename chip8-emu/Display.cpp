#include "Display.hpp"
#include <SDL3/SDL.h>


bool Display::init() {
	bool success{ true };

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == false) {
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

	SDL_zero(spec);
	spec.channels = 1;
	spec.format = SDL_AUDIO_F32;
	spec.freq = 48000;

	device = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
	if (!device) {
		SDL_Log("Failed to open audio: %s", SDL_GetError());
	}
	SDL_ResumeAudioStreamDevice(device);

	bufferSize = spec.freq;
	beepBuffer = new float[bufferSize];

	int period = spec.freq / 440;
	for (int i = 0; i < bufferSize; i++) {
		beepBuffer[i] = (i % period < period / 2) ? 0.25f : -0.25f;
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

void Display::beep(bool enable) {
	if (!device) {
		return;
	}
	if (enable && !isBeeping) {
		// start looping beep
		SDL_PutAudioStreamData(device, beepBuffer, bufferSize * sizeof(float));
		isBeeping = true;
	}
	else if (!enable && isBeeping) {
		// stop beep by clearing stream
		SDL_ClearAudioStream(device);
		isBeeping = false;
	}
}