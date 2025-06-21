#pragma once

#include "common.h"
#include <stdint.h>

boardState fenToBitboard(const char *fen);
void bitboardToFen(const boardState *board, char *fen);
