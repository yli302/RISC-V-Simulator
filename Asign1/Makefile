all: riscvsim.out 
	
riscv_sim_framework.o: riscv_sim_framework.c riscv_sim_framework.h
	gcc -c -Wall -DHAS_READLINE -c riscv_sim_framework.c
 
riscv_execute_one.o: riscv_execute_one.c riscv_sim_framework.h
	gcc -c -Wall -DHAS_READLINE -c riscv_execute_one.c

riscvsim.out: riscv_sim_framework.o riscv_execute_one.o
	gcc -o riscvsim riscv_sim_framework.o riscv_execute_one.o -lreadline

