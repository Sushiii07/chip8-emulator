#pragma once
#include <cstdint>
#include <string>

class Chip8 {
public:
	Chip8(std::string filePath);

	uint8_t memory[4096]{};
	uint16_t pc{};
	uint16_t index{};
	uint16_t stack[16]{};
	uint8_t sp{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint8_t registers[16]{};
	uint8_t display[64 * 32]{};
	uint8_t keypad[16] = { 0 };

	void setKeypad(uint8_t keypad[16]);
	void loadFonts();
	void test();
	uint16_t fetchInstruction();
	void op_00E0();
	void op_1NNN(uint16_t NNN);
	void op_00EE();
	void op_2NNN(uint16_t NNN);
	void op_3XNN(uint8_t X, uint8_t NN);
	void op_4XNN(uint8_t X, uint8_t NN);
	void op_5XY0(uint8_t X, uint8_t Y);
	void op_9XY0(uint8_t X, uint8_t Y);
	void op_6XNN(uint8_t X, uint8_t NN);
	void op_7XNN(uint8_t X, uint8_t NN);
	void op_8XY0(uint8_t X, uint8_t Y);
	void op_8XY1(uint8_t X, uint8_t Y);
	void op_8XY2(uint8_t X, uint8_t Y);
	void op_8XY3(uint8_t X, uint8_t Y);
	void op_8XY4(uint8_t X, uint8_t Y);
	void op_8XY5(uint8_t X, uint8_t Y);
	void op_8XY6(uint8_t X, uint8_t Y);
	void op_8XY7(uint8_t X, uint8_t Y);
	void op_8XYE(uint8_t X, uint8_t Y);
	void op_ANNN(uint16_t NNN);
	void op_BNNN(uint16_t NNN);
	void op_CXNN(uint8_t X, uint8_t NN);
	void op_DXYN(uint8_t VX, uint8_t VY, uint8_t N);
	void op_EX9E(uint8_t X);
	void op_EXA1(uint8_t X);
	void op_FX07(uint8_t X);
	void op_FX15(uint8_t X);
	void op_FX18(uint8_t X);
	void op_FX1E(uint8_t X);
	void op_FX0A(uint8_t X);
	void op_FX29(uint8_t X);
	void op_FX33(uint8_t X);
	void op_FX55(uint8_t X);
	void op_FX65(uint8_t X);

	void printDisplay();
};
