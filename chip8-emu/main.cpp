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


};

class Display {
public:
	static constexpr int screenWidth{ 640 };
	static constexpr int screenHeight{ 320 };

	SDL_Window* gWindow{ nullptr };
	SDL_Renderer* gRenderer{ nullptr };


	bool init() {
		bool success{ true };

		if (SDL_Init(SDL_INIT_VIDEO) == false) {
			SDL_Log("SD could not initialize! SDL error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			gWindow = SDL_CreateWindow("CHIP-8 Emulator", screenWidth, screenHeight, 0);
			if (gWindow == nullptr) {
				SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				gRenderer = SDL_CreateRenderer(gWindow, nullptr);
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

	int start() {
		int exitCode{ 0 };

		if (init() == false) {
			SDL_Log("Unable to initialize program!\n");
			exitCode = 1;
		}
		else {
			SDL_FRect rect = { 0, 0, 10, 10 };
			SDL_Color rect_color = { 255, 255, 255, 255 };
			bool quit{ false };

			SDL_Event e;
			SDL_zero(e);

			while (quit == false) {

				
				while (SDL_PollEvent(&e) == true) {
					if (e.type == SDL_EVENT_QUIT) {
						quit = true;
					}
				}
				
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
				SDL_RenderClear(gRenderer);

				SDL_SetRenderDrawColor(gRenderer, rect_color.r, rect_color.g, rect_color.b, rect_color.a);
				SDL_RenderFillRect(gRenderer, &rect);

				SDL_RenderPresent(gRenderer);
				SDL_UpdateWindowSurface(gWindow);
			}
		}

		return exitCode;
	}
};

int main(int argc, char* args[]) {
	Chip8 chip8("./1-chip8-logo.ch8");
	chip8.loadFonts();


	std::cout << "hello" << std::endl;
	for (int i = 0; i < 100; i++) {
		std::cout << std::hex << static_cast<int>(chip8.memory[i + 0x200]) << " ";
	}
	//for (int i = 0; i < 80; i++) {
	//	std::cout << std::bitset<8>(chip8.memory[i + 0x50]) << std::endl;
	//	if ((i+1) % 5 == 0) {
	//		std::cout << std::endl;
	//	}
	//}

	Display display;

	int exitCode = display.start();

	display.init();




	display.close();

	return exitCode;


}
