#ifndef ASMINSTRUCTIONS_H_
#define ASMINSTRUCTIONS_H_
#include "DrillLib.h"

#define PC_REGISTER 16
typedef ArenaArrayList<ArenaArrayList<char>> File;
void io_print(File&, StrA);

struct asm_label {
	StrA label;
	U64 line_number;
};
struct exec_state {
	U32 registers[18];
	ArenaArrayList<asm_label> asm_labels{};
	MemoryArena* memory_arena;
	StrA* lines;
	U64 num_lines;
	U8* memory;
	U64 memory_size;
	U32 (*input_callback)();
	File* out_file;
	File* error_file;
};
exec_state* current_state;

void ASM_THROW_ERROR(StrA error) {
	if (current_state->error_file) {
		io_print(*current_state->error_file, strafmt(*(current_state->memory_arena), "ERROR on line %: %\n"a, current_state->registers[PC_REGISTER] + 1, error));
	}
}


asm_label find_label(StrA line) {
	for (U64 i = 0; i < current_state->asm_labels.size; i++) {
		if (line.starts_with(current_state->asm_labels.data[i].label)) {
			if (line.length == current_state->asm_labels.data[i].label.length || SerializeTools::is_whitespace(line.skip(current_state->asm_labels.data[i].label.length)[0])) {
				return current_state->asm_labels.data[i];
			}
		}
	}
	return {""a, current_state->registers[PC_REGISTER]};
}

void parse_labels(MemoryArena* mem, StrA* lines, U64 num_lines) {
	if (!current_state->asm_labels.allocator) {
		current_state->asm_labels = {mem};
	}

	for (U64 i = 0; i < num_lines; i++) {
		if (lines[i].ends_with(":"a)) {
			bool valid = true;
			for (U64 j = 0; j < lines[i].length; j++) {
				if (SerializeTools::is_whitespace(lines[i][j])) {
				ASM_THROW_ERROR("Label may not have whitespace"a);
					valid = false;
					break;
				}
			}
			if (valid) {
				lines[i].length--;
				current_state->asm_labels.push_back(asm_label{lines[i], i});
				lines[i].length = 0;
			}
		}
	}
}

bool is_register(StrA line) {
	return line.starts_with("r"a);
}

void skip_comma_after_value(StrA* line) {
	SerializeTools::skip_whitespace(line);
	if ((*line)[0] == ',') {
		(*line)++;
	}
	SerializeTools::skip_whitespace(line);
}

U8 parse_register(StrA* line) {
	if (line->starts_with("r"a)) {
		(*line)++;
		U64 out;
		if (SerializeTools::parse_u64(&out, line) == SerializeTools::INT_PARSE_SUCCESS) {
			if (out < 16) {
				skip_comma_after_value(line);
				return out;
			}
			else {
				ASM_THROW_ERROR("Invalid register number"a);
			}
		}
		else {
			ASM_THROW_ERROR("Failed to parse register"a);
		}
	}
	/// TODO: other register name
	return U8_MAX;
}

U32 parse_value(StrA* line) {
	SerializeTools::skip_whitespace(line);
	U64 value;
	if (!is_register(*line)) {
		if (SerializeTools::parse_u64((U64*)&value, line) != SerializeTools::INT_PARSE_SUCCESS) {
			ASM_THROW_ERROR("Falied to parse imm32 as 2nd argument to add"a);
		}
	}
	else {
		value = current_state->registers[parse_register(line)];
	}
	skip_comma_after_value(line);
	return value;
}

void add_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
	ASM_THROW_ERROR("first parameter of add should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = src1_value + src2_value;
}

U8 add_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of add should be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("destination parameter of add should be a register"a);
		ret = 0;
	}
	return ret;
}

void sub_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
	ASM_THROW_ERROR("first parameter of sub should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = src1_value - src2_value;
}

U8 sub_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of sub must be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("destination parameter of sub must be a register"a);
		ret = 0;
	}
	return ret;
}

void div_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
	ASM_THROW_ERROR("first parameter of div should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = (I32)src1_value / (I32)src2_value;
}

U8 div_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of div must be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("destination parameter of div must be a register"a);
		ret = 0;
	}
	return ret;
}

void mult_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
	ASM_THROW_ERROR("first parameter of mult should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = src1_value * src2_value;
}

U8 mult_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of mult must be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("destination parameter of mult must be a register"a);
		ret = 0;
	}
	return ret;
}

