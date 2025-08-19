#include <cstdint>
#include <SDL3/SDL.h>
#include <iostream>
#include <bitset>
#include <fstream>

class Chip8 {
public:
	uint8_t memory[4096]{};
	uint16_t pc{};
	uint16_t index{};
	uint16_t stack[16]{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint8_t registers[16]{};
	uint8_t display[64 * 32]{};

	Chip8(std::string filePath) {
		std::ifstream file(filePath, std::ios::binary | std::ios::ate);
		if (!file) {
			throw std::runtime_error("Failed to open file: " + filePath);
		}
		std::streamsize fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		if (!file.read(reinterpret_cast<char*>(&memory[0x200]), fileSize)) {
			throw std::runtime_error("Failed to read file: " + filePath);
		}

		file.close();
	}

	void loadFonts() {
		uint8_t fonts[80] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
							0x20, 0x60, 0x20, 0x20, 0x70,	// 1
							0xF0, 0x10, 0xF0, 0x80, 0xF0,	// 2
							0xF0, 0x10, 0xF0, 0x10, 0xF0,	// 3
							0x90, 0x90, 0xF0, 0x10, 0x10,	// 4
							0xF0, 0x80, 0xF0, 0x10, 0xF0,	// 5
							0xF0, 0x80, 0xF0, 0x90, 0xF0,	// 6
							0xF0, 0x10, 0x20, 0x40, 0x40,	// 7
							0xF0, 0x90, 0xF0, 0x90, 0xF0,	// 8
							0xF0, 0x90, 0xF0, 0x10, 0xF0,	// 9
							0xF0, 0x90, 0xF0, 0x90, 0x90,	// A
							0xE0, 0x90, 0xE0, 0x90, 0xE0,	// B
							0xF0, 0x80, 0x80, 0x80, 0xF0,	// C
							0xE0, 0x90, 0x90, 0x90, 0xE0,	// D
							0xF0, 0x80, 0xF0, 0x80, 0xF0,	// E
							0xF0, 0x80, 0xF0, 0x80, 0x80 };	// F

		for (int i = 0x50; i <= 0x9f; i++) {
			memory[i] = fonts[i - 0x50];
		}
	}

	void op_00E0() {
		memset(display, 0, sizeof(display));
	}

	void test() {
		for (int i = 0; i < 64 * 32; i += 5) {
			display[i] = 1;
		}
	}

};

class Display {
public:
	static constexpr int WIDTH{ 64 };
	static constexpr int HEIGHT{ 32 };
	static constexpr int SCALE{ 10 };
	

	SDL_Window* gWindow{ nullptr };
	SDL_Renderer* gRenderer{ nullptr };
	SDL_Texture* gTexture{ nullptr };


	bool init() {
		bool success{ true };

		if (SDL_Init(SDL_INIT_VIDEO) == false) {
			SDL_Log("SD could not initialize! SDL error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			gWindow = SDL_CreateWindow("CHIP-8 Emulator", WIDTH*SCALE, HEIGHT*SCALE, 0);
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

	void close() {
		SDL_DestroyRenderer(gRenderer);
		SDL_DestroyWindow(gWindow);
		gRenderer = nullptr;
		gWindow = nullptr;

		SDL_Quit();
	}

	bool processInput() {
		SDL_Event e;
		SDL_zero(e);
		while (SDL_PollEvent(&e) == true) {
			if (e.type == SDL_EVENT_QUIT) {
				return true;
			}
		}
		return false;
	}

	void updateDisplay(uint8_t displayBuffer[64 * 32]) {
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
};

int main(int argc, char* args[]) {
	Chip8 chip8("./1-chip8-logo.ch8");
	chip8.loadFonts();

	Display display;

	bool quit = false;

	display.init();

	chip8.test();

	while (quit == false) {
		quit = display.processInput();
		display.updateDisplay(chip8.display);
	}

	display.close();

	return 0;


}
