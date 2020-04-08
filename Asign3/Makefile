all: riscvsim.out

riscv_sim_pipeline_framework.o: riscv_sim_pipeline_framework.c riscv_sim_pipeline_framework.h
	gcc -c -O3 -Wall -Wextra -DHAS_READLINE -c riscv_sim_pipeline_framework.c

riscv_pipeline.o: riscv_pipeline.c riscv_sim_pipeline_framework.h
	gcc -c -O3 -Wall -Wextra -DHAS_READLINE -c riscv_pipeline.c
	
riscvsim.out: riscv_sim_pipeline_framework.o riscv_pipeline.o
	gcc -o riscvsim riscv_sim_pipeline_framework.o riscv_pipeline.o -lreadline

