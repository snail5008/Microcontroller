/*
	An emulator for an turing-complete
	microcontroller
	
	Reuben Lilley, 2022

	IMPORTANT NOTES
	the 'out' and 'in' commands do not work properly in
	this emulator - in fact they do not work at all!

	out: they are just printed to stdout, this is
	just so that the output can be seen. It also
	outputs the entire register instead of just
	the lowest byte

	in: no point implementing it because there is no way to
	connect the emulator to external devices

	these commands would both work if it was implemented
	in real life
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SIZEOF_RAM 1048576

const bool QUIET = true;

#define PRINT_INSTRUCTION(x) if (!QUIET) { puts(x); }

const char* get_register_name(int id) {
	const char* str;
	switch (id) {
		case 0x00:
			str = "A";
			break;
		case 0x01:
			str = "B";
			break;
		case 0x02:
			str = "C";
			break;
		case 0x03:
			str = "D";
			break;
		case 0x04:
			str = "E";
			break;
		case 0x05:
			str = "ip";
			break;
		case 0x06:
			str = "sp";
			break;
		case 0x07:
			str = "bp";
			break;
		default:
			str = "UNKNOWN_REGISTER";
			break;
	}
	return str;
}

void print_all_registers(uint32_t** regs) {
	for (int i = 0; i < 8; i++) {
		if (strlen(get_register_name(i)) == 1)
			printf("%s:  0x%x (%u)\n", get_register_name(i), *regs[i], *regs[i]);
		else
			printf("%s: 0x%x (%u)\n", get_register_name(i), *regs[i], *regs[i]);
	} puts("\n");
}

int main(int argc, char** argv) {

	if (argc != 2) {
		puts("usage: ./emulator <binary>");
		exit(1);
	}

	uint8_t* program = malloc(1048576);
	if (!program) {
		puts("failed to allocate 1 MiB for program memory");
		exit(1);
	}
	FILE* f = fopen(argv[1], "rb");
	if (!f) {
		printf("failed to open file '%s'\n", argv[1]);
		exit(1);
	}
	fread(program, 1048576, 1, f);

	uint32_t a = 0;
	uint32_t b = 0;
	uint32_t c = 0;
	uint32_t d = 0;
	uint32_t e = 0;
	uint32_t ip = 0;
	uint32_t sp = 0;
	uint32_t bp = 0;
	uint32_t flags = 0;

	uint32_t* registers[] = {&a, &b, &c, &d, &e, &ip, &sp, &bp};
	uint32_t* ram = malloc(SIZEOF_RAM);
	
	uint32_t* r0;
	uint32_t* r1;

	uint8_t value[4];
	uint8_t value2[4];

	uint8_t output_ports = 0;

	bool should_halt = false;

	// fibbonaci
	// uint8_t program[] = {
	// 	// a = 0; b = 1; d = 0;
	// 	0x0e, 0x00, 0x00,				// xor a, a
	// 	0x0e, 0x01, 0x01,				// xor b, b
	// 	0x07, 0x01,						// inc b
	// 	0x0e, 0x03, 0x03,				// xor d, d

	// 	// c = a + b;
	// 	0x17, 0x02, 0x00,				// mov c, a
	// 	0x02, 0x02, 0x01,				// add c, b

	// 	// a = b;
	// 	0x17, 0x00, 0x01,				// mov a, b

	// 	// b = c;
	// 	0x17, 0x01, 0x02,				// mov b, c

	// 	// printf("%d", c);
	// 	0xfe, 0x02,						// out c

	// 	// if (j < 5) i++; goto 11;
	// 	0x1c, 0x03, 0x05, 0x00, 0x00, 0x00, // cmp d, 5
	// 	0x07, 0x03,						// inc d
	// 	0x1f, 0x0B, 0x00, 0x00, 0x00,	// jl 11 (0xb)

	// 	// exit(0);
	// 	0xff							// hlt
	// };

	// init ram to zeroes
	memset(ram, 0, SIZEOF_RAM);

	// main loop
	while (!should_halt) {
		switch (program[ip]) {
			// nop
			case 0x00:
				PRINT_INSTRUCTION("nop");
				ip++;
				break;
			
			// add r, v
			case 0x01:
				PRINT_INSTRUCTION("add r, v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				*r0 += *(uint32_t*)value;
				ip++;
				break;
			
			// add r, r
			case 0x02:
				PRINT_INSTRUCTION("add r, r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				*r0 = *r0 + *r1;
				ip++;
				break;

			// sub r, v
			case 0x03:
				PRINT_INSTRUCTION("sub r, v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				*r0 -= *(uint32_t*)value;
				ip++;
				break;
			
			// sub r, r
			case 0x04:
				PRINT_INSTRUCTION("sub r, r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				*r0 = *r0 - *r1;
				ip++;
				break;
			
			// mul r, v
			case 0x05:
				PRINT_INSTRUCTION("mul r, v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				*r0 *= *(uint32_t*)value;
				ip++;
				break;
			
			// mul r, r
			case 0x06:
				PRINT_INSTRUCTION("mul r, r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				*r0 = *r0 * *r1;
				ip++;
				break;
			
			// inc r
			case 0x07:
				PRINT_INSTRUCTION("inc r");
				ip++;
				r0 = registers[program[ip]];
				(*r0)++;
				ip++;
				break;
			
			// dec r
			case 0x08:
				PRINT_INSTRUCTION("dec r");
				ip++;
				r0 = registers[program[ip]];
				(*r0)--;
				ip++;
				break;
			
			// and r, v
			case 0x09:
				PRINT_INSTRUCTION("and r, v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				*r0 &= *(uint32_t*)value;
				ip++;
				break;
			
			// and r, r
			case 0x0A:
				PRINT_INSTRUCTION("and r, r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				*r0 = *r0 & *r1;
				ip++;
				break;

			// or r, v
			case 0x0B:
				PRINT_INSTRUCTION("or r, v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				*r0 |= *(uint32_t*)value;
				ip++;
				break;
			
			// or r, r
			case 0x0C:
				PRINT_INSTRUCTION("or r, r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				*r0 = *r0 | *r1;
				ip++;
				break;
			
			// xor r, v
			case 0x0D:
				PRINT_INSTRUCTION("xor r, v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				*r0 ^= *(uint32_t*)value;
				ip++;
				break;
			
			// xor r, r
			case 0x0E:
				PRINT_INSTRUCTION("xor r, r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				*r0 = *r0 ^ *r1;
				ip++;
				break;

			// nand r, v
			case 0x0F:
				PRINT_INSTRUCTION("nand r, v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				*r0 &= *(uint32_t*)value;
				*r0 = ~(*r0);
				ip++;
				break;
			
			// nand r, r
			case 0x10:
				PRINT_INSTRUCTION("nand r, r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				*r0 = *r0 & *r1;
				*r0 = ~(*r0);
				ip++;
				break;

			// nor r, v
			case 0x11:
				PRINT_INSTRUCTION("nor r, v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				*r0 |= *(uint32_t*)value;
				*r0 = ~(*r0);
				ip++;
				break;
			
			// nor r, r
			case 0x12:
				PRINT_INSTRUCTION("nor r, r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				*r0 = *r0 | *r1;
				*r0 = ~(*r0);
				ip++;
				break;
			
			// xnor r, v
			case 0x13:
				PRINT_INSTRUCTION("xnor r, v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				*r0 ^= *(uint32_t*)value;
				*r0 = ~(*r0);
				ip++;
				break;
			
			// xnor r, r
			case 0x14:
				PRINT_INSTRUCTION("xnor r, r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				*r0 = *r0 ^ *r1;
				*r0 = ~(*r0);
				ip++;
				break;
			
			// not r
			case 0x15:
				PRINT_INSTRUCTION("not r");
				ip++;
				r0 = registers[program[ip]];
				*r0 = ~(*r0);
				ip++;
				break;

			// mov r, v
			case 0x16:
				PRINT_INSTRUCTION("mov r, v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				*r0 = *(uint32_t*)value;
				if (r0 != registers[5])
					ip++;

				break;
			
			// mov r, r
			case 0x17:
				PRINT_INSTRUCTION("mov r, r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				*r0 = *r1;
				if (r0 != registers[5])
					ip++;
				break;
			
			// mov r, m
			case 0x18:
				PRINT_INSTRUCTION("mov r, m");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				if (*(uint32_t*)value > SIZEOF_RAM / 4 - 4) {
					printf("DEBUG: reading outside of SIZEOF_RAM / 4 - 4, "
					"putting 0 into register %s\n",
					get_register_name(*r0));
					*r0 = 0;
				}
				else
					*r0 = ram[*(uint32_t*)value];
				if (r0 != registers[5])
					ip++;
				break;
			
			// mov m, r
			case 0x19:
				PRINT_INSTRUCTION("mov m, r");
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				ip++;
				r0 = registers[program[ip]];
				if (*(uint32_t*)value > SIZEOF_RAM / 4 - 4) {
					printf("DEBUG: reading outside of SIZEOF_RAM / 4 - 4, "
					"doing nothing\n");
				}
				else
					ram[*(uint32_t*)value] = *r0;
				if (r0 != registers[5])
					ip++;
				break;
			
			// mov m, v
			case 0x1A:
				PRINT_INSTRUCTION("mov m, v");
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				ip++;
				value2[0] = program[ip];
				ip++;
				value2[1] = program[ip];
				ip++;
				value2[2] = program[ip];
				ip++;
				value2[3] = program[ip];
				if (*(uint32_t*)value > SIZEOF_RAM / 4 - 4) {
					printf("DEBUG: reading outside of SIZEOF_RAM / 4 - 4, "
					"doing nothing\n");
				}
				else
					ram[*(uint32_t*)value] = *(uint32_t*)value2;
				if (r0 != registers[5])
					ip++;

				break;
			
			
			// mov [r], v
			case 0xA0:
				PRINT_INSTRUCTION("mov [r], v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				if (*r0 > SIZEOF_RAM / 4 - 4) {
					printf("DEBUG: reading outside of SIZEOF_RAM / 4 - 4, "
					"doing nothing\n");
				}
				else
					ram[*r0] = *(uint32_t*)value;
				if (r0 != registers[5])
					ip++;

				break;

			// mov r, [r]
			case 0xA1:
				PRINT_INSTRUCTION("mov r, [r]");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				if (*r1 > SIZEOF_RAM / 4 - 4) {
					printf("DEBUG: reading outside of SIZEOF_RAM / 4 - 4, "
					"doing nothing\n");
				}
				else
					*r0 = ram[*r1];

				if (r0 != registers[5])
					ip++;

				break;
			
			// mov [r], r
			case 0xA2:
				PRINT_INSTRUCTION("mov [r], r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				if (*r1 > SIZEOF_RAM / 4 - 4) {
					printf("DEBUG: reading outside of SIZEOF_RAM / 4 - 4, "
					"doing nothing\n");
				}
				else
					ram[*r0] = *r1;

				if (r0 != registers[5])
					ip++;

				break;

			// CHANGE ELSE IFs TO IFs IF SOMETHING DOESN'T WORK WITH EITHER OF THE CMP INSTRUCTIONS

			// cmp r, r
			case 0x1B:
				PRINT_INSTRUCTION("cmp r, r");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				r1 = registers[program[ip]];
				flags = 0;
				if (*r0 == *r1) 
					flags |= (1 << 0);
				else if (*r0 < *r1)
					flags |= (1 << 1);
				else if (*r1 > *r0)
					flags |= (1 << 2);
				ip++;
				break;
			
			// cmp r, v
			case 0x1C:
				PRINT_INSTRUCTION("cmp r, v");
				ip++;
				r0 = registers[program[ip]];
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				flags = 0;
				if (*r0 == *(uint32_t*)value) 
					flags |= (1 << 0);
				else if (*r0 < *(uint32_t*)value)
					flags |= (1 << 1);
				else if (*r1 > *(uint32_t*)value)
					flags |= (1 << 2);
				ip++;
				break;

			// jmp v
			case 0x1D:
				PRINT_INSTRUCTION("jmp v");
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				ip = *(uint32_t*)value;
				break;
			
			// je/jz v
			case 0x1E:
				PRINT_INSTRUCTION("je/jz v");
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				if ((flags & (1 << 0)) == (1 << 0))
					ip = *(uint32_t*)value;
				break;

			// jl v
			case 0x1F:
				PRINT_INSTRUCTION("jl v");
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				if ((flags & (1 << 1)) == (1 << 1))
					ip = *(uint32_t*)value;
				break;
			
			// jg v
			case 0x20:
				PRINT_INSTRUCTION("jg v");
				ip++;
				value[0] = program[ip];
				ip++;
				value[1] = program[ip];
				ip++;
				value[2] = program[ip];
				ip++;
				value[3] = program[ip];
				if ((flags & (1 << 2)) == (1 << 2))
					ip = *(uint32_t*)value;
				break;
			

			// in r
			case 0xFD:
				PRINT_INSTRUCTION("in r (does nothing in this emulator)");
				ip += 2;
				break;

			// out r
			case 0xFE:
				PRINT_INSTRUCTION("out r");
				ip++;
				printf("OUTPUT (%s): 0x%x (%u)", get_register_name(program[ip]),
				*(uint32_t*)registers[program[ip]],
				*(uint32_t*)registers[program[ip]]
				);
				puts("");
				ip++;
				break;

			// hlt
			case 0xFF:
				PRINT_INSTRUCTION("hlt");
				ip++;
				should_halt = true;
				break;

			// unknown instruction
			default:
				printf("unknown instruction '%x'\n", program[ip]);
				ip++;
				break;
		}

		//if (!QUIET)
		//	print_all_registers(registers);
		if (should_halt)
			break;
	}
	free(ram);
	fclose(f);
	return 0;
}
