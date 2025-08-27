#include "Chip8.hpp"
#include <fstream>
#include <random>
#include <SDL3/SDL.h>
#include <chrono>

Chip8::Chip8(std::string filePath, bool modern) {
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

	pc = 0x200;
	lastTimerUpdate = std::chrono::steady_clock::now();
	this->modern = modern;
}

void Chip8::setKeypad(uint8_t keypad[16]) {
	for (int i = 0; i < 16; i++) {
		this->keypad[i] = keypad[i];
	}
}

void Chip8::loadFonts() {
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

uint16_t Chip8::fetchInstruction() {
	if (pc + 1 >= 4096) {
		return 0x0000;
	}
	uint16_t instruction = (memory[pc] << 8) + memory[pc+1];
	pc = pc + 2;
	return instruction;
}

void Chip8::updateTimers() {
	auto now = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimerUpdate).count();

	if (elapsed >= 16) {
		if (delayTimer > 0) {
			delayTimer--;
		}
		if (soundTimer > 0) {
			soundTimer--;
		}
		lastTimerUpdate = now;
	}
}

void Chip8::op_00E0() {
	memset(display, 0, sizeof(display));
}
void Chip8::op_1NNN(uint16_t NNN) {
	pc = NNN;
}
void Chip8::op_00EE() {
	sp--;
	pc = stack[sp];
}
void Chip8::op_2NNN(uint16_t NNN) {
	stack[sp] = pc;
	sp++;
	pc = NNN;
}
void Chip8::op_3XNN(uint8_t X, uint8_t NN) {
	if (registers[X] == NN) {
		pc += 2;
	}
}
void Chip8::op_4XNN(uint8_t X, uint8_t NN) {
	if (registers[X] != NN) {
		pc += 2;
	}
}
void Chip8::op_5XY0(uint8_t X, uint8_t Y) {
	if (registers[X] == registers[Y]) {
		pc += 2;
	}
}
void Chip8::op_9XY0(uint8_t X, uint8_t Y) {
	if (registers[X] != registers[Y]) {
		pc += 2;
	}
}
void Chip8::op_6XNN(uint8_t X, uint8_t NN) {
	registers[X] = NN;
}
void Chip8::op_7XNN(uint8_t X, uint8_t NN) {
	registers[X] += NN;
}
void Chip8::op_8XY0(uint8_t X, uint8_t Y) {
	registers[X] = registers[Y];
}
void Chip8::op_8XY1(uint8_t X, uint8_t Y) {
	registers[X] = registers[X] | registers[Y];
}
void Chip8::op_8XY2(uint8_t X, uint8_t Y) {
	registers[X] = registers[X] & registers[Y];
}
void Chip8::op_8XY3(uint8_t X, uint8_t Y) {
	registers[X] = registers[X] ^ registers[Y];
}
void Chip8::op_8XY4(uint8_t X, uint8_t Y) {
	uint16_t sum = registers[X] + registers[Y];
	registers[X] = sum & 0xFF;
	registers[0xF] = sum > 0xFF;
}
void Chip8::op_8XY5(uint8_t X, uint8_t Y) {
	uint8_t x = registers[X];
	uint8_t y = registers[Y];
	registers[X] = x - y;
	registers[0xF] = x >= y;
}
void Chip8::op_8XY7(uint8_t X, uint8_t Y) {
	uint8_t flag = registers[Y] >= registers[X];
	registers[X] = registers[Y] - registers[X];
	registers[0xF] = flag;
}
void Chip8::op_8XY6(uint8_t X, uint8_t Y) {
	if (!modern) {
		registers[X] = registers[Y];
	}
	uint8_t flag = registers[X] & 0x01;
	registers[X] = registers[X] >> 1;
	registers[0xF] = flag;
}
void Chip8::op_8XYE(uint8_t X, uint8_t Y) {
	if (!modern) {
		registers[X] = registers[Y];
	}
	uint8_t flag = (registers[X] & 0x80) >> 7;
	registers[X] = registers[X] << 1;
	registers[0xF] = flag;
}
void Chip8::op_ANNN(uint16_t NNN) {
	index = NNN;
}
void Chip8::op_BNNN(uint16_t NNN) {
	if (!modern) {
		pc = NNN + registers[0];
	}
	else {
		uint8_t X = (NNN & 0x0F00) >> 8;
		pc = NNN + registers[X];
	}
}
void Chip8::op_CXNN(uint8_t X, uint8_t NN) {
	std::random_device r;
	uint8_t rand = r();
	registers[X] = rand & NN;
}
void Chip8::op_DXYN(uint8_t VX, uint8_t VY, uint8_t N) {
	uint8_t X = registers[VX] % 64;
	uint8_t Y = registers[VY] % 32;

	registers[0xF] = 0;

	for (int row = Y; row < Y + N; row++) {
		if (row >= 32) {
			break;
		}
		uint8_t sprite = memory[index + row - Y];
		for (uint8_t col = 0; col < 8; col++) {
			if (X + col >= 64) {
				break;
			}
			uint8_t pixel = (sprite >> (7 - col)) & 1;
			if (pixel) {
				if (display[row * 64 + X + col]) {
					registers[0xF] = 1;
				}
				display[row * 64 + X + col] ^= 1;
			}
		}
	}
}
void Chip8::op_EX9E(uint8_t X) {
	if (keypad[registers[X]]) {
		pc += 2;
	}
}
void Chip8::op_EXA1(uint8_t X) {
	if (!keypad[registers[X]]) {
		pc += 2;
	}
}
void Chip8::op_FX07(uint8_t X) {
	registers[X] = delayTimer;
}
void Chip8::op_FX15(uint8_t X) {
	delayTimer = registers[X];
}
void Chip8::op_FX18(uint8_t X) {
	soundTimer = registers[X];
}
void Chip8::op_FX1E(uint8_t X) {
	index += registers[X];
	if (modern) {
		if (index > 0x0FFF) {
			registers[0xF] = 1;
		}
		else {
			registers[0xF] = 0;
		}
	}
}
void Chip8::op_FX0A(uint8_t X) {
	static bool waitingForRelease = false;
	if (!waitingForRelease) {
		for (uint8_t i = 0; i < 16; i++) {
			if (keypad[i] != 0) {
				registers[X] = i;
				waitingForRelease = true;
				pc -= 2;
				return;
			}
		}
		pc -= 2;
	}
	else {
		bool anyPressed = false;
		for (uint8_t i = 0; i < 16; i++) {
			if (keypad[i] != 0) {
				anyPressed = true;
				break;
			}
		}
		if (!anyPressed) {
			waitingForRelease = false;
 		}
		else {
			pc -= 2;
		}
	}
}
void Chip8::op_FX29(uint8_t X) {
	index = registers[X];
}
void Chip8::op_FX33(uint8_t X) {
	int n = registers[X];
	memory[index + 2] = n % 10;
	n /= 10;
	memory[index + 1] = n % 10;
	n /= 10;
	memory[index] = n % 10;
}
void Chip8::op_FX55(uint8_t X) {
	if (!modern) {
		for (int i = 0x0; i <= X; i++) {
			memory[index] = registers[i];
			index++;
		}
	}
	else {
		for (int i = 0x0; i <= X; i++) {
			memory[index+i] = registers[i];
		}
	}
}
void Chip8::op_FX65(uint8_t X) {
	if (!modern) {
		for (int i = 0x0; i <= X; i++) {
			registers[i] = memory[index];
			index++;
		}
	}
	else {
		for (int i = 0x0; i <= X; i++) {
			registers[i] = memory[index+i];
		}
	}
}