void rem_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
	ASM_THROW_ERROR("first parameter of rem should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = src1_value % src2_value;
}

U8 rem_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of rem must be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("destination parameter of rem must be a register"a);
		ret = 0;
	}
	return ret;
}

void and_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
	ASM_THROW_ERROR("first parameter of and should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = src1_value & src2_value;
}

U8 and_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of and should be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("destination parameter of and should be a register"a);
		ret = 0;
	}
	return ret;
}


void or_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
	ASM_THROW_ERROR("first parameter of or should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = src1_value | src2_value;
}

U8 or_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of or should be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("destination parameter of or should be a register"a);
		ret = 0;
	}
	return ret;
}

void xor_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
	ASM_THROW_ERROR("first parameter of xor should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = src1_value ^ src2_value;
}

U8 xor_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of xor should be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("destination parameter of xor should be a register"a);
		ret = 0;
	}
	return ret;
}

void not_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
	ASM_THROW_ERROR("first parameter of not should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = !src1_value;
}

U8 not_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of not should be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("destination parameter of not should be a register"a);
		ret = 0;
	}
	return ret;
}

void shl_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
	ASM_THROW_ERROR("first parameter of shl should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = src1_value << src2_value;
}

U8 shl_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of shl should be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("destination parameter of shl should be a register"a);
		ret = 0;
	}
	return ret;
}

void shr_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
	ASM_THROW_ERROR("first parameter of shr should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = src1_value >> src2_value;
}

U8 shr_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of shr should be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("destination parameter of shr should be a register"a);
		ret = 0;
	}
	return ret;
}

enum CMP_TYPE {
	CMPUNCOND,
	CMPEQ,
	CMPNE,
	CMPGT,
	CMPGE,
	CMPLT,
	CMPLE
};
enum CMP_TYPE get_comparison_type(StrA* line) {
	if (SerializeTools::is_whitespace((*line)[0])) {
		return CMPUNCOND;
	}
	enum CMP_TYPE type;
	if (line->starts_with("eq"a)) {
		type = CMPEQ;
	}
	else if (line->starts_with("ne"a)) {
		type = CMPNE;
	}
	else if (line->starts_with("gt"a)) {
		type = CMPGT;
	}
	else if (line->starts_with("ge"a)) {
		type = CMPGE;
	}
	else if (line->starts_with("lt"a)) {
		type = CMPLT;
	}
	else if (line->starts_with("le"a)) {
		type = CMPLE;
	}
	else {
		ASM_THROW_ERROR("Unknown comparison type"a);
	}
	*line = line->skip(2);
	return type;
}

U32 run_comparison(enum CMP_TYPE type, U32 src1, U32 src2) {
	switch (type) {
		case CMPUNCOND:
			return 1;
		case CMPEQ:
			return src1 == src2;
		case CMPNE:
			return src1 != src2;
		case CMPGT:
			return src1 > src2;
		case CMPGE:
			return src1 >= src2;
		case CMPLT:
			return src1 < src2;
		case CMPLE:
			return src1 <= src2;
	}
	return 0;
}

void cmp_inst(StrA line) {
	enum CMP_TYPE type = get_comparison_type(&line);
	if (type == CMPUNCOND) {
		type = CMPEQ;
	}
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of cmp should be a register"a);
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	U8 dst_index = parse_register(&line);
	current_state->registers[dst_index] = run_comparison(type, src1_value, src2_value);
}

U8 cmp_error_check(StrA line) {
	U8 ret = 1;
	enum CMP_TYPE type = get_comparison_type(&line);
	if (type == CMPUNCOND) {
		type = CMPEQ;
	}
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of cmp must be a register"a);
		ret = 0;
	}
	U32 src1_value = parse_value(&line);
	U32 src2_value = parse_value(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("dst parameter of cmp must be a register"a);
		ret = 0;
	}
	U8 dst_index = parse_register(&line);
	return ret;
}

void jump_inst(StrA line) {
	enum CMP_TYPE type = get_comparison_type(&line);
	SerializeTools::skip_whitespace(&line);
	U32 src1_value = 0;
	U32 src2_value = 0;
	if (type != CMPUNCOND) {
		if (!is_register(line)) {
			ASM_THROW_ERROR("first parameter of conditional jump should be a register"a);
		}
		src1_value = parse_value(&line);
		src2_value = parse_value(&line);
	}
	U32 result = run_comparison(type, src1_value, src2_value);
	if (result) {
		asm_label label = find_label(line);
		current_state->registers[PC_REGISTER] = label.line_number;
	}
}

