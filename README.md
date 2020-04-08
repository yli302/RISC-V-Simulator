# RISC-V-Simulator
- Built RISC-V processor in C which supports a limited instruction set of the RISC-V ISA, restricted to the 32-bit
base with 64-bit extensions (RV32I and RV64I)
- Implemented the Synch, System, and Counters instructions. Support variable pipeline delays to a RISC-V pipeline,
instruction and data cache to pipelined RISC-V CPU, and branch prediction.
- Implement virtual memory translation. Include a TLB and the logic to perform virtual to physical translation to fill
the TLB on a TLB miss
