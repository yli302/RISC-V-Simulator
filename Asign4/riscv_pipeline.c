#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include "riscv_sim_framework.h"
#include "riscv_pipeline_registers_vars.h"
/******************************************************************************************
 *
 * BTB table. 
 * valid: valid bit, 1 or 0.
 * tag: the rest bits of pc.
 * target: the target branch.
 *
 * create_BTB_entry: create a BTB_entry with valid bit, tag, and target.
 *  
 * calcu_index:
 *         input: pc or address
 *         output: calculate index of this pc or address
 * calcu_tag:
 *		   input: pc or address
 *		   output: caculate index of this pc or address
 * get_valid:
 *         input: BTB_entry
 *		   output: valid bit of this entry
 * get_tag:
 *         input: BTB_entry
 *		   output: tag in this entry
 * get_target:
 *         input: BTB_entry
 *		   output: target in this entry
 *****************************************************************************************/
uint64_t btb[32];
typedef struct BTB_block{
	int valid;
	int tag;
	int target;
}* BTB_entry;

BTB_entry create_BTB_entry(int valid, int tag, int target){
	BTB_entry entry = malloc(1*sizeof(struct BTB_block));
	
	entry -> valid = valid;
	entry -> tag = tag;
	entry -> target = target;
	return entry;
}

uint64_t calcu_index(uint64_t pc){
	return (pc>>2) & 0x1f;
}

uint64_t calcu_tag(uint64_t pc){
	return (pc>>7);
}

int get_valid(BTB_entry e){
	return e -> valid;
}

int get_tag(BTB_entry e){
	return e -> tag;
}

int get_target(BTB_entry e){
	return e -> target;
}

/******************************************************************************************
 *
 * Sign extend x bits to 64 bit.  
 *
 *****************************************************************************************/
uint64_t sign_extend(uint64_t value, uint64_t byte){
	switch(byte){
		case 1 :
			if (value > 0x7f) {
				value = value | 0xffffffffffffff80; // negative result
			}
			break;
		case 2 :
			if (value > 0x7fff) {
				value = value | 0xffffffffffff8000; // negative result
			}
			break;
		case 4 :
			if (value > 0x7fffffff) {
				value = value | 0xffffffff80000000; // negative result
			}
			break;
		default :
			break;
	}
	return value;
}
/******************************************************************************************
 *
 * Cache
 *
 * cache block:
 * creat_entry(): Create a Cache_block with valid bit, tag and data
 * update_block(): Update a Cache_block with new valid bit, tag, and data.
 * get_cache_validBit():
 * 		input: cache block entry
 *		output: valid bit of this entry
 * get_cache_tag():
 * 		input: cache block entry
 *		output: tag of this entry
 * get_cache_data():
 * 		input: cache block entry
 *		output: data of this entry
 * 
 * calcu_cache_index:
 *      input: pc or address
 *      output: calculate index of this pc or address
 *
 * calcu_cache_tag:
 *      input: pc or address
 *      output: calculate tag of this pc or address
 *
 * calcu_cache_offset:
 *      input: pc or address
 *      output: calculate offset of this pc or address
 *
 * calcu_cache_size:
 * calcu_cache_block_size:
 * calcu_cache_block_num:
 *****************************************************************************************/
uint64_t i_cache[512];
uint64_t d_cache[2048];
typedef struct Cache_block{
 	int valid;
	int tag;
	void* i_data; // for I-cache
	void* d_data; // for D-cache
}* Cache_entry;
 
Cache_entry create_cache_entry(int valid, int tag, void* i_data, void* d_data){
	Cache_entry entry = malloc(1*sizeof(struct Cache_block));
	
	entry -> valid = valid;
	entry -> tag = tag;
	entry -> i_data = i_data;
	entry -> d_data = d_data;
	//debug
	//printf("create\n\n\n off = 0x%.8lX\n", *(int*)entry -> i_data);
	return entry;
}

int get_cache_valid(Cache_entry e){
	return e -> valid;
}

int get_cache_tag(Cache_entry e){
	return e -> tag;
}

void *get_cache_i_data(Cache_entry e){
	//debug
	//printf("get\n\n\n get = 0x%.8lX\n", e -> i_data);
	return e -> i_data;
}

void *get_cache_d_data(Cache_entry e){
	return e -> d_data;
}
// i-cache caculation
int calcu_cache_i_index(uint64_t pc){
	return (pc>>4) & 0x1ff;
} 

int calcu_cache_i_tag(uint64_t pc){
	return (pc>>13);
}

int calcu_cache_i_offset(uint64_t pc){
	return pc & 0xf;
}
// d-cache caculation
int calcu_cache_d_index(uint64_t pc){
	return (pc>>2) & 0x7ff;
} 

int calcu_cache_d_tag(uint64_t pc){
	return (pc>>14);
}

int calcu_cache_d_offset(uint64_t pc){
	return pc & 0x3;
}

/******************************************************************************************
 *
 * Page table
 *
 ******************************************************************************************/
 
 typedef struct page_table_entry{
 	int valid;
	int* ppn; // the pointer to next level page table
}* PTE;
 
PTE create_PTE(int valid, int* ppn){
	PTE entry = malloc(1*sizeof(struct page_table_entry));
	
	entry -> valid = valid;
	entry -> ppn = ppn;
	return entry;
}

int* get_next_page_table(PTE curr_PTE){
	return curr_PTE -> ppn;
}

// bits calculation
int calcu_base_index(uint64_t vpn){
	return vpn >> 22;
} 

int calcu_second_index(uint64_t vpn){
	return (vpn >> 12) & 0x3ff;
}

int calcu_page_offset(uint64_t vpn){
	return vpn & 0xfff;
}

int physical_address(uint64_t ppn, uint64_t page_offset){
	return (ppn<<12) + page_offset;
}

/******************************************************************************************
 *
 * TLB
 *
 ******************************************************************************************/

uint64_t i_TLB[8];
uint64_t d_TLB[8];
typedef struct TLB_block{
 	int valid;
	int tag; // the tag from virtual page number 
	int* ppn; // the physical page number 
}* TLB_entry;
 
TLB_entry create_TLB_entry(int valid, int tag, int* ppn){
	TLB_entry entry = malloc(1*sizeof(struct TLB_block));
	
	entry -> valid = valid;
	entry -> tag = tag;
	entry -> ppn = ppn;
	return entry;
}

int get_TLB_valid(TLB_entry e){
	return e -> valid;
}

int get_TLB_tag(TLB_entry e){
	return e -> tag;
}

int *get_TLB_data(TLB_entry e){
	return e -> ppn;
}

// cache bits calculation
int calcu_TLB_index(uint64_t vpn){
	return vpn & 0x3;
} 

int calcu_TLB_tag(uint64_t vpn){
	return (vpn>>3);
}

int calcu_TLB_offset(uint64_t vpn){
	return vpn & 0xfff;
}

/******************************************************************************************
 *
 * stage_fetch, decode, execute, memory, writeback
 *
 *****************************************************************************************/ 
