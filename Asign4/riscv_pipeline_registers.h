/*
 * External routines for use in CMPE 110, Fall 2018
 *
 * (c) 2018 Ethan L. Miller
 * Redistribution not permitted without explicit permission from the copyright holder.
 *
 */

/*
 * IMPORTANT: rename this file to riscv_pipeline_registers.h
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

/*
 * These pipeline stage registers are loaded at the end of the cycle with whatever
 * values were filled in by the relevant pipeline stage.
 *
 * Add fields to these stage registers for whatever values you'd like to have passed from
 * one stage to the next.  You may have as many values as you like in each stage.  However,
 * this is the ONLY information that may be passed from one stage to stages in the next
 * cycle.
 */


struct stage_reg_d {
    uint64_t    new_pc;
    uint32_t    instruction;
	uint64_t	isPrediction;
	uint64_t	predictionWrong;
	uint64_t	isICacheStall;
	uint64_t	i_cacheIsReady;
};

struct stage_reg_x {
    uint64_t    new_pc;
    uint32_t    instruction;
	
	uint64_t    *array;
	uint64_t	isPrediction;
	uint64_t	predictionWrong;
};

struct stage_reg_m {
	uint64_t	new_pc;
    uint32_t    instruction;
	uint64_t	value_rd;
	uint64_t	address_rd;
	uint64_t	address_memory;
	uint64_t	byte_memory;
	uint64_t	address_target;
	uint64_t	isStore;
	uint64_t	isLoad;
	uint64_t	load_signed;
	uint64_t	isBranch;
	uint64_t	branchAndFlush;
	uint64_t	predictionAndFlush;
	uint64_t	isPrediction;
	uint64_t	predictionWrong;
};

struct stage_reg_w {
    uint32_t    instruction;
	uint64_t	value_rd;
	uint64_t	address_rd;
};