U8 jump_error_check(StrA line) {
	U8 ret = 1;
	enum CMP_TYPE type = get_comparison_type(&line);
	SerializeTools::skip_whitespace(&line);
	U32 src1_value = 0;
	U32 src2_value = 0;
	if (type != CMPUNCOND) {
		if (!is_register(line)) {
			ASM_THROW_ERROR("first parameter of conditional jump must be a register"a);
			ret = 0;
		}
		src1_value = parse_value(&line);
		src2_value = parse_value(&line);
	}
	asm_label label = find_label(line);
	if (label.label == ""a) {
		ASM_THROW_ERROR(strafmt(*(current_state->memory_arena), "could not find label % in jump"a, line));
		ret = 0;
	}
	return ret;
}

void load_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("load must take a dst register"a);
	}
	U8 dst_register = parse_register(&line);
	SerializeTools::skip_whitespace(&line);
	if (line[0] != '[') {
		ASM_THROW_ERROR("load address must be surrounded by square brackets"a);
	}
	line++;
	U32 address = parse_value(&line);
	if (address < 0 || address > current_state->memory_size) {
		ASM_THROW_ERROR("load address out of bounds"a);
	}
	current_state->registers[dst_register] = current_state->memory[address];
}

U8 load_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("load must take a dst register"a);
		ret = 0;
	}
	U8 dst_register = parse_register(&line);
	SerializeTools::skip_whitespace(&line);
	if (line[0] != '[') {
		ASM_THROW_ERROR("load address must be surrounded by square brackets"a);
		ret = 0;
	}
	line++;
	U8 addr_is_register = 1;
	if (!is_register(line)) {
		addr_is_register = 0;
	}
	U32 address = parse_value(&line);
	if (!addr_is_register) {
		if (address < 0 || address > current_state->memory_size) {
			ASM_THROW_ERROR("load address out of bounds"a);
			ret = 0;
		}
	}
	return ret;
}

void store_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("store must take a dst register"a);
	}
	U8 dst_register = parse_register(&line);
	SerializeTools::skip_whitespace(&line);
	if (line[0] != '[') {
		ASM_THROW_ERROR("store address must be surrounded by square brackets"a);
	}
	line++;
	U32 address = parse_value(&line);
	if (address < 0 || address > current_state->memory_size) {
		ASM_THROW_ERROR("store address out of bounds"a);
	}
	current_state->memory[address] = current_state->registers[dst_register];
}

U8 store_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("store must take a dst register"a);
		ret = 0;
	}
	U8 dst_register = parse_register(&line);
	SerializeTools::skip_whitespace(&line);
	if (line[0] != '[') {
		ASM_THROW_ERROR("store address must be surrounded by square brackets"a);
		ret = 0;
	}
	line++;
	U8 addr_is_register = 1;
	if (!is_register(line)) {
		addr_is_register = 0;
	}
	U32 address = parse_value(&line);
	if (!addr_is_register) {
		if (address < 0 || address > current_state->memory_size) {
			ASM_THROW_ERROR("store address out of bounds"a);
			ret = 0;
		}
	}
	return ret;
}

void move_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	U8 src1 = parse_register(&line);
	SerializeTools::skip_whitespace(&line);
	U32 src2_value = parse_value(&line);
	current_state->registers[src1] = src2_value;
}

U8 move_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("first parameter of move must be a register"a);
		ret = 0;
	}
	return ret;
}

void in_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	U8 src1 = parse_register(&line);
	current_state->registers[src1] = current_state->input_callback();
}

U8 in_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("parameter of in must be a register"a);
		ret = 0;
	}
	return ret;
}

void out_inst(StrA line) {
	SerializeTools::skip_whitespace(&line);
	U8 src1 = parse_register(&line);
	if (current_state->out_file) {
		io_print(*current_state->out_file, strafmt(*(current_state->memory_arena), "%"a, current_state->registers[src1]));
	}
}

U8 out_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("parameter of out must be a register"a);
		ret = 0;
	}
	return ret;
}

void fly_inst(StrA line) {
	*((U32*)NULL) = 15;
}

U8 fly_error_check(StrA line) {
	U8 ret = 1;
	SerializeTools::skip_whitespace(&line);
	if (!is_register(line)) {
		ASM_THROW_ERROR("parameter of fly must be a register"a);
		ret = 0;
	}
	return ret;
}

#endif // ASMINSTRUCTIONS_H_