extern void stage_fetch (struct stage_reg_d *new_d_reg) {
	int *a = malloc(sizeof(uint8_t) * 16); 
	void * instr = a;// address of instruction
	
	int index = calcu_cache_i_index(get_pc());
	int tag = calcu_cache_i_tag(get_pc());
	int offset = calcu_cache_i_offset(get_pc());
	
	// if entry exist, hit
	// if entry dont exist, miss
	if(i_cache[index] != 0 &&
		get_cache_tag(i_cache[index]) == tag && get_cache_valid(i_cache[index]) == 1){
			instr = get_cache_i_data(i_cache[index]);
			instr = instr + offset;
			
			new_d_reg -> isICacheStall = 0;
			//debug
			//printf("in cache read 0x%.8lX at PC 0x%.3lX\n", *(int*)instr, get_pc());
			//printf("status = %d\n", memory_status(get_pc(), instr));
			//printf("there\n\n\n off = 0x%.8lX\n, offset = %d", *(int*)instr, index);
	}else{
		printf("Fetch instruction 0x%.8lX at PC 0x%.3lX\n\n", 0, get_pc());
		uint64_t size_in_bytes = 16; // read 128 bits from main memory
		int memory_read_boolean = 0;
		int i_cache_status = memory_status(get_pc(), instr);
		// when i-cache stall and memory_status become true, we get the data.
		if(i_cache_status == 1 || current_stage_d_register -> isICacheStall == 0){
			memory_read_boolean = memory_read (get_pc(), instr, size_in_bytes); // memory access
			new_d_reg -> isICacheStall = 1;
			//debug
			//printf("is memory read, read = %d, status = %d, isICStall = %d\n", memory_read_boolean, i_cache_status, current_stage_d_register -> isICacheStall);
		}
		if(memory_read_boolean == 1){
			i_cache[index] = create_cache_entry(1, tag, instr, instr);
			//debug
			//void* tk = get_cache_i_data(i_cache[index]);
			//tk = tk +4;
			//printf("here\n\n\n = 0x%.8lX\n\n", *(int*)tk);
			set_pc(get_pc());
		}
		new_d_reg -> new_pc = get_pc();
		new_d_reg -> instruction = 0;
		new_d_reg -> predictionWrong = 0;
		new_d_reg -> isPrediction = 0;
		return;
	}
	

	
	printf("Fetch instruction 0x%.8lX at PC 0x%.3lX\n\n", *(int*)instr, get_pc());
	
	// update next stage register
	new_d_reg -> new_pc = get_pc();
	new_d_reg -> instruction = *(int*)instr;
	new_d_reg -> predictionWrong = 0;
	new_d_reg -> isPrediction = 0;
	// Branch prediction
	uint64_t curr_index = calcu_index(get_pc());
	uint64_t curr_tag = calcu_tag(get_pc());
	if(btb[curr_index] != 0){
		if(get_tag(btb[curr_index]) == curr_tag){
			set_pc(get_target(btb[curr_index]));
			new_d_reg -> isPrediction = 1;
			//debug
			//printf("here!!\n\n\n\n\n\n");
			return;
		}
	}
	
	//debug
	/*if(btb[2] != 0 ){
		printf("tag = 0x%.8lX, target = 0x%.8lX\n", get_tag(btb[2]), get_target(btb[2]));
	}*/
	
	// move pc by 4 bits in normally sequential order 
	uint64_t new_pc = get_pc() + 0x4; 
	set_pc (new_pc);// move pc to a new address.
	
	// for debug, need to delete
	//printf("instr in regD is 0x%.8lX\n", current_stage_d_register->instruction); 
}

