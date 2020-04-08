#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include "riscv_sim_pipeline_framework.h"
void stage_fetch (struct stage_reg_d *new_d_reg) {//uint64_t pc, uint64_t * new_pc
    uint64_t fetch_result; // address of instruction
	uint64_t size_in_bytes = 4; // read 32 bits from pc
	memory_read (pc, &fetch_result, size_in_bytes);

	*new_pc = pc + 0x4; // move pc to a new address.
	
    return fetch_result;
}

void stage_decode (struct stage_reg_x *new_x_reg) {//uint64_t fetch_result
    uint64_t opcode = fetch_result & 0x7F;
    uint64_t rd;
    uint64_t funct3;
    uint64_t rs1;
    uint64_t rs2;
    uint64_t funct7;
    uint64_t imm11_0;
    uint64_t imm4_0;
    uint64_t imm11_5;
    uint64_t imm4_1_11;
    uint64_t imm12_10_5;
    uint64_t imm31_12;
    uint64_t imm20_10_1_11_19_12;
    uint64_t value_rs1; // it is a unsigned 32-bit value
    uint64_t value_rs2; // it is a unsigned 32-bit value
    
	// allocate memory to array.
	uint64_t *array;
	array = (uint64_t*)malloc(sizeof(uint64_t) * 20);
    // initial array.
	for (uint64_t i = 0; i < 20; i++) {
        *(array+i) = 0;
    }
	
	// type R
    if(opcode == 0x33 || opcode == 0x3b){
        rd = fetch_result & 0xf80;
        rd = rd >> 7;
		/* remember change pow, pow is douyble, but we need uint64_t, 
		 *pow need to be used in all sectiopn to logic right shift
		*/   
        funct3 = fetch_result & 0x7000; //bitmasking
        funct3 = funct3 >> 12; // logic right shift
        
        rs1 = fetch_result & 0xf8000;
        rs1 = rs1 >> 15;
        
        rs2 = fetch_result & 0x1f00000;
        rs2 = rs2 >> 20;
        
        funct7 = fetch_result & 0xfe000000;
        funct7 = funct7 >> 25;
    }
	// type I
    else if(opcode == 0x03 || opcode == 0x13 || opcode == 0x1b || opcode == 0x67 || opcode ==0x73){
        rd = fetch_result & 0xf80;
        rd = rd >> 7;
        
        funct3 = fetch_result & 0x7000;
        funct3 = funct3 >> 12;
        
        rs1 = fetch_result & 0xf8000;
        rs1 = rs1 >> 15;
        
        imm11_0 = fetch_result & 0xfff00000;
        imm11_0 = imm11_0 >> 20;
    }
	// type S
    else if(opcode == 0x23){
        imm4_0 = fetch_result & 0xf80;
		imm4_0 = imm4_0 >> 7; 
		
        funct3 = fetch_result & 0x7000;
		funct3 = funct3 >> 12; 
		
        rs1 = fetch_result & 0xf8000;
		rs1 = rs1 >> 15; 
		
        rs2 = fetch_result & 0x1f00000;
		rs2 = rs2 >> 20; 
		
        imm11_5 = fetch_result & 0xfe000000;
		imm11_5 = imm11_5 >> 25;
    }
	// type SB
    else if(opcode == 0x63){
        imm4_1_11 = fetch_result & 0xf80;
		imm4_1_11 = imm4_1_11 >> 7;
		
        funct3 = fetch_result & 0x7000;
		funct3 = funct3 >> 12; 
		
        rs1 = fetch_result & 0xf8000;
		rs1 = rs1 >> 15; 
		
        rs2 = fetch_result & 0x1f00000;
		rs2 = rs2 >> 20; 
		
        imm12_10_5 = fetch_result & 0xfe000000;
		imm12_10_5 = imm12_10_5 >> 25;
    }
	// type U
    else if(opcode == 0x17 || opcode == 0x37){
        rd = fetch_result & 0xf80;
        rd = rd >> 7;
		
        imm31_12 = fetch_result & 0xfffff000;
		imm31_12 = imm31_12 >> 12;
    }
	// type UJ
    else if(opcode == 0x6f){
        rd = fetch_result & 0xf80;
        rd = rd >> 7;
		
        imm20_10_1_11_19_12 = fetch_result & 0xfffff000;
		imm20_10_1_11_19_12 = imm20_10_1_11_19_12 >> 12;
    }
    //register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
    //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
	// push corresponding uint64_t to decoded array.
	if(opcode == 0x03 && funct3 == 0x00){
        //For lb
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lb = 1;
        *array = lb;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x03 && funct3 == 0x01){
        //For lh
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lh = 2;
        *array = lh;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x03 && funct3 == 0x02){
        //For lw
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lw = 3;
        *array = lw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x03 && funct3 == 0x03){
        //For ld
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t ld = 4;
        *array = ld;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x03 && funct3 == 0x04){
        //For lbu
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lbu = 5;
        *array = lbu;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x03 && funct3 == 0x05){
        //For lhu
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lhu = 6;
        *array = lhu;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x03 && funct3 == 0x06){
        //For lwu
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lwu = 7;
        *array = lwu;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x13 && funct3 == 0x00){
        //For addi
        register_read (rs1, rs1, &value_rs1, &value_rs1);
		uint64_t addi = 8;
		*array = addi;
		*(array+1) = opcode;
		*(array+2) = rd;
		*(array+3) = funct3;
		*(array+4) = value_rs1;
		*(array+7) = imm11_0;
    }
    else if(opcode == 0x13 && funct3 == 0x01){
        //For slli
        register_read (rs1, rs1, &value_rs1, &value_rs1);
		uint64_t imm11_5;
		imm11_5 = imm11_0 & 0xfe0;
		imm11_5 = imm11_0 >> 5;
		if(imm11_5 == 0x00){
			uint64_t slli = 9;
			uint64_t imm4_0 = imm11_0 & 0x01f;
			*array = slli;
			*(array+1) = opcode;
			*(array+2) = rd;
			*(array+3) = funct3;
			*(array+4) = value_rs1;
			*(array+7) = imm4_0;
		}
    }
    else if(opcode == 0x13 && funct3 == 0x02){
        //For slti
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t slti = 10;
        *array = slti;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x13 && funct3 == 0x03){
        //For sltiu
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t sltiu = 11;
        *array = sltiu;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x13 && funct3 == 0x04){
        //For xori
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t xori = 12;
        *array = xori;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x13 && funct3 == 0x05){
        //For srli
        register_read (rs1, rs1, &value_rs1, &value_rs1);
		uint64_t imm11_5;
		imm11_5 = imm11_0 & 0xfe0;
		imm11_5 = imm11_0 >> 5;
		if(imm11_5 == 0x00) {
			uint64_t srli = 13;
			uint64_t imm4_0 = imm11_0 & 0x01f;
			*array = srli;
			*(array+1) = opcode;
			*(array+2) = rd;
			*(array+3) = funct3;
			*(array+4) = value_rs1;
			*(array+7) = imm4_0;
		}
		//For srai
		else if(imm11_5 == 0x20) {
			uint64_t srai = 14;
			uint64_t imm4_0 = imm11_0 & 0x01f;
			*array = srai;
			*(array+1) = opcode;
			*(array+2) = rd;
			*(array+3) = funct3;
			*(array+4) = value_rs1;
			*(array+7) = imm4_0;
		}
    }
    else if(opcode == 0x13 && funct3 == 0x06){
        //For ori
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t ori = 15;
        *array = ori;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x13 && funct3 == 0x07){
        //For andi
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t andi = 16;
        *array = andi;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x17){
        //For auipc
        uint64_t auipc = 17;
        *array = auipc;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+12) = imm31_12;
    }
    else if(opcode == 0x1b && funct3 == 0x00){
        //For addiw
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t addiw = 18;
        *array = addiw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x1b && funct3 == 0x01){
        //For slliw
        register_read (rs1, rs1, &value_rs1, &value_rs1);
		uint64_t imm11_5;
		imm11_5 = imm11_0 & 0xfe0;
		imm11_5 = imm11_0 >> 5;
		if(imm11_5 == 0x00){
			uint64_t slliw = 19;
			uint64_t imm4_0 = imm11_0 & 0x01f;
			*array = slliw;
			*(array+1) = opcode;
			*(array+2) = rd;
			*(array+3) = funct3;
			*(array+4) = value_rs1;
			*(array+7) = imm4_0;
		}
    }
    else if(opcode == 0x1b && funct3 == 0x05){
        //For srliw
        register_read (rs1, rs1, &value_rs1, &value_rs1);
		uint64_t imm11_5;
		imm11_5 = imm11_0 & 0xfe0;
		imm11_5 = imm11_0 >> 5;
		if(imm11_5 == 0x00){
			uint64_t srliw = 20;
			uint64_t imm4_0 = imm11_0 & 0x01f;
			*array = srliw;
			*(array+1) = opcode;
			*(array+2) = rd;
			*(array+3) = funct3;
			*(array+4) = value_rs1;
			*(array+7) = imm4_0;
		}
		//For sraiw
		else if(imm11_5 == 0x20){
			uint64_t sraiw = 21;
			uint64_t imm4_0 = imm11_0 & 0x01f;
			*array = sraiw;
			*(array+1) = opcode;
			*(array+2) = rd;
			*(array+3) = funct3;
			*(array+4) = value_rs1;
			*(array+7) = imm4_0;
		}
    }
    else if(opcode == 0x23 && funct3 == 0x00){
        //For sb
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sb = 22;
        *array = sb;
        *(array+1) = opcode;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+8) = imm4_0;
        *(array+9) = imm11_5;
    }
    else if(opcode == 0x23 && funct3 == 0x01){
        //For sh
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sh = 23;
        *array = sh;
        *(array+1) = opcode;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+8) = imm4_0;
        *(array+9) = imm11_5;
    }
    else if(opcode == 0x23 && funct3 == 0x02){
        //For sw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sw = 24;
        *array = sw;
        *(array+1) = opcode;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+8) = imm4_0;
        *(array+9) = imm11_5;
    }
    else if(opcode == 0x23 && funct3 == 0x03){
        //For sd
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sd = 25;
        *array = sd;
        *(array+1) = opcode;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+8) = imm4_0;
        *(array+9) = imm11_5;
    }
    else if(opcode == 0x33 && funct3 == 0x00 && funct7 == 0x00){
        //For add
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t add = 26;
        *array = add;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x00 && funct7 == 0x20){
        //For sub
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sub = 27;
        *array = sub;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x01 && funct7 == 0x00){
        //For sll
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sll = 28;
        *array = sll;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x02 && funct7 == 0x00){
        //For slt
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t slt = 29;
        *array = slt;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x03 && funct7 == 0x00){
        //For sltu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sltu = 30;
        *array = sltu;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x04 && funct7 == 0x00){
        //For xor
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t xor = 31;
        *array = xor;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x05 && funct7 == 0x00){
        //For srl
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t srl = 32;
        *array = srl;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x05 && funct7 == 0x20){
        //For sra
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sra = 33;
        *array = sra;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x06 && funct7 == 0x00){
        //For or
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t or = 34;
        *array = or;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x07 && funct7 == 0x00){
        //For and
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t and = 35;
        *array = and;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x37){
        //For lui
        uint64_t lui = 36;
        *array = lui;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+12) = imm31_12;
    }
    else if(opcode == 0x3b && funct3 == 0x00 && funct7 == 0x00){
        //For addw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t addw = 37;
        *array = addw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x3b && funct3 == 0x00 && funct7 == 0x20){
        //For subw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t subw = 38;
        *array = subw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x3b && funct3 == 0x01 && funct7 == 0x00){
        //For sllw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sllw = 39;
        *array = sllw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = rs1;
        *(array+5) = rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x3b && funct3 == 0x05 && funct7 == 0x00){
        //For srlw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t srlw = 40;
        *array = srlw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x3b && funct3 == 0x05 && funct7 == 0x20){
        //For sraw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sraw = 41;
        *array = sraw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x63 && funct3 == 0x00){
        //For beq
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t beq = 42;
        *array = beq;
        *(array+1) = opcode;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+10) = imm4_1_11;
        *(array+11) = imm12_10_5;
    }
    else if(opcode == 0x63 && funct3 == 0x01){
        //For bne
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t bne = 43;
        *array = bne;
        *(array+1) = opcode;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+10) = imm4_1_11;
        *(array+11) = imm12_10_5;
    }
    else if(opcode == 0x63 && funct3 == 0x04){
        //For blt
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t blt = 44;
        *array = blt;
        *(array+1) = opcode;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+10) = imm4_1_11;
        *(array+11) = imm12_10_5;
    }
    else if(opcode == 0x63 && funct3 == 0x05){
        //For bge
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t bge = 45;
        *array = bge;
        *(array+1) = opcode;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+10) = imm4_1_11;
        *(array+11) = imm12_10_5;
    }
    else if(opcode == 0x63 && funct3 == 0x06){
        //For bltu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t bltu = 46;
        *array = bltu;
        *(array+1) = opcode;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+10) = imm4_1_11;
        *(array+11) = imm12_10_5;
    }
    else if(opcode == 0x63 && funct3 == 0x07){
        //For bgeu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t bgeu = 47;
        *array = bgeu;
        *(array+1) = opcode;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+10) = imm4_1_11;
        *(array+11) = imm12_10_5;
    }
    else if(opcode == 0x67 && funct3 == 0x00){
        //For jalr
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t jalr = 48;
        *array = jalr;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+7) = imm11_0;
    }
    else if(opcode == 0x6F){
        //For jal
        uint64_t jal = 49;
        *array = jal;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+13) = imm20_10_1_11_19_12;
    }
    else if(opcode == 0x33 && funct3 == 0x00 && funct7 == 0x01) {
        //For mul
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        int mul = 50;
        *array = mul;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x01 && funct7 == 0x01) {
        //For mulh
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        int mulh = 51;
        *array = mulh;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x02 && funct7 == 0x01) {
        //For mulhsu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        int mulhsu = 52;
        *array = mulhsu;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x33 && funct3 == 0x03 && funct7 == 0x01) {
        //For mulhu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        int mulhu = 53;
        *array = mulhu;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
    else if(opcode == 0x3b && funct3 == 0x00 && funct7 == 0x01) {
        //For mulw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        int mulw = 54;
        *array = mulw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
	 else if(opcode == 0x33 && funct3 == 0x04 && funct7 == 0x01){
        //For div
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t div = 55;
        *array = div;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
	else if(opcode == 0x33 && funct3 == 0x05 && funct7 == 0x01){
        //For divu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t divu = 56;
        *array = divu;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
	else if(opcode == 0x33 && funct3 == 0x06 && funct7 == 0x01){
        //For rem
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t rem = 57;
        *array = rem;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
	else if(opcode == 0x33 && funct3 == 0x07 && funct7 == 0x01){
        //For remu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t remu = 58;
        *array = remu;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
	else if(opcode == 0x3b && funct3 == 0x04 && funct7 == 0x01){
        //For divw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t divw = 59;
        *array = divw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
	else if(opcode == 0x3b && funct3 == 0x05 && funct7 == 0x01){
        //For divuw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t divuw = 60;
        *array = divuw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
	else if(opcode == 0x3b && funct3 == 0x06 && funct7 == 0x01){
        //For remw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t remw = 61;
        *array = remw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
	else if(opcode == 0x3b && funct3 == 0x07 && funct7 == 0x01){
        //For remuw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t remuw = 62;
        *array = remuw;
        *(array+1) = opcode;
        *(array+2) = rd;
        *(array+3) = funct3;
        *(array+4) = value_rs1;
        *(array+5) = value_rs2;
        *(array+6) = funct7;
    }
	return array;
}

void stage_execute(struct stage_reg_m *new_m_reg) {//uint64_t *array, uint64_t pc, uint64_t * new_pc
	uint64_t value_rs1; // it is a unsigned 32-bit value
	uint64_t value_rs2; // it is a unsigned 32-bit value
	int64_t signed_rs1; // it is a signed 32-bit value
	int64_t signed_rs2; // it is a signed 32-bit value
	int64_t signed_rd; // it is a signed 32-bit value
	uint64_t value_rd; // it is a unsigned 64-bit value
	
	if (*(array) == 1) {  
		// lb 
		uint64_t register_rd = *(array + 2);
		uint64_t register_rs1 = *(array + 4);
		uint64_t imm11_0 = *(array + 7);
		
		register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		
		uint64_t address =  value_rs1 + imm11_0;
		// read 1 byte from [(rs1) + (imm)], and store it to address of rd.
		memory_read(address, &value_rd, 1);
		// extend rd to 64-bit value. (negative # is different from positive #)
		if (value_rd >= 0x80) {
			value_rd = value_rd | 0xffffffffffffff80;
		} else {
			value_rd = value_rd & 0x00000000000000ff;
		}
		
	} else if (*(array) == 2) {  
		// lh
		uint64_t register_rd = *(array + 2);
		uint64_t register_rs1 = *(array + 4);
		uint64_t imm11_0 = *(array + 7);
		
		register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		
		uint64_t address =  value_rs1 + imm11_0;
		// read 2 byte from [(rs1) + (imm)], and store it to address of rd.
		memory_read(address, &value_rd, 2);
		// extend rd to 64-bit value. (negative # is different from positive #)
		if (value_rd >= 0x8000) {
			value_rd = value_rd | 0xffffffffffff8000;
		} else {
			value_rd = value_rd & 0x000000000000ffff;
		}
		
    } else if (*(array) == 3) { 
		// lw
        uint64_t register_rd = *(array + 2);
		uint64_t register_rs1 = *(array + 4);
		uint64_t imm11_0 = *(array + 7);
		
		register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		
		uint64_t address =  value_rs1 + imm11_0;
		// read 4 byte from [(rs1) + (imm)], and store it to address of rd.
		memory_read(address, &value_rd, 4);
		// extend rd to 64-bit value. (negative # is different from positive #)
		if (value_rd >= 0x80000000) {
			value_rd = value_rd | 0xffffffff80000000;
		} else {
			value_rd = value_rd & 0x00000000ffffffff;
		}
		
    } else if (*(array) == 4) { 
		// ld
        uint64_t register_rd = *(array + 2);
		uint64_t register_rs1 = *(array + 4);
		uint64_t imm11_0 = *(array + 7);
		
		register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		
		uint64_t address =  value_rs1 + imm11_0;
		// read 8 byte from [(rs1) + (imm)], and store it to address of rd.
		memory_read(address, &value_rd, 8);
		
    } else if (*(array) == 5) { 
		// lbu
        uint64_t register_rd = *(array + 2);
		uint64_t register_rs1 = *(array + 4);
		uint64_t imm11_0 = *(array + 7);
		
		register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		
		uint64_t address =  value_rs1 + imm11_0;
		// read 1 byte from [(rs1) + (imm)], and store it to address of rd.
		memory_read(address, &value_rd, 1);
		// extend rd to 64-bit value.
		value_rd = value_rd & 0x00000000000000ff;
		
    } else if (*(array) == 6) { 
		// lhu
        uint64_t register_rd = *(array + 2);
		uint64_t register_rs1 = *(array + 4);
		uint64_t imm11_0 = *(array + 7);
		
		register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		
		uint64_t address =  value_rs1 + imm11_0;
		// read 2 byte from [(rs1) + (imm)], and store it to address of rd.
		memory_read(address, &value_rd, 2);
		// extend rd to 64-bit value.
		value_rd = value_rd & 0x000000000000ffff;
		
    } else if (*(array) == 7) { 
		// lwu
        uint64_t register_rd = *(array + 2);
		uint64_t register_rs1 = *(array + 4);
		uint64_t imm11_0 = *(array + 7);
		
		register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		
		uint64_t address =  value_rs1 + imm11_0;
		// read 4 byte from [(rs1) + (imm)], and store it to address of rd.
		memory_read(address, &value_rd, 4);
		// extend rd to 64-bit value.
		value_rd = value_rd & 0x00000000ffffffff;

    } else if (*(array) == 8) { 
		// addi 
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm11_0 = *(array + 7);
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
        /* unsigned -> signed
		 * rd = rs1 + imm
		 * signed -> unsigned
        */
		signed_rs1 = *(int64_t *) &value_rs1;
        signed_rd = signed_rs1 + value_imm11_0;
        *(int64_t *) &value_rd = signed_rd;
        
    } else if (*(array) == 9) { 
		// slli
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm4_0 = *(array + 7);
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
        
        value_rd = value_rs1 << value_imm4_0;
        
    } else if (*(array) == 10) { 
		// slti
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm11_0 = *(array + 7);
		int64_t signed_imm11_0; 
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
        // extend imm to 64-bit
		if (value_imm11_0 >= 0x800) {
			value_imm11_0 = value_imm11_0 | 0xfffffffffffff800;
		} else {
			value_imm11_0 = value_imm11_0 & 0x0000000000000fff;
		}
		// unsigned -> signed
        signed_rs1 = *(int64_t *) &value_rs1;
		signed_imm11_0 = *(int64_t *) &value_imm11_0;
		
        if (signed_rs1 < signed_imm11_0) {
            value_rd = 1;
        } else {
            value_rd = 0;
        }
      
    } else if (*(array) == 11) { 
		// sltiu
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm11_0 = *(array + 7);
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
        if(value_rs1 < value_imm11_0){
            value_rd = 1;
        }
        else{
            value_rd = 0;
        } 
		
    } else if (*(array) == 12) { 
		// xori
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm11_0 = *(array + 7);
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		// extend imm to 64-bit
		if (value_imm11_0 >= 0x800) {
			value_imm11_0 = value_imm11_0 | 0xfffffffffffff800;
		} else {
			value_imm11_0 = value_imm11_0 & 0x0000000000000fff;
		}
		
        value_rd = value_rs1 ^ value_imm11_0;
       
    } else if (*(array) == 13) { 
		// srli
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm4_0 = *(array + 7);
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		
        value_rd = value_rs1 >> value_imm4_0;
        
    } else if (*(array) == 14) { 
		// srai
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm4_0 = *(array + 7);
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
        /* unsigned -> signed
		 * arithmetic right shift
		 * signed -> unsigned
		*/
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rd = signed_rs1 >> value_imm4_0;
        *(int64_t *) &value_rd = signed_rd;
        
    } else if (*(array) == 15) { 
		// ori
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm11_0 = *(array + 7);
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		// extend imm to 64-bit
		if (value_imm11_0 >= 0x800) {
			value_imm11_0 = value_imm11_0 | 0xfffffffffffff800;
		} else {
			value_imm11_0 = value_imm11_0 & 0x0000000000000fff;
		}
		
        value_rd = value_rs1 | value_imm11_0;  
        
    } else if (*(array) == 16) { 
		// andi
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm11_0 = *(array + 7);
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		// extend imm to 64-bit
		if (value_imm11_0 >= 0x800) {
			value_imm11_0 = value_imm11_0 | 0xfffffffffffff800;
		} else {
			value_imm11_0 = value_imm11_0 & 0x0000000000000fff;
		}
		
        value_rd = value_rs1 & value_imm11_0;
        
    } else if (*(array) == 17) { 
		// auipc
        uint64_t register_rd = *(array + 2);
		uint64_t value_imm31_12 = *(array + 12);
		
		value_rd = pc + value_imm31_12;
		
    } else if (*(array) == 18) { 
		// addiw
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm11_0 = *(array + 7);
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
        // extend imm to 64-bit
		if (value_imm11_0 >= 0x800) {
			value_imm11_0 = value_imm11_0 | 0xfffffffffffff800;
		} else {
			value_imm11_0 = value_imm11_0 & 0x0000000000000fff;
		}
        /* unsigned -> signed
		 * rd = rs1 + imm
		 * signed -> unsigned
		*/
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rd = signed_rs1 + value_imm11_0;
        *(int64_t *) &value_rd = signed_rd;
        //bit mask, only need low 32 bits.
        value_rd = value_rd & 0xffffffff;
        // extend to 64-bit
		if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x00000000ffffffff; // positive result
        }
        
    } else if (*(array) == 19) { 
		// slliw
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm4_0 = *(array + 7);
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
        
        value_rd = value_rs1 << value_imm4_0;
		//bit mask, only need low 32 bits.
        value_rd = value_rd & 0xffffffff;
		// extend to 64-bit
		if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x00000000ffffffff; // positive result
        }
        
    } else if (*(array) == 20) { 
		// srliw
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm4_0 = *(array + 7);
        
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
        
        value_rd = value_rs1 >> value_imm4_0;
		//bit mask, only need low 32 bits.
        value_rd = value_rd & 0xffffffff;
		// extend to 64-bit
		if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x00000000ffffffff; // positive result
        }
		
    } else if (*(array) == 21) { 
		// sraiw
        uint64_t register_rd = *(array + 2);
        uint64_t register_rs1 = *(array + 4);
        uint64_t value_imm4_0 = *(array + 7);
		int32_t signed_32_rs1;
		
        register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
        // bit mask, only need low 32 bits.
        value_rs1 = value_rs1 & 0xffffffff;
		/* unsigned -> signed
		 * arithmetic right shift
		 * signed -> unsigned
		*/
		signed_32_rs1 = *(int32_t *) &value_rs1;
        signed_32_rs1 = signed_32_rs1 >> value_imm4_0;
		*(int64_t *) &value_rd = signed_32_rs1;
		// extend to 64-bit
		if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x00000000ffffffff; // positive result
        }
		
    } else if (*(array) == 22) { 
		// sb
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t imm4_0 = *(array + 8);
        uint64_t imm11_5 = *(array + 9);
		
		uint64_t imm11_0;
		imm11_5 = imm11_5 << 5;
		imm11_0 = imm11_5 + imm4_0;
		
        register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		// store 1 byte from rs2, and store it to [rs1 + imm].
		uint64_t address =  value_rs1 + imm11_0;
		//memory_write(address, value_rs2, 1);
    } else if (*(array) == 23) {
        // sh
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t imm4_0 = *(array + 8);
        uint64_t imm11_5 = *(array + 9);
		
		uint64_t imm11_0;
		imm11_5 = imm11_5 << 5;
		imm11_0 = imm11_5 + imm4_0;
		
        register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		// store 2 byte from rs2, and store it to [rs1 + imm].
		uint64_t address =  value_rs1 + imm11_0;
		//memory_write(address, value_rs2, 2);
    } else if (*(array) == 24) {
        // sw
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t imm4_0 = *(array + 8);
        uint64_t imm11_5 = *(array + 9);
		
		uint64_t imm11_0;
		imm11_5 = imm11_5 << 5;
		imm11_0 = imm11_5 + imm4_0;
		
        register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		// store 4 byte from rs2, and store it to [rs1 + imm].
		uint64_t address =  value_rs1 + imm11_0;
		//memory_write(address, value_rs2, 4);
    } else if (*(array) == 25) {
        // sd
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t imm4_0 = *(array + 8);
        uint64_t imm11_5 = *(array + 9);
		
		uint64_t imm11_0;
		imm11_5 = imm11_5 << 5;
		imm11_0 = imm11_5 + imm4_0;
		
        register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		// store 1 byte from rs2, and store it to [rs1 + imm].
		uint64_t address =  value_rs1 + imm11_0;
		//memory_write(address, value_rs2, 8);
    } else if (*(array) == 26) {
        // add
        uint64_t register_rs1 = *(array+4);
        uint64_t register_rs2 = *(array+5);
        uint64_t register_rd = *(array+2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        // add operation
        signed_rd = signed_rs1 + signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        
    } else if (*(array) == 27) {
        // sub
        uint64_t register_rs1 = *(array+4);
        uint64_t register_rs2 = *(array+5);
        uint64_t register_rd = *(array+2);
        
        register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        // sub operation
        signed_rd = signed_rs1 - signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        
    } else if (*(array) == 28) {
        // sll
        uint64_t register_rs1 = *(array+4);
        uint64_t register_rs2 = *(array+5);
        uint64_t register_rd = *(array+2);
        
        register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
        // logical left shift
        value_rd = value_rs1 << value_rs2;
        
    } else if (*(array) == 29) {
        // slt
        uint64_t register_rs1 = *(array+4);
        uint64_t register_rs2 = *(array+5);
        uint64_t register_rd = *(array+2);
        
        register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        if (signed_rs1 < signed_rs2) {
            value_rd = 1;
        } else {
            value_rd = 0;
        }
        
    } else if (*(array) == 30) {
        // sltu
        uint64_t register_rs1 = *(array+4);
        uint64_t register_rs2 = *(array+5);
        uint64_t register_rd = *(array+2);
        
        register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
        if (value_rs1 < value_rs2) {
            value_rd = 1;
        } else {
            value_rd = 0;
        }
        
    } else if (*(array) == 31) {
        // xor
        uint64_t register_rs1 = *(array+4);
        uint64_t register_rs2 = *(array+5);
        uint64_t register_rd = *(array+2);
        
        register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
        value_rd = value_rs1 ^ value_rs2;
        
    } else if (*(array) == 32) {
        // srl
        uint64_t register_rs1 = *(array+4);
        uint64_t register_rs2 = *(array+5);
        uint64_t register_rd = *(array+2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        value_rd = value_rs1 >> value_rs2;
        
    } else if (*(array) == 33) {
        // sra
        uint64_t register_rs1 = *(array+4);
        uint64_t register_rs2 = *(array+5);
        uint64_t register_rd = *(array+2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        //sra operation
        signed_rd = signed_rs1 >> signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        
    } else if (*(array) == 34) {
        // or
        uint64_t register_rs1 = *(array+4);
        uint64_t register_rs2 = *(array+5);
        uint64_t register_rd = *(array+2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
        
        //or operation
        value_rd = value_rs1 | value_rs2;
        
    } else if (*(array) == 35) {
        // and
        uint64_t register_rs1 = *(array+4);
        uint64_t register_rs2 = *(array+5);
        uint64_t register_rd = *(array+2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		
        value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        //and operation
        value_rd = value_rs1 & value_rs2;
        
        register_write(register_rd, value_rd);
    } else if (*(array) == 36) {
        //lui
        uint64_t register_rd = *(array + 2);
        uint64_t imm31_12 = *(array + 12);
		
		// logical right shift 12 bits
		imm31_12 = imm31_12 << 12;
		value_rd = imm31_12;
		
    } else if (*(array) == 37) { 
		// addw
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        /* add operation, at this time, value_rd should be 64-bit result from sign-extended 32-bit result.
         * 1. add signed value in 32-bit, ignore overflow.
         * 2. extend to 64 bits.
         */
        signed_rd = signed_rs1 + signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        // only need low 32 bits.
        value_rd = value_rd & 0xffffffff;
		// extend to 64-bit value
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
    } else if (*(array) == 38) { 
		// subw
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        /* sub operation, at this time, value_rd should be 64-bit result from sign-extended 32-bit result.
         * 1. rsd = rs1 - rs2, get signed value in 32-bit, ignore overflow.
         * 2. extend to 64 bits.
         */
        signed_rd = signed_rs1 - signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        // only need low 32 bits.
        value_rd = value_rd & 0xffffffff;
		// extend to 64-bit value
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
    } else if (*(array) == 39) { 
		// sllw
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        /* logical left operation on 32-bit values and produce signed 32-bit results.
         * 1. logical left rd = rs1 << rs2
         * 2. bitmasking 33~64 bits to 0.
         */
        value_rd = value_rs1 << value_rs2;
		// only need low 32 bits.
        value_rd = value_rd & 0x00000000ffffffff;
		// extend to 64-bit value
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
    } else if (*(array) == 40) { 
		// srlw
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        /* logical right operation on 32-bit values and produce signed 32-bit results.
         * 1. logical right rd = rs1 >> rs2
         * 2. bitmasking 33~64 bits to 0.
         */
        value_rd = value_rs1 >> value_rs2;
		// only need low 32 bits.
        value_rd = value_rd & 0x00000000ffffffff;
		// extend to 64-bit value
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
    } else if (*(array) == 41) {
		// sraw
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
		int32_t signed_32_rs1;
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        /* Arithmetic right operation on 32-bit values and produce signed 32-bit results.
         * 1. convert rs1 to 32-bit signed value
         * 2. arithmetic right shift rd = rs1 >> rs2
         * 3. convert rd to unsigned value
         * 4. bitmasking 33~64 bits to 0.
         */
        signed_32_rs1 = *(int32_t *) &value_rs1;
        signed_32_rs1 = signed_32_rs1 >> value_rs2;
        *(int64_t *) &value_rd = signed_32_rs1;
        // extend to 64-bit
		if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x00000000ffffffff; // positive result
        }
        
    } else if (*(array) == 42) { 
		// beq
		uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
		uint64_t imm4_1_11 = *(array + 10);
		uint64_t imm12_10_5 = *(array +11);
        // get imm_12_1 from imm4_1_11 and imm12_10_5.
		uint64_t imm_4_1;
		uint64_t imm_5_10;
		uint64_t imm_11;
		uint64_t imm_12;
		uint64_t imm_12_1;
		
		imm_4_1 = imm4_1_11 & 0x1e;
		imm_4_1 = imm_4_1 >> 1;
		
		imm_5_10 = imm12_10_5 & 0x3f;
		imm_5_10 = imm_5_10 << 4;
		
		imm_11 = imm4_1_11 & 0x1;
		imm_11 = imm_11 << 10;
		
		imm_12 = imm12_10_5 & 0x40;
		imm_12 = imm_12 << 5;
		
		imm_12_1 = imm_4_1 + imm_5_10 + imm_11 + imm_12;
		imm_12_1 = imm_12_1 << 1;
		
		//register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
		if (value_rs1 == value_rs2) {
			*new_pc = pc + imm_12_1;
		}
    } else if (*(array) == 43) {
		// bne
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
		uint64_t imm4_1_11 = *(array + 10);
		uint64_t imm12_10_5 = *(array +11);
		// get imm_12_1 from imm4_1_11 and imm12_10_5.
		uint64_t imm_4_1;
		uint64_t imm_5_10;
		uint64_t imm_11;
		uint64_t imm_12;
		uint64_t imm_12_1;
		
		imm_4_1 = imm4_1_11 & 0x1e;
		imm_4_1 = imm_4_1 >> 1;
		
		imm_5_10 = imm12_10_5 & 0x3f;
		imm_5_10 = imm_5_10 << 4;
		
		imm_11 = imm4_1_11 & 0x1;
		imm_11 = imm_11 << 10;
		
		imm_12 = imm12_10_5 & 0x40;
		imm_12 = imm_12 << 5;
		
		imm_12_1 = imm_4_1 + imm_5_10 + imm_11 + imm_12;
		imm_12_1 = imm_12_1 << 1;
		
		//register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
		if (value_rs1 != value_rs2) {
			*new_pc = pc + imm_12_1;
		}
    } else if (*(array) == 44) {
		// blt
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
		uint64_t imm4_1_11 = *(array + 10);
		uint64_t imm12_10_5 = *(array +11);
		// get imm_12_1 from imm4_1_11 and imm12_10_5.
		uint64_t imm_4_1;
		uint64_t imm_5_10;
		uint64_t imm_11;
		uint64_t imm_12;
		uint64_t imm_12_1;
		
		imm_4_1 = imm4_1_11 & 0x1e;
		imm_4_1 = imm_4_1 >> 1;
		
		imm_5_10 = imm12_10_5 & 0x3f;
		imm_5_10 = imm_5_10 << 4;
		
		imm_11 = imm4_1_11 & 0x1;
		imm_11 = imm_11 << 10;
		
		imm_12 = imm12_10_5 & 0x40;
		imm_12 = imm_12 << 5;
		
		imm_12_1 = imm_4_1 + imm_5_10 + imm_11 + imm_12;
		imm_12_1 = imm_12_1 << 1;
		
		//register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
		// convert unsigned to signed.
		signed_rs1 = *(int64_t *) &value_rs1;
		signed_rs2 = *(int64_t *) &value_rs2;
		
		if (signed_rs1 < signed_rs2) {
			*new_pc = pc + imm_12_1;
		}
    } else if (*(array) == 45) {
		// bge
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
		uint64_t imm4_1_11 = *(array + 10);
		uint64_t imm12_10_5 = *(array +11);
		// get imm_12_1 from imm4_1_11 and imm12_10_5.
		uint64_t imm_4_1;
		uint64_t imm_5_10;
		uint64_t imm_11;
		uint64_t imm_12;
		uint64_t imm_12_1;
		
		imm_4_1 = imm4_1_11 & 0x1e;
		imm_4_1 = imm_4_1 >> 1;
		
		imm_5_10 = imm12_10_5 & 0x3f;
		imm_5_10 = imm_5_10 << 4;
		
		imm_11 = imm4_1_11 & 0x1;
		imm_11 = imm_11 << 10;
		
		imm_12 = imm12_10_5 & 0x40;
		imm_12 = imm_12 << 5;
		
		imm_12_1 = imm_4_1 + imm_5_10 + imm_11 + imm_12;
		imm_12_1 = imm_12_1 << 1;
		
		//register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
		// convert unsigned to signed.
		signed_rs1 = *(int64_t *) &value_rs1;
		signed_rs2 = *(int64_t *) &value_rs2;
		
		if (signed_rs1 >= signed_rs2) {
			*new_pc = pc + imm_12_1;
		}
    } else if (*(array) == 46) {
		// bltu
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
		uint64_t imm4_1_11 = *(array + 10);
		uint64_t imm12_10_5 = *(array +11);
		// get imm_12_1 from imm4_1_11 and imm12_10_5.
		uint64_t imm_4_1;
		uint64_t imm_5_10;
		uint64_t imm_11;
		uint64_t imm_12;
		uint64_t imm_12_1;
		
		imm_4_1 = imm4_1_11 & 0x1e;
		imm_4_1 = imm_4_1 >> 1;
		
		imm_5_10 = imm12_10_5 & 0x3f;
		imm_5_10 = imm_5_10 << 4;
		
		imm_11 = imm4_1_11 & 0x1;
		imm_11 = imm_11 << 10;
		
		imm_12 = imm12_10_5 & 0x40;
		imm_12 = imm_12 << 5;
		
		imm_12_1 = imm_4_1 + imm_5_10 + imm_11 + imm_12;
		imm_12_1 = imm_12_1 << 1;
		
		//register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
		if (value_rs1 < value_rs2) {
			*new_pc = pc + imm_12_1;
		}
    } else if (*(array) == 47) {
		// bgeu
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
		uint64_t imm4_1_11 = *(array + 10);
		uint64_t imm12_10_5 = *(array +11);
		// get imm_12_1 from imm4_1_11 and imm12_10_5.
		uint64_t imm_4_1;
		uint64_t imm_5_10;
		uint64_t imm_11;
		uint64_t imm_12;
		uint64_t imm_12_1;
		
		imm_4_1 = imm4_1_11 & 0x1e;
		imm_4_1 = imm_4_1 >> 1;
		
		imm_5_10 = imm12_10_5 & 0x3f;
		imm_5_10 = imm_5_10 << 4;
		
		imm_11 = imm4_1_11 & 0x1;
		imm_11 = imm_11 << 10;
		
		imm_12 = imm12_10_5 & 0x40;
		imm_12 = imm_12 << 5;
		
		imm_12_1 = imm_4_1 + imm_5_10 + imm_11 + imm_12;
		imm_12_1 = imm_12_1 << 1;
		
		//register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
		if (value_rs1 >= value_rs2) {
			*new_pc = pc + imm_12_1;
		}
    } else if (*(array) == 48) { // jalr
        uint64_t register_rd = *(array + 2);
		uint64_t register_rs1 = *(array + 4);
		uint64_t imm11_0 = *(array +7);
		
		//register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
		
		value_rs1 = register_rs1;
		
		value_rd = pc + 0x4;
		*new_pc = value_rs1 + imm11_0;
    } else if (*(array) == 49) { // jal
        uint64_t register_rd = *(array + 2);
		uint64_t imm20_10_1_11_19_12 = *(array +13);
		// get imm_20_1 from imm20_10_1_11_19_12.
		uint64_t imm_20;
		uint64_t imm_10_1;
		uint64_t imm_11;
		uint64_t imm_19_12;
		uint64_t imm_20_1;
		
		imm_10_1 = imm20_10_1_11_19_12 & 0x7fe00;
		imm_10_1 = imm_10_1 >> 9;
		
		imm_11 = imm20_10_1_11_19_12 & 0x00100;
		imm_11 = imm_11 << 2;
		
		imm_19_12 = imm20_10_1_11_19_12 & 0x000ff;
		imm_19_12 = imm_19_12 << 11;
		
		imm_20 = imm20_10_1_11_19_12 &80000;
		
		imm_20_1 = imm_10_1 + imm_11 + imm_19_12 + imm_20;
		imm_20_1 = imm_20_1 << 1;
		
		value_rd = pc + 0x4;
		*new_pc = pc +imm_20_1;
    } else if (*(array) == 50) { // mul
		// ERROR for ALL 4 mul functions: the result of function is always 0
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        signed_rd = signed_rs1 * signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        //bit masking
        value_rd = value_rd & 0xffffffff;
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
        
    } else if (*(array) == 51) { // mulh
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
        
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        signed_rd = signed_rs1 * signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        //bit masking
        value_rd = value_rd & 0xffffffff00000000;
        value_rd = value_rd >> 8;
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
        
    } else if (*(array) == 52) { // mulhsu
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        
        signed_rd = signed_rs1 * value_rs2;
        *(int64_t *) &value_rd = signed_rd;
        //bit masking
        value_rd = value_rd & 0xffffffff00000000;
        value_rd = value_rd >> 8;
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
        
    } else if (*(array) == 53) { // mulhu
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
        
        value_rd = value_rs1 * value_rs2;
        //bit masking
        value_rd = value_rd & 0xffffffff00000000;
        value_rd = value_rd >> 8;
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
        
    } else if(*(array) == 54) { //mulw
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
		
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
        
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        signed_rd = signed_rs1 * signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        //bit masking
        value_rd = value_rd & 0xffffffff;
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
        
    } else if (*(array) == 55) { // div
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        value_rs1 = register_rs1;
		value_rs2 = register_rs2;
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        signed_rd = signed_rs1 / signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        
	} else if (*(array) == 56) { // divu
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        value_rd = value_rs1 / value_rs2;
        
	}else if (*(array) == 57) { // rem
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        signed_rd = signed_rs1 % signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        
	}else if (*(array) == 58) { // remu
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        value_rd = value_rs1 / value_rs2;
        
	}else if (*(array) == 59) { // divw
		uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
	    value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        signed_rd = signed_rs1 / signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        //bit mask.
        value_rd = value_rd & 0xffffffff;
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
	}else if (*(array) == 60) { // divuw
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        value_rd = value_rs1 / value_rs2;
        //bit mask.
        value_rd = value_rd & 0xffffffff;
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
	}else if (*(array) == 61) { // remw
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        
		value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        // convert unsigned to signed
        signed_rs1 = *(int64_t *) &value_rs1;
        signed_rs2 = *(int64_t *) &value_rs2;
        
        signed_rd = signed_rs1 % signed_rs2;
        *(int64_t *) &value_rd = signed_rd;
        //bit mask.
        value_rd = value_rd & 0xffffffff;
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
	}else if (*(array) == 62) { // remuw
        uint64_t register_rs1 = *(array + 4);
        uint64_t register_rs2 = *(array + 5);
        uint64_t register_rd = *(array + 2);
        
        //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
        value_rs1 = register_rs1;
		value_rs2 = register_rs2;
		
        value_rd = value_rs1 % value_rs2;
        //bit mask.
        value_rd = value_rd & 0xffffffff;
        if (value_rd >= 0x80000000) {
            value_rd = value_rd | 0xffffffff80000000; // negative result
        } else {
            value_rd = value_rd & 0x000000007fffffff; // positive result
        }
        
	}
}

void stage_memory (struct stage_reg_w *new_w_reg) {
    
}

void stage_writeback (void) {
    register_write(register_rd, value_rd);
}

extern void execute_single_instruction (uint64_t pc, uint64_t * new_pc){
	uint64_t instr = fetch(pc, new_pc);
	// print instruction.
	printf("Executing instruction 0x%08lx at PC 0x%03lx\n", instr, pc);
	
	uint64_t *decoded = decode(instr);
	// print decoded array
	for (int i =0; i<20; i++) {
		printf("t = 0x%08lx\n", decoded[i]);
	}
	
	execute(decoded, pc, new_pc);
}

