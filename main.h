#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// 
// For internal use only!
// 
#ifdef _DEBUG
#define DP(x, ...) printf(x, __VA_ARGS__)
#else
#define DP(x, ...) (void)0
#endif

#define SKIP_JUNK_IN_LINE(line) if (line[0] == '\n') continue;

#define NEW(t) (calloc(1, sizeof(t)))
#define DEL(p) (free(p))