# FSM internal documentation

## parse()
The parsing function works roughly as follows:
1. an internal state variable is initialized to zero
2. for each input character:
	1. an input symbol is computed by looking up the character in an 'equivalence table'
	2. the current state and input symbol are used to look up the next state in a two-dimensional 'transition table'
	3. if the new state is greater than or equal to the 'output state cutoff':
		1. an output symbol is generated based on the value of the new state
		2. parsing is terminated if the new state equals the output state cutoff
		3. the state is reset to zero, and the input character is reparsed

## Lookup Tables
Two configurable lookup tables are used for parsing; an 'equivalence table', and 'transition table'.

### Equivalence Table
The equivalence table stores two hundred fifty-six unsigned character entries (one for each possible input character).
Characters are interpreted as unsigned and then passed through the table to compute a symbol used for further processing.
The intention of using 'equivalent' symbols instead of the characters themselves is to reduce the size of the transition table when the FSM contains many possible states.

### Transition Table
The transition table is a two-dimensional array that maps state/input pairs to new states.
Unlike most two-dimensional arrays, which are stored as an array of pointers to arrays, the transition table has consistently sized rows, which it simply stores one after the other without any further indirection.
Indices into the table are computed using the number of possible input symbols specified in the constructor.

Each state has a corresponding row in the table (based on the numerical value of its ID) with a list of target state IDs for each input symbol.
Specifically, the expression used to compute the index of a transition is `currentState*numInputSymbols + inputSymbol`.
No padding or additional formatting is required; a properly formatted transition table should just look like a large array of unsigned chars.

## FSM Design
This section describes the suggested design for configured Finite State Machines.

FSMs should contain two types of states; general, and output. Both are identified by a numerical (`unsigned char`) identifier.
General states are used to create the actual logic of the FSM, whereas output states are simply goals to be reached; generating an output symbol and resetting the state when hit.
IDs should be organized based on type; general states should be less than the output state cutoff, and outputs greater than or equal.
Additionally, two IDs have special meaning; `0`, and the ID directly equal to the output state cutoff.
`0` should represent some sort of 'neutral'/whitespace state. The FSM is initialized to `0` during parsing, and is reset to `0` before reparsing a character due to an output being generated.
The ID equal to the output state cutoff should be a 'null' state, only reached at the end of the input string. Parsing terminates when this state is reached.
When an output is generated, the specific value written is equal to the output state cutoff subtracted from its ID (i.e. the null state becomes `0` in the output).

All other IDs can be freely used for whatever the FSM needs them for. General IDs should additionally be packed tightly together, without gaps, as every such gap would require a row in the transition table.
On the other hand, there can be an arbitrary number of unused IDs in-between the final general state and the output state cutoff, causing no loss in performance or increase in memory usage.
Output states can also be arbitrarily spaced without ill effects. That being said, you may want to tightly pack outputs if you are feeding the FSM that generates them into another FSM, as those outputs will be become inputs.

The design of the equivalence table and number of general states have the largest effect on the memory usage of the FSM; ideally, you want the equivalence table to map to the smallest number of input symbols possible, and to have as few general states as possible.
Specifically, the memory usage (in bytes) of the transition table is equal to the number of general states multiplied by the number of possible input symbols.
Note that only general states need to be accounted for in the transition table; the number of output states has no effect on performance or memory usage.

Note that there are a few cases where the FSM can enter an infinite loop if designed improperly:
- (assuming the input is null-terminated) there is no transition from state zero to the null state, and/or a state has a transition to a non-output state when seeing a null character
- the input string does not contain a character guaranteed to transition to the null state
- there exists a transition to an output state where the act of reparsing would cause the same transition to occur (this is most likely to happen if the FSM is configured to immediately transition to an output upon certain characters)

The last case may unfortunately lead to some situations where parsing is slower than necessary due to the FSM having to 'wait' for another character to be able to output, even when the character seen would have no effect on the decision. This is a known limitation of the current implementation, and a solution can be easily implemented in the future should performance loss be shown to be significant in a realistic test.

## Example
Let's say we want to parse out all instances of the strings 'abc' and 'abd' from an input.