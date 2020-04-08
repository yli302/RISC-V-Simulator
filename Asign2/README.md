*************************************************************************
				Assignment_2: pipeline RISC-V sim						
*************************************************************************

Files: All in root dirextion.
    riscv_pipeline.c	riscv_sim_pipeline_framework.c
    riscv_sim_pipeline_framework.h  riscv_pipeline_registers.h
	   Makefile	designdoc.txt	README.md

Compile: type 'make' to compile.

This is a simulater of pipeline RISC-V. Instructions are implemented in fetch, decode, execute, memory, and writeback stages.
Forwards(MX, WM, WX) and stalls(for load instructions) are implemented, branch predictions have some bugs.
