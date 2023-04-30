#pragma once

#include <span>

/**
 * @brief A configurable Finite State Machine intended to be used for the parsing of input data.
 * 
 * Details on internal behavior and the expected formatting of the lookup tables is described in `FSM.md`.
 */
class FSM final {
public:
	constexpr FSM() noexcept : equivalenceTable(nullptr) {}
	constexpr FSM(unsigned char (*equivalenceTable)[256], unsigned char numSymbols, unsigned char *transitionTable, unsigned char outputStateCutoff) noexcept;
	constexpr ~FSM() noexcept = default;

	constexpr size_t parse(const std::span<const char> input, const std::span<char> output) const noexcept;

private:
	unsigned char (*equivalenceTable)[256];
	unsigned char *transitionTable;
	unsigned char numSymbols;
	unsigned char outputStateCutoff;
};