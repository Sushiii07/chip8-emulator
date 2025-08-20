#include "Chip8.hpp"
#include <fstream>
#include <random>
#include <iostream>

Chip8::Chip8(std::string filePath) {
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

void Chip8::test() {
	for (int i = 0; i < 64 * 32; i += 5) {
		display[i] = 1;
	}
}

void Chip8::printDisplay() {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			std::cout << std::hex << static_cast<int>(display[i * 64 + j]);
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

}

uint16_t Chip8::fetchInstruction() {
	if (pc + 1 >= 4096) {
		return 0xFFFF;
	}
	uint16_t instruction = (memory[pc] << 8) + memory[pc+1];
	pc = pc + 2;
	return instruction;
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
	stack[sp] = pc - 2;
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
	registers[0xF] = (sum > 0xFF);
	registers[X] = sum & 0xFF;
}
void Chip8::op_8XY5(uint8_t X, uint8_t Y) {
	registers[0xF] = registers[X] > registers[Y];
	registers[X] = registers[X] - registers[Y];
}
void Chip8::op_8XY7(uint8_t X, uint8_t Y) {
	registers[0xF] = registers[Y] > registers[X];
	registers[X] = registers[Y] - registers[X];
}
void Chip8::op_8XY6(uint8_t X, uint8_t Y) {
	registers[X] = registers[Y];
	registers[0xF] = registers[X] & 0x01;
	registers[X] = registers[X] >> 1;

}
void Chip8::op_8XYE(uint8_t X, uint8_t Y) {
	registers[X] = registers[Y];
	registers[0xF] = registers[X] & 0x80;
	registers[X] = registers[X] << 1;
}
void Chip8::op_ANNN(uint16_t NNN) {
	index = NNN;
}
void Chip8::op_BNNN(uint16_t NNN) {
	pc = NNN + registers[0];
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
		uint8_t sprite = memory[index];
		for (uint8_t col = 0; col < 8; col++) {
			if (X + col >= 64) {
				break;
			}
			if ((sprite & (1 << (7 - col))) && display[row * 64 + X + col]) {
				display[row * 64 + X + col] = 0;
				registers[0xF] = 1;
			}
			else if (sprite & (1 << (7 - col))) {
				display[row * 64 + X + col] = 1;
			}
		}
		index++;
	}
}

