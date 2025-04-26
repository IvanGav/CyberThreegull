//// Do Not Include in Final Project!!!!
#include "DrillLib.h"
#include "SerializeTools.h"
#include "AsmInstructions.h"

struct instruction {
	StrA inst;
	void(*inst_func)(StrA);
	U8(*error_check_func)(StrA);
};


instruction insts[] = {
	{"add"a, add_inst, add_error_check},
	{"sub"a, sub_inst, sub_error_check},
	{"div"a, div_inst, div_error_check},
	{"mult"a, mult_inst, mult_error_check},
	{"rem"a, rem_inst, rem_error_check},
	{"and"a, and_inst, and_error_check},
	{"or"a,  or_inst,  or_error_check},
	{"xor"a, xor_inst, xor_error_check},
	{"not"a, not_inst, not_error_check},
	{"shl"a, shl_inst, shl_error_check},
	{"shr"a, shr_inst, shr_error_check},
	{"cmp"a, cmp_inst, cmp_error_check},
	{"jump"a, jump_inst, jump_error_check},
	{"load"a, load_inst, load_error_check},
	{"store"a, store_inst, store_error_check},
	{"move"a, move_inst, move_error_check},
	{"in"a, in_inst, in_error_check},
	{"out"a, out_inst, out_error_check},
	{"fly"a, fly_inst, fly_error_check},
};
const U64 num_instructions = sizeof(insts) / sizeof(insts[0]);

void output(U32 val) {
	printf("%\n"a, val);
}


void parse_line(StrA);
B8 init_file_for_interpretation(exec_state*, StrA*, U64);
B8 interpret(exec_state*, U64);
U8 check_errors(StrA*, U64);


void run_program(exec_state* state, StrA* prog, U64 num_lines) {
	/*StrA prog = "move r1, 10\n"
		"move r2, 11\n"
		"add  r1, r2, r3\n"
		"shl  r3, 2,  r3\n"
		"jump test2\n"
		"test:\n"
		"cmp r3, r1, r2\n"
		"jump test3\n"
		"test2:\n"
		"jumpeq r3, 21, test\n"
		"test3:\n"
		"out r3"a;
	*/

	for (int i = 0; i < 18; i++) {
		state->registers[i] = 0;
	}

	if (init_file_for_interpretation(state, prog, num_lines)) {
		while (interpret(state, 10000)) {
			ASM_THROW_ERROR("Ran for too long"a);
			return;
		}
	}
}

// Return 1 if no errors were found
B8 init_file_for_interpretation(exec_state* state, StrA* file_contents, U64 num_lines) {
	current_state = state;

	state->lines = file_contents;
	state->num_lines = num_lines;
	parse_labels(state->memory_arena, file_contents, num_lines);
	B8 error_free = check_errors(file_contents, num_lines);
	current_state = NULL;
	return error_free;
}

// Return 1 if still has uninterpreted lines
B8 interpret(exec_state* state, U64 num_lines_to_exec) {
	U64 num_lines_executed = 0;
	current_state = state;
	for (;state->registers[PC_REGISTER] < state->num_lines && num_lines_executed < num_lines_to_exec; state->registers[PC_REGISTER]++, num_lines_executed++) {
		parse_line(state->lines[state->registers[PC_REGISTER]]);
	}
	current_state = NULL;
	return state->registers[PC_REGISTER] < state->num_lines;
}


U8 check_errors(StrA* lines, U64 num_lines) {
	U8 error_free = 1;
	for (current_state->registers[PC_REGISTER] = 0; current_state->registers[PC_REGISTER] < num_lines; current_state->registers[PC_REGISTER]++) {
		StrA& line = lines[current_state->registers[PC_REGISTER]];
		SerializeTools::skip_whitespace(&line);
		for (U64 i = 0; i < num_instructions; i++) {
			if (line.starts_with(insts[i].inst)) {
				error_free = (error_free && insts[i].error_check_func(line.skip(insts[i].inst.length)));
				break;
			}
		}
	}
	current_state->registers[PC_REGISTER] = 0;
	return error_free;
}

void parse_line(StrA line) {
	SerializeTools::skip_whitespace(&line);
	for (U64 i = 0; i < num_instructions; i++) {
		if (line.starts_with(insts[i].inst)) {
			line = line.skip(insts[i].inst.length);
			printf("%: %\n"a, current_state->registers[PC_REGISTER], insts[i].inst);
			insts[i].inst_func(line);
		}
	}
}
