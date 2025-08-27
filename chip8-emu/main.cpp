#include "Chip8.hpp"
#include "Display.hpp"
#include <cstdint>
#include <fstream>

uint8_t keypad[16] = { 0 };
int keymap[16] = {
	SDL_SCANCODE_X,    // 0
	SDL_SCANCODE_1,    // 1
	SDL_SCANCODE_2,    // 2
	SDL_SCANCODE_3,    // 3
	SDL_SCANCODE_Q,    // 4
	SDL_SCANCODE_W,    // 5
	SDL_SCANCODE_E,    // 6
	SDL_SCANCODE_A,    // 7
	SDL_SCANCODE_S,    // 8
	SDL_SCANCODE_D,    // 9
	SDL_SCANCODE_Z,    // A
	SDL_SCANCODE_C,    // B
	SDL_SCANCODE_4,    // C
	SDL_SCANCODE_R,    // D
	SDL_SCANCODE_F,    // E
	SDL_SCANCODE_V     // F
};

int main(int argc, char* args[]) {
	Chip8 chip8("./6-keypad.ch8", true);
	chip8.loadFonts();

	Display display;

	bool quit = false;

	display.init();

	

	while (quit == false) {
		SDL_Event e;

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				quit = true;
			}
			/*else if (e.type == SDL_EVENT_KEY_DOWN) {
				for (int i = 0; i < 16; i++) {
					if (e.key.scancode == keymap[i]) {
						keypad[i] = 1;
					}
				}
			}
			else if (e.type == SDL_EVENT_KEY_UP) {
				for (int i = 0; i < 16; i++) {
					if (e.key.scancode == keymap[i]) {
						keypad[i] = 0;
					}
				}
			}*/

			else if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
				for (int i = 0; i < 16; i++) {
					if (e.key.scancode == keymap[i]) {
						keypad[i] = (e.type == SDL_EVENT_KEY_DOWN);
					}
				}
			}
		}
		chip8.setKeypad(keypad);
		display.beep(chip8.soundTimer);
		display.updateDisplay(chip8.display);
	
		uint16_t instruction = chip8.fetchInstruction();

		uint8_t type = (instruction & 0xF000) >> 12;
		uint8_t X = (instruction & 0x0F00) >> 8;
		uint8_t Y = (instruction & 0x00F0) >> 4;
		uint8_t N = instruction & 0x000F;
		uint8_t NN = instruction & 0x00FF;
		uint16_t NNN = instruction & 0x0FFF;

		switch (type) {
			case 0: {
				switch (NN) {
					case 0x00: {
						break;
					}
					case 0xE0: {
						chip8.op_00E0(); // Clear the display
						break;
					}
					case 0xEE: {
						chip8.op_00EE(); // Return from Subroutine
						break;
					}
				}
				break;
			}
			case 1: {
				chip8.op_1NNN(NNN);		// Jump to NNN
				break;
			}
			case 2: {
				chip8.op_2NNN(NNN);		// Subroutine
				break;
			}
			case 3: {
				chip8.op_3XNN(X, NN);	// Skip conditionally
				break;
			}
			case 4: {
				chip8.op_4XNN(X, NN);	// Skip conditionally
				break;
			}
			case 5: {
				chip8.op_5XY0(X, Y);	// Skip conditionally
				break;
			}
			case 9: {
				chip8.op_9XY0(X, Y);	// Skip conditionally
				break;
			}
			case 6: {
				chip8.op_6XNN(X, NN);
				break;
			}
			case 7: {
				chip8.op_7XNN(X, NN);
				break;
			}
			case 8: {
				switch (N) {
					case 0: {
						chip8.op_8XY0(X, Y);
						break;
					}
					case 1: {
						chip8.op_8XY1(X, Y);
						break;
					}
					case 2: {
						chip8.op_8XY2(X, Y);
						break;
					}
					case 3: {
						chip8.op_8XY3(X, Y);
						break;
					}
					case 4: {
						chip8.op_8XY4(X, Y);
						break;
					}
					case 5: {
						chip8.op_8XY5(X, Y);
						break;
					}
					case 6: {
						chip8.op_8XY6(X, Y);
						break;
					}
					case 7: {
						chip8.op_8XY7(X, Y);
						break;
					}
					case 0xE: {
						chip8.op_8XYE(X, Y);
						break;
					}
				}
				break;
			}
			case 0xA: {
				chip8.op_ANNN(NNN);
				break;
			}
			case 0xB: {
				chip8.op_BNNN(NNN);
				break;
			}
			case 0xC: {
				chip8.op_CXNN(X, NN);
				break;
			}
			case 0xD: {
				chip8.op_DXYN(X, Y, N);
				break;
			}
			case 0xE: {
				switch (Y) {
					case 9: {
						chip8.op_EX9E(X);
						break;
					}
					case 0xA: {
						chip8.op_EXA1(X);
						break;
					}
				}
				break;
			}
			case 0xF: {
				switch (NN) {
					case 0x07: {
						chip8.op_FX07(X);
						break;
					}
					case 0x15: {
						chip8.op_FX15(X);
						break;
					}
					case 0x18: {
						chip8.op_FX18(X);
						break;
					}
					case 0x1E: {
						chip8.op_FX1E(X);
						break;
					}
					case 0x0A: {
						chip8.op_FX0A(X);
						break;
					}
					case 0x29: {
						chip8.op_FX29(X);
						break;
					}
					case 0x33: {
						chip8.op_FX33(X);
						break;
					}
					case 0x55: {
						chip8.op_FX55(X);
						break;
					}
					case 0x65: {
						chip8.op_FX65(X);
						break;
					}
				}
				break;
			}
		}
		chip8.updateTimers();
		display.beep(chip8.soundTimer > 0);

	}

	display.close();

	return 0;


}