extern void stage_decode (struct stage_reg_x *new_x_reg) {
	// flush
	if(current_stage_m_register -> branchAndFlush == 1){
		new_x_reg -> instruction = 0x00000000;
		printf("Decode stage, inst=0x%.8lX.\n", 0);
		return;
	}
	if(current_stage_m_register -> predictionAndFlush == 1 && current_stage_m_register -> isPrediction == 1){
		new_x_reg -> instruction = 0x00000000;
		printf("Decode stage, inst=0x%.8lX.\n", 0);
		return;
	}
	
	
	uint64_t fetch_result = current_stage_d_register -> instruction;
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
	
	uint64_t tempInstr = current_stage_d_register->instruction;
	// stall
	if(current_stage_x_register -> instruction != 0){
		uint64_t *tempArr = current_stage_x_register -> array;
		// special data hazard stall for load instructions
		if(*tempArr == 1 || *tempArr == 2 || *tempArr == 3 || *tempArr == 4 ||
		  *tempArr == 5 || *tempArr == 6 || *tempArr == 7) {
			// if cur_reg_x -> rd address == rs1 address or cur_reg_x -> rd address == rs2 address
			if(*(tempArr + 2) == rs1 || *(tempArr + 2) == rs2){
				set_pc(current_stage_d_register -> new_pc);
				tempInstr = current_stage_x_register -> instruction;
				
				printf("Decode stage, inst=0x%.8lX.\n", tempInstr);
				new_x_reg -> new_pc = 0;
				new_x_reg -> instruction = 0;
				new_x_reg -> array = *array;
				new_x_reg -> isPrediction = current_stage_d_register -> isPrediction;
				new_x_reg -> predictionWrong = current_stage_d_register -> predictionWrong;
				return;
			}
		}
	}
	
	printf("Decode stage, inst=0x%.8lX.\n", tempInstr);
    //register_read (register_rs1, register_rs1, &value_rs1, &value_rs1);
    //register_read (register_rs1, register_rs2, &value_rs1, &value_rs2);
	// push corresponding uint64_t to decoded array.
	if(opcode == 0x03 && funct3 == 0x00){
        //For lb
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lb = 1;
        array[0] = lb;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x03 && funct3 == 0x01){
        //For lh
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lh = 2;
        array[0] = lh;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x03 && funct3 == 0x02){
        //For lw
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lw = 3;
        array[0] = lw;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x03 && funct3 == 0x03){
        //For ld
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t ld = 4;
        array[0] = ld;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x03 && funct3 == 0x04){
        //For lbu
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lbu = 5;
        array[0] = lbu;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x03 && funct3 == 0x05){
        //For lhu
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lhu = 6;
        array[0] = lhu;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x03 && funct3 == 0x06){
        //For lwu
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t lwu = 7;
        array[0] = lwu;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x13 && funct3 == 0x00){
        //For addi
        register_read (rs1, rs1, &value_rs1, &value_rs1);
		uint64_t addi = 8;
		array[0] = addi;
		array[1] = opcode;
		array[2] = rd;
		array[3] = funct3;
		array[4] = value_rs1;
		array[7] = imm11_0;
        array[14] = rs1;
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
			array[0] = slli;
			array[1] = opcode;
			array[2] = rd;
			array[3] = funct3;
			array[4] = value_rs1;
			array[7] = imm4_0;
            array[14] = rs1;
		}
    }
    else if(opcode == 0x13 && funct3 == 0x02){
        //For slti
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t slti = 10;
        array[0] = slti;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x13 && funct3 == 0x03){
        //For sltiu
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t sltiu = 11;
        array[0] = sltiu;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x13 && funct3 == 0x04){
        //For xori
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t xori = 12;
        array[0] = xori;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
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
			array[0] = srli;
			array[1] = opcode;
			array[2] = rd;
			array[3] = funct3;
			array[4] = value_rs1;
			array[7] = imm4_0;
            array[14] = rs1;
		}
		//For srai
		else if(imm11_5 == 0x20) {
			uint64_t srai = 14;
			uint64_t imm4_0 = imm11_0 & 0x01f;
			array[0] = srai;
			array[1] = opcode;
			array[2] = rd;
			array[3] = funct3;
			array[4] = value_rs1;
			array[7] = imm4_0;
            array[14] = rs1;
		}
    }
    else if(opcode == 0x13 && funct3 == 0x06){
        //For ori
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t ori = 15;
        array[0] = ori;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x13 && funct3 == 0x07){
        //For andi
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t andi = 16;
        array[0] = andi;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x17){
        //For auipc
        uint64_t auipc = 17;
        array[0] = auipc;
        array[1] = opcode;
        array[2] = rd;
        array[12] = imm31_12;
    }
    else if(opcode == 0x1b && funct3 == 0x00){
        //For addiw
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t addiw = 18;
        array[0] = addiw;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
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
			array[0] = slliw;
			array[1] = opcode;
			array[2] = rd;
			array[3] = funct3;
			array[4] = value_rs1;
			array[7] = imm4_0;
            array[14] = rs1;
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
			array[0] = srliw;
			array[1] = opcode;
			array[2] = rd;
			array[3] = funct3;
			array[4] = value_rs1;
			array[7] = imm4_0;
            array[14] = rs1;
		}
		//For sraiw
		else if(imm11_5 == 0x20){
			uint64_t sraiw = 21;
			uint64_t imm4_0 = imm11_0 & 0x01f;
			array[0] = sraiw;
			array[1] = opcode;
			array[2] = rd;
			array[3] = funct3;
			array[4] = value_rs1;
			array[7] = imm4_0;
            array[14] = rs1;
		}
    }
    else if(opcode == 0x23 && funct3 == 0x00){
        //For sb
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sb = 22;
        array[0] = sb;
        array[1] = opcode;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[8] = imm4_0;
        array[9] = imm11_5;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x23 && funct3 == 0x01){
        //For sh
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sh = 23;
        array[0] = sh;
        array[1] = opcode;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[8] = imm4_0;
        array[9] = imm11_5;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x23 && funct3 == 0x02){
        //For sw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sw = 24;
        array[0] = sw;
        array[1] = opcode;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[8] = imm4_0;
        array[9] = imm11_5;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x23 && funct3 == 0x03){
        //For sd
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sd = 25;
        array[0] = sd;
        array[1] = opcode;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[8] = imm4_0;
        array[9] = imm11_5;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x00 && funct7 == 0x00){
        //For add
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t add = 26;
        array[0] = add;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x00 && funct7 == 0x20){
        //For sub
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sub = 27;
        array[0] = sub;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x01 && funct7 == 0x00){
        //For sll
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sll = 28;
        array[0] = sll;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x02 && funct7 == 0x00){
        //For slt
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t slt = 29;
        array[0] = slt;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x03 && funct7 == 0x00){
        //For sltu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sltu = 30;
        array[0] = sltu;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x04 && funct7 == 0x00){
        //For xor
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t xor = 31;
        array[0] = xor;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x05 && funct7 == 0x00){
        //For srl
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t srl = 32;
        array[0] = srl;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x05 && funct7 == 0x20){
        //For sra
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sra = 33;
        array[0] = sra;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x06 && funct7 == 0x00){
        //For or
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t or = 34;
        array[0] = or;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x07 && funct7 == 0x00){
        //For and
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t and = 35;
        array[0] = and;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x37){
        //For lui
        uint64_t lui = 36;
        array[0] = lui;
        array[1] = opcode;
        array[2] = rd;
        array[12] = imm31_12;
    }
    else if(opcode == 0x3b && funct3 == 0x00 && funct7 == 0x00){
        //For addw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t addw = 37;
        array[0] = addw;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x3b && funct3 == 0x00 && funct7 == 0x20){
        //For subw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t subw = 38;
        array[0] = subw;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x3b && funct3 == 0x01 && funct7 == 0x00){
        //For sllw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t sllw = 39;
        array[0] = sllw;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x3b && funct3 == 0x05 && funct7 == 0x00){
        //For srlw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t srlw = 40;
        array[0] = srlw;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
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
        *(array+14) = rs1;
        *(array+15) = rs2;
    }
    else if(opcode == 0x63 && funct3 == 0x00){
        //For beq
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t beq = 42;
        array[0] = beq;
        array[1] = opcode;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[10] = imm4_1_11;
        array[11] = imm12_10_5;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x63 && funct3 == 0x01){
        //For bne
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t bne = 43;
        array[0] = bne;
        array[1] = opcode;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[10] = imm4_1_11;
        array[11] = imm12_10_5;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x63 && funct3 == 0x04){
        //For blt
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t blt = 44;
        array[0] = blt;
        array[1] = opcode;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[10] = imm4_1_11;
        array[11] = imm12_10_5;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x63 && funct3 == 0x05){
        //For bge
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t bge = 45;
        array[0] = bge;
        array[1] = opcode;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[10] = imm4_1_11;
        array[11] = imm12_10_5;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x63 && funct3 == 0x06){
        //For bltu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t bltu = 46;
        array[0] = bltu;
        array[1] = opcode;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[10] = imm4_1_11;
        array[11] = imm12_10_5;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x63 && funct3 == 0x07){
        //For bgeu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t bgeu = 47;
        array[0] = bgeu;
        array[1] = opcode;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[10] = imm4_1_11;
        array[11] = imm12_10_5;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x67 && funct3 == 0x00){
        //For jalr
        register_read (rs1, rs1, &value_rs1, &value_rs1);
        uint64_t jalr = 48;
        array[0] = jalr;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[7] = imm11_0;
        array[14] = rs1;
    }
    else if(opcode == 0x6F){
        //For jal
        uint64_t jal = 49;
        array[0] = jal;
        array[1] = opcode;
        array[2] = rd;
        array[13] = imm20_10_1_11_19_12;
    }
    else if(opcode == 0x33 && funct3 == 0x00 && funct7 == 0x01) {
        //For mul
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        int mul = 50;
        array[0] = mul;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x01 && funct7 == 0x01) {
        //For mulh
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        int mulh = 51;
        array[0] = mulh;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x02 && funct7 == 0x01) {
        //For mulhsu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        int mulhsu = 52;
        array[0] = mulhsu;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x33 && funct3 == 0x03 && funct7 == 0x01) {
        //For mulhu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        int mulhu = 53;
        array[0] = mulhu;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
    else if(opcode == 0x3b && funct3 == 0x00 && funct7 == 0x01) {
        //For mulw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        int mulw = 54;
        array[0] = mulw;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
	 else if(opcode == 0x33 && funct3 == 0x04 && funct7 == 0x01){
        //For div
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t div = 55;
        array[0] = div;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
	else if(opcode == 0x33 && funct3 == 0x05 && funct7 == 0x01){
        //For divu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t divu = 56;
        array[0] = divu;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
	else if(opcode == 0x33 && funct3 == 0x06 && funct7 == 0x01){
        //For rem
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t rem = 57;
        array[0] = rem;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
	else if(opcode == 0x33 && funct3 == 0x07 && funct7 == 0x01){
        //For remu
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t remu = 58;
        array[0] = remu;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
	else if(opcode == 0x3b && funct3 == 0x04 && funct7 == 0x01){
        //For divw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t divw = 59;
        array[0] = divw;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
	else if(opcode == 0x3b && funct3 == 0x05 && funct7 == 0x01){
        //For divuw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t divuw = 60;
        array[0] = divuw;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
	else if(opcode == 0x3b && funct3 == 0x06 && funct7 == 0x01){
        //For remw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t remw = 61;
        array[0] = remw;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
	else if(opcode == 0x3b && funct3 == 0x07 && funct7 == 0x01){
        //For remuw
        register_read (rs1, rs2, &value_rs1, &value_rs2);
        uint64_t remuw = 62;
        array[0] = remuw;
        array[1] = opcode;
        array[2] = rd;
        array[3] = funct3;
        array[4] = value_rs1;
        array[5] = value_rs2;
        array[6] = funct7;
        array[14] = rs1;
        array[15] = rs2;
    }
	
	// update next stage register
	new_x_reg -> new_pc = current_stage_d_register -> new_pc;
	new_x_reg -> instruction = current_stage_d_register -> instruction;
	new_x_reg -> array = array;
	new_x_reg -> isPrediction = current_stage_d_register -> isPrediction;
	new_x_reg -> predictionWrong = current_stage_d_register -> predictionWrong;

	// for debug, need to delete
	//int *a = new_x_reg -> array;
	//int haha = *(a);
	//printf("The index of instruction is %d\n", haha);
}

extern void stage_execute (struct stage_reg_m *new_m_reg) {
	// flush
	if(current_stage_m_register -> branchAndFlush == 1){
		new_m_reg -> instruction = 0x00000000;
		new_m_reg -> branchAndFlush = 0;
		printf("Execute stage, inst=0x%.8lX.\n", 0);
		return;
	}
	if(current_stage_m_register -> predictionAndFlush == 1 && current_stage_m_register -> isPrediction == 1){
		new_m_reg -> instruction = 0x00000000;
		new_m_reg -> predictionAndFlush = 0;
		new_m_reg -> isPrediction = 0;
		printf("Execute stage, inst=0x%.8lX.\n", 0);
		return;
	}
	printf("Execute stage, inst=0x%.8lX.\n", current_stage_x_register->instruction);
	
	uint64_t value_rs1; // it is a unsigned 32-bit value
	uint64_t value_rs2; // it is a unsigned 32-bit value
	int64_t signed_rs1; // it is a signed 32-bit value
	int64_t signed_rs2; // it is a signed 32-bit value
	int64_t signed_rd; // it is a signed 32-bit value
	
	// variables pass to next stages
	uint64_t value_rd = 0; // it is a unsigned 64-bit value
	uint64_t address_rd = 0; //it is a unsigned 64-bit value
	uint64_t address_memory = 0;
	uint64_t byte_memory = 0;
	uint64_t address_target = 0;
	new_m_reg -> isStore = 0;
	new_m_reg -> isLoad = 0;
	new_m_reg -> isBranch = 0;
	new_m_reg -> branchAndFlush = 0;
	new_m_reg -> predictionAndFlush = 0;
	new_m_reg -> new_pc = current_stage_x_register -> new_pc;
	new_m_reg -> isPrediction = current_stage_x_register -> isPrediction;
	new_m_reg -> predictionWrong = current_stage_x_register -> predictionWrong;
	
	// if we get execute signal, then execute.
	if(current_stage_x_register->instruction!=0){
		uint64_t *array = current_stage_x_register -> array;
		
		// give rs1 and rs2 value base on the decode in last circle.
		value_rs1 = array [4];
		value_rs2 = array [5];
		
		//debug
		//printf("rs1 = %d\n", value_rs1);
		
		uint64_t address_rs1; // address of resource register 1
		uint64_t address_rs2; // address of resource register 2
		uint64_t previous_cycle_address_rd;
		uint64_t previous_cycle_value_rd;
		address_rs1 = array [14];
		address_rs2 = array [15];
		previous_cycle_address_rd = current_stage_m_register -> address_rd;
		previous_cycle_value_rd = current_stage_m_register -> value_rd;
		// check WX forward
		if (address_rs1 == current_stage_w_register -> address_rd){
			value_rs1 = current_stage_w_register -> value_rd;
		}
		if (address_rs2 == current_stage_w_register -> address_rd){
			value_rs2 = current_stage_w_register -> value_rd;
		}
		
		// check MX forward and rewrite the value of rs1 or rs2 as the value of previous cycle rd value.
		if (address_rs1 == previous_cycle_address_rd) {
			value_rs1 = previous_cycle_value_rd;
			//debug
			//printf("rs1 in MX= %d\n", value_rs1);
			//printf("rs1A = %d, rdA = %d\n", address_rs1, previous_cycle_address_rd);
		}
		if (address_rs2 == previous_cycle_address_rd) {
			value_rs2 = previous_cycle_value_rd;
		}
		
		
		// execution implementation
		if (array[0] == 1) {  
			// lb 
			uint64_t register_rd = array [2];
			uint64_t imm11_0 = array [7];
			
			address_rd = register_rd;
			
			uint64_t address =  value_rs1 + imm11_0;
			// read 1 byte from [(rs1) + (imm)], and store it to address of rd.
			//memory_read(address, &value_rd, 1);
			// extend rd to 64-bit value. (negative # is different from positive #)
			//if (value_rd >= 0x80) {
			//    value_rd = value_rd | 0xffffffffffffff80;
			//} else {
			//	value_rd = value_rd & 0x00000000000000ff;
			//}
			address_memory = address;
			new_m_reg -> isLoad = 1;
			byte_memory = 1;
			//debug
			//printf("here? lb rs1 %d address %d\n", value_rs1, address_memory);
			new_m_reg -> load_signed = 1;
		} else if (array[0] == 2) {  
			// lh
			uint64_t register_rd = array[2];
			uint64_t imm11_0 = array[7];
			
			address_rd = register_rd;
			
			uint64_t address =  value_rs1 + imm11_0;
			// read 2 byte from [(rs1) + (imm)], and store it to address of rd.
			//memory_read(address, &value_rd, 2);
			// extend rd to 64-bit value. (negative # is different from positive #)
			//if (value_rd >= 0x8000) {
			//	value_rd = value_rd | 0xffffffffffff8000;
			//} else {
			//	value_rd = value_rd & 0x000000000000ffff;
			//}
			address_memory = address;
			new_m_reg -> isLoad = 1;
			byte_memory = 2;
			new_m_reg -> load_signed = 1;
		} else if (array[0] == 3) { 
			// lw
			uint64_t register_rd = array[2];
			uint64_t imm11_0 = array[7];
			
			address_rd = register_rd;
			
			uint64_t address =  value_rs1 + imm11_0;
			// read 4 byte from [(rs1) + (imm)], and store it to address of rd.
			//memory_read(address, &value_rd, 4);
			// extend rd to 64-bit value. (negative # is different from positive #)
			//if (value_rd >= 0x80000000) {
			//	value_rd = value_rd | 0xffffffff80000000;
			//} else {
			//	value_rd = value_rd & 0x00000000ffffffff;
			//}
			address_memory = address;
			new_m_reg -> isLoad = 1;
			byte_memory = 4;
			new_m_reg -> load_signed = 1;
		} else if (array[0] == 4) { 
			// ld
			uint64_t register_rd = array[2];
			uint64_t imm11_0 = array[7];
			
			address_rd = register_rd;
			
			uint64_t address =  value_rs1 + imm11_0;
			// read 8 byte from [(rs1) + (imm)], and store it to address of rd.
			//memory_read(address, &value_rd, 8);
			address_memory = address;
			new_m_reg -> isLoad = 1;
			byte_memory = 8;
			//debug
			//printf("here?ld\n");
		} else if (array[0] == 5) { 
			// lbu
			uint64_t register_rd = array[2];
			uint64_t imm11_0 = array[7];
			
			address_rd = register_rd;
			
			uint64_t address =  value_rs1 + imm11_0;
			// read 1 byte from [(rs1) + (imm)], and store it to address of rd.
			//memory_read(address, &value_rd, 1);
			// extend rd to 64-bit value.
			//value_rd = value_rd & 0x00000000000000ff;
			address_memory = address;
			new_m_reg -> isLoad = 1;
			byte_memory = 1;
		} else if (array[0] == 6) { 
			// lhu
			uint64_t register_rd =array[2];
			uint64_t imm11_0 = array[7];
			
			address_rd = register_rd;
			
			uint64_t address =  value_rs1 + imm11_0;
			// read 2 byte from [(rs1) + (imm)], and store it to address of rd.
			//memory_read(address, &value_rd, 2);
			// extend rd to 64-bit value.
			//value_rd = value_rd & 0x000000000000ffff;
			address_memory = address;
			new_m_reg -> isLoad = 1;
			byte_memory = 2;
		} else if (array[0] == 7) { 
			// lwu
			uint64_t register_rd = array[2];
			uint64_t imm11_0 = array[7];
			
			address_rd = register_rd;
			
			uint64_t address =  value_rs1 + imm11_0;
			// read 4 byte from [(rs1) + (imm)], and store it to address of rd.
			//memory_read(address, &value_rd, 4);
			// extend rd to 64-bit value.
			//value_rd = value_rd & 0x00000000ffffffff;
			address_memory = address;
			new_m_reg -> isLoad = 1;
			byte_memory = 4;
		} else if (array[0] == 8) { 
			// addi 
			uint64_t register_rd = array[2];
			uint64_t value_imm11_0 = array[7];
			
			address_rd = register_rd;

			/* unsigned -> signed
			 * rd = rs1 + imm
			 * signed -> unsigned
			*/
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rd = signed_rs1 + value_imm11_0;
			*(int64_t *) &value_rd = signed_rd;
			//debug
			//printf("rs1 = %d, imm = %d, rd = %d\n", signed_rs1, value_imm11_0, value_rd);
		} else if (array[0] == 9) { 
			// slli
			uint64_t register_rd = array[2];
			uint64_t value_imm4_0 = array[7];
			
			address_rd = register_rd;
			
			value_rd = value_rs1 << value_imm4_0;
		} else if (array[0] == 10) { 
			// slti
			uint64_t register_rd = array[2];
			uint64_t value_imm11_0 = array[7];
			int64_t signed_imm11_0; 
			
			address_rd = register_rd;
			
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
		} else if (array[0] == 11) { 
			// sltiu
			uint64_t register_rd = array[2];
			uint64_t value_imm11_0 = array[7];
			
			address_rd = register_rd;

			if(value_rs1 < value_imm11_0){
				value_rd = 1;
			}
			else{
				value_rd = 0;
			} 
		} else if (array[0] == 12) { 
			// xori
			uint64_t register_rd = array[2];
			uint64_t value_imm11_0 = array[7];
			
			address_rd = register_rd;
					
			// extend imm to 64-bit
			if (value_imm11_0 >= 0x800) {
				value_imm11_0 = value_imm11_0 | 0xfffffffffffff800;
			} else {
				value_imm11_0 = value_imm11_0 & 0x0000000000000fff;
			}
			
			value_rd = value_rs1 ^ value_imm11_0;
		} else if (array[0] == 13) { 
			// srli
			uint64_t register_rd = array[2];
			uint64_t value_imm4_0 = array[7];
			
			address_rd = register_rd;
					
			value_rd = value_rs1 >> value_imm4_0;
		} else if (array[0] == 14) { 
			// srai
			uint64_t register_rd = array[2];
			uint64_t value_imm4_0 = array[7];
			
			address_rd = register_rd;
			
			/* unsigned -> signed
			 * arithmetic right shift
			 * signed -> unsigned
			*/
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rd = signed_rs1 >> value_imm4_0;
			*(int64_t *) &value_rd = signed_rd;
		} else if (array[0] == 15) { 
			// ori
			uint64_t register_rd = array[2];
			uint64_t value_imm11_0 = array[7];
			
			address_rd = register_rd;	
			
			// extend imm to 64-bit
			if (value_imm11_0 >= 0x800) {
				value_imm11_0 = value_imm11_0 | 0xfffffffffffff800;
			} else {
				value_imm11_0 = value_imm11_0 & 0x0000000000000fff;
			}
			
			value_rd = value_rs1 | value_imm11_0;
		} else if (array[0] == 16) { 
			// andi
			uint64_t register_rd = array[2];
			uint64_t value_imm11_0 = array[7];
			
			address_rd = register_rd;
			
			// extend imm to 64-bit
			if (value_imm11_0 >= 0x800) {
				value_imm11_0 = value_imm11_0 | 0xfffffffffffff800;
			} else {
				value_imm11_0 = value_imm11_0 & 0x0000000000000fff;
			}
			
			value_rd = value_rs1 & value_imm11_0;
		} else if (array[0] == 17) { 
			// auipc
			uint64_t register_rd = array[2];
			uint64_t value_imm31_12 = array[12];
			
			address_rd = register_rd;
			
			value_rd = current_stage_x_register -> new_pc + value_imm31_12;
		} else if (array[0] == 18) { 
			// addiw
			uint64_t register_rd = array[2];
			uint64_t value_imm11_0 = array[7];
			
			address_rd = register_rd;
			
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
		} else if (array[0] == 19) { 
			// slliw
			uint64_t register_rd = array[2];
			uint64_t value_imm4_0 = array[7];
			
			address_rd = register_rd;
			
			value_rd = value_rs1 << value_imm4_0;
			//bit mask, only need low 32 bits.
			value_rd = value_rd & 0xffffffff;
			// extend to 64-bit
			if (value_rd >= 0x80000000) {
				value_rd = value_rd | 0xffffffff80000000; // negative result
			} else {
				value_rd = value_rd & 0x00000000ffffffff; // positive result
			}
		} else if (array[0] == 20) { 
			// srliw
			uint64_t register_rd = array[2];
			uint64_t value_imm4_0 = array[7];
			
			address_rd = register_rd;
			
			value_rd = value_rs1 >> value_imm4_0;
			//bit mask, only need low 32 bits.
			value_rd = value_rd & 0xffffffff;
			// extend to 64-bit
			if (value_rd >= 0x80000000) {
				value_rd = value_rd | 0xffffffff80000000; // negative result
			} else {
				value_rd = value_rd & 0x00000000ffffffff; // positive result
			}
		} else if (array[0] == 21) { 
			// sraiw
			uint64_t register_rd = array[2];
			uint64_t value_imm4_0 = array[7];
			int32_t signed_32_rs1;
			
			address_rd = register_rd;
			
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
		} else if (array[0] == 22) { 
			// sb
			address_rd = array[15];
			uint64_t imm4_0 = array[8];
			uint64_t imm11_5 = array[9];
			
			uint64_t imm11_0;
			imm11_5 = imm11_5 << 5;
			imm11_0 = imm11_5 + imm4_0;
			
			// store 1 byte from rs2, and store it to [rs1 + imm].
			uint64_t address =  value_rs1 + imm11_0;
			value_rd = value_rs2;
			new_m_reg -> isStore = 1;
			byte_memory = 1;
			address_memory = address;
			//debug
			//printf("here? sb %d address %d\n", value_rs1, address_memory);
		} else if (array[0] == 23) {
			// sh
			address_rd = array[15];
			uint64_t imm4_0 = array[8];
			uint64_t imm11_5 = array[9];
			
			uint64_t imm11_0;
			imm11_5 = imm11_5 << 5;
			imm11_0 = imm11_5 + imm4_0;
				
			// store 2 byte from rs2, and store it to [rs1 + imm].
			uint64_t address =  value_rs1 + imm11_0;
			value_rd = value_rs2;
			new_m_reg -> isStore = 1;
			byte_memory = 2;
			address_memory = address;
		} else if (array[0] == 24) {
			// sw
			address_rd = array[15];
			uint64_t imm4_0 = array[8];
			uint64_t imm11_5 = array[9];
			
			uint64_t imm11_0;
			imm11_5 = imm11_5 << 5;
			imm11_0 = imm11_5 + imm4_0;
					
			// store 4 byte from rs2, and store it to [rs1 + imm].
			uint64_t address =  value_rs1 + imm11_0;
			value_rd = value_rs2;
			new_m_reg -> isStore = 1;
			byte_memory = 4;
			address_memory = address;
		} else if (array[0] == 25) {
			// sd
			address_rd = array[15];
			uint64_t imm4_0 = array[8];
			uint64_t imm11_5 = array[9];
			
			uint64_t imm11_0;
			imm11_5 = imm11_5 << 5;
			imm11_0 = imm11_5 + imm4_0;
			
			// store 8 byte from rs2, and store it to [rs1 + imm].
			uint64_t address =  value_rs1 + imm11_0;
			value_rd = value_rs2;
			new_m_reg -> isStore = 1;
			byte_memory = 8;
			//debug
			//printf("here? sd %d\n", value_rs2);
			address_memory = address;
		} else if (array[0] == 26) {
			// add
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			// convert unsigned to signed
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rs2 = *(int64_t *) &value_rs2;
			
			// add operation
			signed_rd = signed_rs1 + signed_rs2;
			*(int64_t *) &value_rd = signed_rd;
		} else if (array[0] == 27) {
			// sub
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			// convert unsigned to signed
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rs2 = *(int64_t *) &value_rs2;
			
			// sub operation
			signed_rd = signed_rs1 - signed_rs2;
			*(int64_t *) &value_rd = signed_rd;
		} else if (array[0] == 28) {
			// sll
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			// logical left shift
			value_rd = value_rs1 << value_rs2;
		} else if (array[0] == 29) {
			// slt
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			// convert unsigned to signed
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rs2 = *(int64_t *) &value_rs2;
			
			if (signed_rs1 < signed_rs2) {
				value_rd = 1;
			} else {
				value_rd = 0;
			}
		} else if (array[0] == 30) {
			// sltu
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			if (value_rs1 < value_rs2) {
				value_rd = 1;
			} else {
				value_rd = 0;
			}
		} else if (array[0] == 31) {
			// xor
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;

			value_rd = value_rs1 ^ value_rs2;
		} else if (array[0] == 32) {
			// srl
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			value_rd = value_rs1 >> value_rs2;
		} else if (array[0] == 33) {
			// sra
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			// convert unsigned to signed
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rs2 = *(int64_t *) &value_rs2;
			
			//sra operation
			signed_rd = signed_rs1 >> signed_rs2;
			*(int64_t *) &value_rd = signed_rd;
		} else if (array[0] == 34) {
			// or
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			//or operation
			value_rd = value_rs1 | value_rs2;
		} else if (array[0] == 35) {
			// and
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			//and operation
			value_rd = value_rs1 & value_rs2;
		} else if (array[0] == 36) {
			//lui
			uint64_t register_rd = array[2];
			uint64_t imm31_12 = array[12];
			
			address_rd = register_rd;
			
			// logical right shift 12 bits
			imm31_12 = imm31_12 << 12;
			value_rd = imm31_12;
		} else if (array[0] == 37) { 
			// addw
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
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
		} else if (array[0] == 38) { 
			// subw
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
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
		} else if (array[0] == 39) { 
			// sllw
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
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
		} else if (array[0] == 40) { 
			// srlw
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
				
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
		} else if (array[0] == 41) {
			// sraw
			uint64_t register_rd = array[2];
			int32_t signed_32_rs1;
			
			address_rd = register_rd;		
			
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
		} else if (array[0] == 42) { 
			// beq
			uint64_t imm4_1_11 = array[10];
			uint64_t imm12_10_5 = array[11];
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
			
			// extend to 64-bit
			if(imm_12_1 >= 0x1000){
				imm_12_1 = imm_12_1 | 0xffffffffffffe000;
			}
			
			if (value_rs1 == value_rs2) {
				address_target = current_stage_x_register -> new_pc + imm_12_1;
				
				if(imm_12_1 >= 0x1000){
					int index = calcu_index(current_stage_x_register -> new_pc);
					int tag = calcu_tag(current_stage_x_register -> new_pc);
					int valid = 1;
					btb[index] = create_BTB_entry(valid, tag, address_target);
				}
				// if this is not a prediction, then flush
				if(current_stage_x_register -> isPrediction == 0){
					new_m_reg -> branchAndFlush = 1;
				}
				new_m_reg -> isBranch = 1;
			} else {
				new_m_reg -> predictionWrong = 1;
				new_m_reg -> predictionAndFlush = 1;
			}
		} else if (array[0] == 43) {
			// bne
			uint64_t imm4_1_11 = array[10];
			uint64_t imm12_10_5 = array[11];
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
			
			// extend to 64-bit
			if(imm_12_1 >= 0x1000){
				imm_12_1 = imm_12_1 | 0xffffffffffffe000;
			}
			
			if (value_rs1 != value_rs2) {
				address_target = current_stage_x_register -> new_pc + imm_12_1;
				
				if(imm_12_1 >= 0x1000){
					int index = calcu_index(current_stage_x_register -> new_pc);
					int tag = calcu_tag(current_stage_x_register -> new_pc);
					int valid = 1;
					btb[index] = create_BTB_entry(valid, tag, address_target);
				}
				// if this is not a prediction, then flush
				if(current_stage_x_register -> isPrediction == 0){
					new_m_reg -> branchAndFlush = 1;
				}
				new_m_reg -> isBranch = 1;
			} else {
				new_m_reg -> predictionWrong = 1;
				new_m_reg -> predictionAndFlush = 1;
			}
		} else if (array[0] == 44) {
			// blt
			uint64_t imm4_1_11 = array[10];
			uint64_t imm12_10_5 = array[11];
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

			
			// convert unsigned to signed.
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rs2 = *(int64_t *) &value_rs2;
			// extend to 64-bit
			if(imm_12_1 >= 0x1000){
				imm_12_1 = imm_12_1 | 0xffffffffffffe000;
			}
			if (signed_rs1 < signed_rs2) {
				address_target = current_stage_x_register -> new_pc + imm_12_1;
				
				if(imm_12_1 >= 0x1000){
					int index = calcu_index(current_stage_x_register -> new_pc);
					int tag = calcu_tag(current_stage_x_register -> new_pc);
					int valid = 1;
					btb[index] = create_BTB_entry(valid, tag, address_target);
					//debug
					//printf("tag = 0x%.8lX, target = 0x%.8lX\n", current_stage_x_register -> new_pc, imm_12_1);
				}
				// if this is not a prediction, then flush
				if(current_stage_x_register -> isPrediction == 0){
					new_m_reg -> branchAndFlush = 1;
				}
				new_m_reg -> isBranch = 1;
			} else {
				new_m_reg -> predictionWrong = 1;
				new_m_reg -> predictionAndFlush = 1;
			}
		} else if (array[0] == 45) {
			// bge
			uint64_t imm4_1_11 = array[10];
			uint64_t imm12_10_5 = array[11];
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
			
		
			// convert unsigned to signed.
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rs2 = *(int64_t *) &value_rs2;
			// extend to 64-bit
			if(imm_12_1 >= 0x1000){
				imm_12_1 = imm_12_1 | 0xffffffffffffe000;
			}
			
			if (signed_rs1 >= signed_rs2) {
				address_target = current_stage_x_register -> new_pc + imm_12_1;
				
				if(imm_12_1 >= 0x1000){
					int index = calcu_index(current_stage_x_register -> new_pc);
					int tag = calcu_tag(current_stage_x_register -> new_pc);
					int valid = 1;
					btb[index] = create_BTB_entry(valid, tag, address_target);
				}
				// if this is not a prediction, then flush
				if(current_stage_x_register -> isPrediction == 0){
					new_m_reg -> branchAndFlush = 1;
				}
				new_m_reg -> isBranch = 1;
			} else {
				new_m_reg -> predictionWrong = 1;
				new_m_reg -> predictionAndFlush = 1;
			}
		} else if (array[0] == 46) {
			// bltu
			uint64_t imm4_1_11 = array[10];
			uint64_t imm12_10_5 = array[11];
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

			// extend to 64-bit
			if(imm_12_1 >= 0x1000){
				imm_12_1 = imm_12_1 | 0xffffffffffffe000;
			}
			
			if (value_rs1 < value_rs2) {
				address_target = current_stage_x_register -> new_pc + imm_12_1;
				
				if(imm_12_1 >= 0x1000){
					int index = calcu_index(current_stage_x_register -> new_pc);
					int tag = calcu_tag(current_stage_x_register -> new_pc);
					int valid = 1;
					btb[index] = create_BTB_entry(valid, tag, address_target);
				}
				// if this is not a prediction, then flush
				if(current_stage_x_register -> isPrediction == 0){
					new_m_reg -> branchAndFlush = 1;
				}
				new_m_reg -> isBranch = 1;
			} else {
				new_m_reg -> predictionWrong = 1;
				new_m_reg -> predictionAndFlush = 1;
			}
		} else if (array[0] == 47) {
			// bgeu
			uint64_t imm4_1_11 = array[10];
			uint64_t imm12_10_5 = array[11];
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

			// extend to 64-bit
			if(imm_12_1 >= 0x1000){
				imm_12_1 = imm_12_1 | 0xffffffffffffe000;
			}
			
			if (value_rs1 >= value_rs2) {
				address_target = current_stage_x_register -> new_pc + imm_12_1;
				
				if(imm_12_1 >= 0x1000){
					int index = calcu_index(current_stage_x_register -> new_pc);
					int tag = calcu_tag(current_stage_x_register -> new_pc);
					int valid = 1;
					btb[index] = create_BTB_entry(valid, tag, address_target);
				}
				// if this is not a prediction, then flush
				if(current_stage_x_register -> isPrediction == 0){
					new_m_reg -> branchAndFlush = 1;
				}
				new_m_reg -> isBranch = 1;
			} else {
				new_m_reg -> predictionWrong = 1;
				new_m_reg -> predictionAndFlush = 1;
			}
		} else if (array[0] == 48) { // jalr
			uint64_t register_rd = array[2];
			uint64_t imm11_0 = array[7];
			
			address_rd = register_rd;
			
			// extend to 64-bit
			if(imm11_0 >= 0x800){
				imm11_0 = imm11_0 | 0xfffffffffffff800;
			}
			value_rd = current_stage_x_register -> new_pc + 0x4;
			address_target = imm11_0;
			
			if(imm11_0 >= 0x800){
					int index = calcu_index(current_stage_x_register -> new_pc);
					int tag = calcu_tag(current_stage_x_register -> new_pc);
					int valid = 1;
					btb[index] = create_BTB_entry(valid, tag, address_target);
			}
			new_m_reg -> branchAndFlush = 1;
			new_m_reg -> isBranch = 1;
		} else if (array[0] == 49) { // jal
			uint64_t register_rd = array[2];
			uint64_t imm20_10_1_11_19_12 = array[13];
			
			address_rd = register_rd;
			
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
			
			// extend to 64-bit
			if(imm_20_1 >= 0x80000){
				imm_20_1 = imm_20_1 | 0xfffffffffff80000;
			}
			
			value_rd = current_stage_x_register -> new_pc + 0x4;
			address_target = imm_20_1;
			
			if(imm_20_1 >= 0x80000){
				int index = calcu_index(current_stage_x_register -> new_pc);
				int tag = calcu_tag(current_stage_x_register -> new_pc);
				int valid = 1;
				btb[index] = create_BTB_entry(valid, tag, address_target);
			}
				
			new_m_reg -> branchAndFlush = 1;
			new_m_reg -> isBranch = 1;
		} else if (array[0] == 50) { // mul
			// ERROR for ALL 4 mul functions: the result of function is always 0
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			// convert unsigned to signed
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rs2 = *(int64_t *) &value_rs2;
			
			signed_rd = signed_rs1 * signed_rs2;
			*(int64_t *) &value_rd = signed_rd;
			//bit masking
			value_rd = value_rd & 0xffffffffffffffff;
			if (value_rd >= 0x80000000) {
				value_rd = value_rd | 0xffffffff80000000; // negative result
			} else {
				value_rd = value_rd & 0x000000007fffffff; // positive result
			}
		} else if (array[0] == 51) { // mulh
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			// convert unsigned to signed
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rs2 = *(int64_t *) &value_rs2;
			
			signed_rd = (signed_rs1 * signed_rs2) >> 32;
			*(int64_t *) &value_rd = signed_rd;
			//bit masking
			if (value_rd >= 0x80000000) {
				value_rd = value_rd | 0xffffffff80000000; // negative result
			} else {
				value_rd = value_rd & 0x000000007fffffff; // positive result
			}
		} else if (array[0] == 52) { // mulhsu
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
		
			// convert unsigned to signed
			signed_rs1 = *(int64_t *) &value_rs1;
			
			signed_rd = (signed_rs1 * value_rs2) >> 32;
			*(int64_t *) &value_rd = signed_rd;
			//bit masking
			if (value_rd >= 0x80000000) {
				value_rd = value_rd | 0xffffffff80000000; // negative result
			} else {
				value_rd = value_rd & 0x000000007fffffff; // positive result
			}
		} else if (array[0] == 53) { // mulhu
			uint64_t register_rd = array[2];
			 
			address_rd = register_rd;

			
			value_rd = (value_rs1 * value_rs2) >> 32;
			//bit masking
			if (value_rd >= 0x80000000) {
				value_rd = value_rd | 0xffffffff80000000; // negative result
			} else {
				value_rd = value_rd & 0x000000007fffffff; // positive result
			}
		} else if(array[0] == 54) { //mulw
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
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
		} else if (array[0] == 55) { // div
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			// convert unsigned to signed
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rs2 = *(int64_t *) &value_rs2;
			
			signed_rd = signed_rs1 / signed_rs2;
			*(int64_t *) &value_rd = signed_rd;
		} else if (array[0] == 56) { // divu
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			value_rd = value_rs1 / value_rs2;
		}else if (array[0] == 57) { // rem
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			// convert unsigned to signed
			signed_rs1 = *(int64_t *) &value_rs1;
			signed_rs2 = *(int64_t *) &value_rs2;
			
			signed_rd = signed_rs1 % signed_rs2;
			*(int64_t *) &value_rd = signed_rd;
		}else if (array[0] == 58) { // remu
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			value_rd = value_rs1 % value_rs2;
		}else if (array[0] == 59) { // divw
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
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
		}else if (array[0] == 60) { // divuw
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
			value_rd = value_rs1 / value_rs2;
			//bit mask.
			value_rd = value_rd & 0xffffffff;
			if (value_rd >= 0x80000000) {
				value_rd = value_rd | 0xffffffff80000000; // negative result
			} else {
				value_rd = value_rd & 0x000000007fffffff; // positive result
			}
		}else if (array[0] == 61) { // remw
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
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
		}else if (array[0] == 62) { // remuw
			uint64_t register_rd = array[2];
			
			address_rd = register_rd;
			
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
	
	
	// for debug, need to delete
	//int b = current_stage_x_register -> instruction;
	//if(current_stage_x_register -> instruction!=0){
	//	int *a = current_stage_x_register -> array;
	//	int haha = *(a);
	//	printf("hahahhahahhaahahahhahah = %d\n", haha);
	//}
	// debug
	//printf("x = %d\n", value_rd);
	
	// update next stage register
	new_m_reg -> instruction = current_stage_x_register -> instruction;
	new_m_reg -> value_rd = value_rd;
	new_m_reg -> address_rd = address_rd;
	new_m_reg -> address_memory = address_memory;
	new_m_reg -> byte_memory = byte_memory;
	new_m_reg -> address_target = address_target;
}

extern void stage_memory (struct stage_reg_w *new_w_reg) {
	printf("Memory stage, inst=0x%.8lX.\n", current_stage_m_register->instruction);
	new_w_reg -> value_rd = current_stage_m_register -> value_rd;
	//debug
	//printf("m = %d, w = %d\n",current_stage_m_register->address_rd,current_stage_w_register->address_rd );
	if(current_stage_m_register->instruction!=0){
		uint64_t value_rd = 0;
		value_rd = current_stage_m_register -> value_rd;
		uint64_t address_rd = current_stage_m_register -> address_rd;
		uint64_t address_memory = current_stage_m_register -> address_memory;
		address_memory = address_memory + 0x4000;
		uint64_t byte_memory = current_stage_m_register -> byte_memory;
		uint64_t load_signed = current_stage_m_register -> load_signed;
		
		// write to memory 
		if(current_stage_m_register -> isStore == 1){
			memory_write(address_memory, value_rd, byte_memory);	
			//debug
			//printf("stAD = 0x%.8lX, stVa = 0x%.8lX, stBy = %d\n",address_memory, value_rd, byte_memory);
		}
		// WM forward happen here.
		if(current_stage_w_register->address_rd == current_stage_m_register->address_rd){
			if(current_stage_m_register -> isStore == 1){
				value_rd = current_stage_w_register -> value_rd;
				memory_write(address_memory, value_rd, byte_memory);	
				//debug
				//printf("stAD = 0x%.8lX, stVa = 0x%.8lX, stBy = %d\n",address_memory, value_rd, byte_memory);
			}
		}
		// read from memory 
		if(current_stage_m_register -> isLoad == 1){
			memory_read(address_memory, &value_rd, byte_memory);
			// if the instruction is lb, lh, lw, we need to sign-extend it.
			if(load_signed == 1){
				value_rd = sign_extend(value_rd, byte_memory);
			}
			new_w_reg -> value_rd = value_rd;
			//debug
			//value_rd = 0x123456789abcdef1;
			//memory_write(1000, value_rd, 4);
			//printf("loAD = %d, loVa = 0x%.8lX, loBy = %d\n", 1000, value_rd, 1);
			//value_rd = 0;
			//memory_read(1000, &value_rd, 4);
			//printf("loAD = 0x%.8lX, loVa = 0x%.8lX, loBy = %d\n", address_memory, value_rd, byte_memory);
		}
		// this is not a prediction branch, just branch
		if(current_stage_m_register -> isBranch == 1){
			//debug
			//printf("adMeo = 0x%.8lX\n", current_stage_m_register -> address_target);
			
			set_pc(current_stage_m_register -> address_target);
			
		}
		// this is a prediction branch
		if(current_stage_m_register -> isPrediction == 1){
			// if prediction is wrong, let pc be the pc after branch pc
			// if prediction is right, nothing happen.
			if(current_stage_m_register -> predictionWrong == 1){
				set_pc(current_stage_m_register -> new_pc + 0x4);
			}
		}
		//debug
		//printf("adMeo = %d, adRd = %d\n", address_memory, address_rd);
	}
	
	// update next stage register
	new_w_reg -> instruction = current_stage_m_register -> instruction;
	new_w_reg -> address_rd = current_stage_m_register -> address_rd;
}

extern void stage_writeback (void) {
	printf("Writeback stage, inst=0x%.8lX.\n", current_stage_w_register->instruction);
	if(current_stage_w_register->instruction!=0){
		register_write(current_stage_w_register -> address_rd, current_stage_w_register -> value_rd);
	}
}