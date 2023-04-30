#include "FSM.h"

/**
 * @brief Constructs a valid Finite State Machine. The arrays pointed to by `equivalenceTable` and `transitionTable` must be valid for the lifetime of the object.
 * 
 * @param equivalenceTable a pointer to a 1D lookup table that converts input characters into internal symbols
 * @param numSymbols the number of internal symbols that the equivalence table maps to
 * @param transitionTable a pointer to a 2D lookup table that defines transitions between states
 * @param outputStateCutoff the id demarcating the beginning of output states
 */
constexpr FSM::FSM(unsigned char (*equivalenceTable)[256], unsigned char numSymbols, unsigned char *transitionTable, unsigned char outputStateCutoff) noexcept
	: equivalenceTable(equivalenceTable), transitionTable(transitionTable), numSymbols(numSymbols), outputStateCutoff(outputStateCutoff) {

}

/**
 * @brief Parses an input string into an output string based on the configuration of the FSM
 * 
 * @param input a span to the input string, must be terminated by a character guaranteeing a transition to the `outputStateCutoff` state specified in the constructor
 * @param output a span to a valid region of memory for the output string to be placed in, must be the same size as `input`
 * @return the number of output symbols generated, or -1u if an error ocurred (either `input` and `output` had differing sizes, or not all input characters were parsed)
 */
constexpr size_t FSM::parse(const std::span<const char> input, const std::span<char> output) const noexcept {
	if (input.size() != output.size()) {
		return -1u;
	}
	
	unsigned char state = 0;

	std::span<const char>::iterator inputIterator = input.begin();
	std::span<char>::iterator outputIterator = output.begin();
	
	while (true) {
		unsigned char symbol = (*equivalenceTable)[static_cast<unsigned char>(*inputIterator)];

		state = transitionTable[state*numSymbols + symbol];

		// check if no output symbol is defined
		// note that an input symbol is reparsed if transitioning to it generated an output
		if (state < outputStateCutoff) [[likely]] {
			inputIterator++;
		} else {
			*outputIterator = state - outputStateCutoff;

			// check if parsing has completed
			if (state == outputStateCutoff) [[unlikely]] {
				break;
			}

			state = 0;
			outputIterator++;
		}
	}

	if (inputIterator - input.begin() != input.size() - 1) [[unlikely]] {
		return -1u;
	}

	return (outputIterator - output.begin()) + 1;
}