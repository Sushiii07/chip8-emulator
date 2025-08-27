#pragma once
#include <cstdint>
#include <SDL3/SDL.h>

class Display {
public:
	static constexpr int WIDTH{ 64 };
	static constexpr int HEIGHT{ 32 };
	static constexpr int SCALE{ 10 };


	SDL_Window* gWindow{ nullptr };
	SDL_Renderer* gRenderer{ nullptr };
	SDL_Texture* gTexture{ nullptr };

	SDL_AudioStream* device{ nullptr };
	SDL_AudioSpec spec;
	float* beepBuffer;
	int bufferSize;
	bool isBeeping = false;


	bool init();
	void close();
	void updateDisplay(uint8_t displayBuffer[64 * 32]);
	void beep(bool enable);
};