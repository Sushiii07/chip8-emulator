#include "Chip8.hpp"
#include "Display.hpp"

#include <cstdint>
#include <iostream>
#include <bitset>
#include <fstream>



int main(int argc, char* args[]) {
	Chip8 chip8("./1-chip8-logo.ch8");
	chip8.loadFonts();

	Display display;

	bool quit = false;

	display.init();

	//chip8.test();

	//for (int i = 0; i < 101; i++) {
	//	// Print each byte in 2-digit hex
	//	std::cout << std::hex << static_cast<int>(chip8.memory[0x1FF + i]) << " ";

	//	// Optional: newline every 16 bytes
	//	if ((i + 1) % 16 == 0) {
	//		std::cout << "\n";
	//	}
	//}

	while (quit == false) {
		quit = display.processInput();
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
				switch (N) {
					case 0: {
						chip8.op_00E0(); // Clear the display
						break;
					}
					case 0x0E: {
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
		}
	}

	display.close();

	return 0;


}
