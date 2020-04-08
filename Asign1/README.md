**************************************************************************
				Assignment_1: single-cycle RISC-V processor	             
**************************************************************************

Files: riscv_execute_one.c	riscv_sim_framework.c	riscv_sim_framework.h
	   Makefile	designdoc.txt	README.md

Compile: type 'make' to compile.

The simulator itself takes the following commands:
	load /x offset filename
	dump /x offset num_bytes [filename]
	readreg reg_num
	writereg reg_num value
	run program_counter num_steps
	exit

Bugs: mulh, mulhsu, and mulhu have some errors.
