#pragma once
#include "gb.h"
#include "helper.h"
#include "mem.h"
#include <stdint.h>

#define Z_FLAG_LOC 7
#define N_FLAG_LOC 6
#define H_FLAG_LOC 5
#define C_FLAG_LOC 4

#define Z_FLAG (1 << Z_FLAG_LOC)
#define N_FLAG (1 << N_FLAG_LOC)
#define H_FLAG (1 << H_FLAG_LOC)
#define C_FLAG (1 << C_FLAG_LOC)

int run_cpu(GB_CPU *cpu, GB_Memory *mem);
