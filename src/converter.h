#pragma once

#include "engine.h"
#include <stdint.h>

ChessBoardState fenToBitboard(const char *fen);
void bitboardToFen(const ChessBoardState *board, char *fen);
