# KEY
_r_:
- 00: a: gp 32 bit
- 01: b: gp 32 bit
- 02: c: gp 32 bit
- 03: d: gp 32 bit
- 04: e: gp 32 bit
- 05: ip: 32 bit instruction pointer
- 06: sp: 32 bit stack pointer
- 07: bp: 32 bit gp, typically used as base pointer
- --: flags: 32 bit flag register, (only modified with `cmp`, not mathematical or bitwise instructions, also has no identifying number because not available to programmer via any instructions involving registers - only `cmp`)

_v_: an one byte immediate value passed as an argument

_m_: a memory location that is an offset into a `uint8_t* ram` multiplied by four, for example: `((uint32_t*)ram)[0]` = `ram[0]` and `((uint32_t*)ram)[1]` = `ram[4]`.


# flag register layout
| greater | less | equal |

# INSTRUCTION SET

## mathematical operations
- 00 one byte NOP (nop)
	- do no operation for an entire clock cycle
- 01 _r_ _v_ - `add r, v`
	- add v to r, store result in r
- 02 _r0_ _r1_ - `add r0, r1`
	- add r0 to r1, store result in r0
- 03 _r_ _v_ - `sub r, v`
	- subtract v from r, store result in r
- 04 _r0_ _r1_ - `sub r0, r1`
	- subtract r1 from r0, store result in r0
- 05 _r_ _v_ - `mul r, v`
	- multiply r0 and r1, store result in r0
- 06 _r0_ _r1_ - `mul r0, r1`
	- multiply r0 and r1, store result in r0
- 07 _r_ - `inc r`
	- increment r (r++)
- 08 _r_ - `dec r`
	- decrement r (r--)

## bitwise operations
- 09 _r_ _v_ - `and r, v`
	- bitwise and: r, v - result in r
- 0A _r0_ _r1_ - `and r0, r1`
	- bitwise and: r0, r1 - result in r0
- 0B _r_ _v_ - `or r, v`
	- bitwise or: r, v - result in r
- 0C _r0_ _r1_ - `or r0, r1`
	- bitwise or: r0, r1 - result in r0
- 0D _r_ _v_ - `xor r, v`
	- bitwise xor: r, v - result in r
- 0E _r0_ _r1_ - `xor r0, r1`
	- bitwise xor: r0, r1 - result in r0
- 0F _r_ _v_ - `nand r, v`
	- bitwise nand: r, v - result in r
- 10 _r0_ _r1_ - `nand r0, r1`
	- bitwise nand: r0, r1 - result in r0
- 11 _r_ _v_ - `nor r, v`
	- bitwise nor: r, v - result in r
- 12 _r0_ _r1_ - `nor r0, r1`
	- bitwise nor: r0, r1 - result in r0
- 13 _r_ _v_ - `xnor r, v`
	- bitwise xnor: r, v - result in r
- 14 _r0_ _r1_ - `xnor r0, r1`
	- bitwise xnor: r0, r1 - result in r0
- 15 _r_ - `not r`
	- bitwise not: !r -> r

## moving values
### for any of the following instructions, if the register value is equal to `ip`, then `ip` will NOT be incremented at the end of the instruction, unlike ALL other instructions - modifying `ip` directly, however, is still not advised as the jmp instruction takes less bytes and is clearer
- 16 _r_ _v_ - `mov r, v`
	- move v into r
- 17 _r0_ _r1_ - `mov r0, r1`
	- move r into r
- 18 _r_ _m_ - `mov r, [m]`
	- move the contents of m into r
- 19 _m_ _r_ - `mov [m], r`
	- move r into address m
- 1A _m_ _v_ - `mov [m], v`
	- move an immediate value into m

## control flow/branching
- 1B _r0_ _r1_ - `cmp r0, r1`
	- compare r0 and r1, store flags in flags registor
- 1C _r_ _v_ - `cmp r, v`
	- compare r and v, store flags in flags registor
- 1D _v_ - `jmp v`
	- jump to memory location 'v', aka move instruction pointer to big endian value
- 1E _v_ - `je/jz v`
	- jump to v if last compare was equal
- 1F _v_ - `jl v`
	- jump to v if last compare was less
- 20 _v_ - `jg v`
	- jump to v if last compare was greater

## the stack
### not actually implemented but can be simulated with the following commands. Also, an assembler can implement some sort of macros to replace, for example `push a` with `mov [sp], a inc`
- `push r` = `mov [sp], r` and `inc sp`
- `pop r` = `dec sp` and `mov r, [sp]`
- `call v` = `mov [sp], v` and `inc sp`
- `ret` = `dec sp`, `mov r, [sp]` and `mov ip, r`


## instructions that should've been added earlier but were not
- A0 _r_ _v_ - `mov [r], v`
	- move v into address r
- A1 _r0_ _r1_ - `mov r0, [r1]`
	- move value at r1 into r0
- A2 _r0_ _r1_ - `mov [r0], r1`
	- move r1 into address r0


## microprocesser instructions
- FD _r_ - `in r`
	- puts the values in the input ports into the lowest 8 bits of r
- FE _r_ - `out r`
	- puts the lowest 8 bits of r into the output ports
- FF - `hlt`
	- halt the microprocessor
