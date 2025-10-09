#include "engine.h"
#include <string.h>
#include <stdlib.h>

static const char piece_ascii[2][7] = {{' ', 'P', 'R', 'N', 'B', 'Q', 'K'},
                                       {' ', 'p', 'r', 'n', 'b', 'q', 'k'}};

void init_chess_board(ChessBoardState *board) {
  memset(board, 0, sizeof(ChessBoardState));

  board->pieces[WHITE][PIECE_PAWN] = 0x000000000000FF00ULL;
  board->pieces[WHITE][PIECE_ROOK] = 0x0000000000000081ULL;
  board->pieces[WHITE][PIECE_KNIGHT] = 0x0000000000000042ULL;
  board->pieces[WHITE][PIECE_BISHOP] = 0x0000000000000024ULL;
  board->pieces[WHITE][PIECE_QUEEN] = 0x0000000000000008ULL;
  board->pieces[WHITE][PIECE_KING] = 0x0000000000000010ULL;

  board->pieces[BLACK][PIECE_PAWN] = 0x00FF000000000000ULL;
  board->pieces[BLACK][PIECE_ROOK] = 0x8100000000000000ULL;
  board->pieces[BLACK][PIECE_KNIGHT] = 0x4200000000000000ULL;
  board->pieces[BLACK][PIECE_BISHOP] = 0x2400000000000000ULL;
  board->pieces[BLACK][PIECE_QUEEN] = 0x0800000000000000ULL;
  board->pieces[BLACK][PIECE_KING] = 0x1000000000000000ULL;

  board->to_move = WHITE;
  board->castle_rights = 0xF; // All castling rights initially available
  board->en_passant_square = -1;
  board->halfmove_clock = 0;
  board->fullmove_number = 1;

  update_board_state(board);
}

void update_board_state(ChessBoardState *board) {
  board->white_pieces = 0;
  board->black_pieces = 0;
  
  for (int piece = PIECE_PAWN; piece <= PIECE_KING; piece++) {
    board->white_pieces |= board->pieces[WHITE][piece];
    board->black_pieces |= board->pieces[BLACK][piece];
  }
  
  board->occupied = board->white_pieces | board->black_pieces;

  for (int square = 0; square < 64; square++) {
    board->square_piece[square] = ' ';
    for (int color = 0; color < 2; color++) {
      for (int piece = 1; piece <= 6; piece++) {
        if (board->pieces[color][piece] & (1ULL << square)) {
          board->square_piece[square] = piece_ascii[color][piece];
        }
      }
    }
  }
}

// Utility functions for square manipulation
int square_to_rank(int square) {
  return square / 8;
}

int square_to_file(int square) {
  return square % 8;
}

int rank_file_to_square(int rank, int file) {
  return rank * 8 + file;
}

// Attack pattern generation
uint64_t get_knight_attacks(int square) {
  uint64_t attacks = 0;
  int rank = square_to_rank(square);
  int file = square_to_file(square);
  
  int knight_moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                           {1, -2}, {1, 2}, {2, -1}, {2, 1}};
  
  for (int i = 0; i < 8; i++) {
    int new_rank = rank + knight_moves[i][0];
    int new_file = file + knight_moves[i][1];
    
    if (new_rank >= 0 && new_rank < 8 && new_file >= 0 && new_file < 8) {
      attacks |= (1ULL << rank_file_to_square(new_rank, new_file));
    }
  }
  
  return attacks;
}

uint64_t get_king_attacks(int square) {
  uint64_t attacks = 0;
  int rank = square_to_rank(square);
  int file = square_to_file(square);
  
  for (int dr = -1; dr <= 1; dr++) {
    for (int df = -1; df <= 1; df++) {
      if (dr == 0 && df == 0) continue;
      
      int new_rank = rank + dr;
      int new_file = file + df;
      
      if (new_rank >= 0 && new_rank < 8 && new_file >= 0 && new_file < 8) {
        attacks |= (1ULL << rank_file_to_square(new_rank, new_file));
      }
    }
  }
  
  return attacks;
}

uint64_t get_pawn_attacks(int square, int color) {
  uint64_t attacks = 0;
  int rank = square_to_rank(square);
  int file = square_to_file(square);
  
  int direction = (color == WHITE) ? 1 : -1;
  int new_rank = rank + direction;
  
  if (new_rank >= 0 && new_rank < 8) {
    if (file > 0) {
      attacks |= (1ULL << rank_file_to_square(new_rank, file - 1));
    }
    if (file < 7) {
      attacks |= (1ULL << rank_file_to_square(new_rank, file + 1));
    }
  }
  
  return attacks;
}

uint64_t get_rook_attacks(int square, uint64_t occupied) {
  uint64_t attacks = 0;
  int rank = square_to_rank(square);
  int file = square_to_file(square);
  
  // Horizontal attacks
  for (int f = file + 1; f < 8; f++) {
    int sq = rank_file_to_square(rank, f);
    attacks |= (1ULL << sq);
    if (occupied & (1ULL << sq)) break;
  }
  for (int f = file - 1; f >= 0; f--) {
    int sq = rank_file_to_square(rank, f);
    attacks |= (1ULL << sq);
    if (occupied & (1ULL << sq)) break;
  }
  
  // Vertical attacks
  for (int r = rank + 1; r < 8; r++) {
    int sq = rank_file_to_square(r, file);
    attacks |= (1ULL << sq);
    if (occupied & (1ULL << sq)) break;
  }
  for (int r = rank - 1; r >= 0; r--) {
    int sq = rank_file_to_square(r, file);
    attacks |= (1ULL << sq);
    if (occupied & (1ULL << sq)) break;
  }
  
  return attacks;
}

uint64_t get_bishop_attacks(int square, uint64_t occupied) {
  uint64_t attacks = 0;
  int rank = square_to_rank(square);
  int file = square_to_file(square);
  
  // Diagonal attacks
  int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  
  for (int d = 0; d < 4; d++) {
    int dr = directions[d][0];
    int df = directions[d][1];
    
    for (int i = 1; i < 8; i++) {
      int new_rank = rank + i * dr;
      int new_file = file + i * df;
      
      if (new_rank < 0 || new_rank >= 8 || new_file < 0 || new_file >= 8) break;
      
      int sq = rank_file_to_square(new_rank, new_file);
      attacks |= (1ULL << sq);
      if (occupied & (1ULL << sq)) break;
    }
  }
  
  return attacks;
}

uint64_t get_queen_attacks(int square, uint64_t occupied) {
  return get_rook_attacks(square, occupied) | get_bishop_attacks(square, occupied);
}

uint64_t get_pawn_moves(int square, int color, uint64_t occupied) {
  uint64_t moves = 0;
  int rank = square_to_rank(square);
  int file = square_to_file(square);
  
  int direction = (color == WHITE) ? 1 : -1;
  int start_rank = (color == WHITE) ? 1 : 6;
  
  // Single push
  int new_rank = rank + direction;
  if (new_rank >= 0 && new_rank < 8) {
    int sq = rank_file_to_square(new_rank, file);
    if (!(occupied & (1ULL << sq))) {
      moves |= (1ULL << sq);
      
      // Double push from starting position
      if (rank == start_rank) {
        new_rank += direction;
        if (new_rank >= 0 && new_rank < 8) {
          sq = rank_file_to_square(new_rank, file);
          if (!(occupied & (1ULL << sq))) {
            moves |= (1ULL << sq);
          }
        }
      }
    }
  }
  
  return moves;
}

int is_square_attacked(ChessBoardState *board, int square, int by_color) {
  // Check pawn attacks
  uint64_t pawn_attacks = get_pawn_attacks(square, 1 - by_color);
  if (pawn_attacks & board->pieces[by_color][PIECE_PAWN]) return 1;
  
  // Check knight attacks
  uint64_t knight_attacks = get_knight_attacks(square);
  if (knight_attacks & board->pieces[by_color][PIECE_KNIGHT]) return 1;
  
  // Check bishop/queen diagonal attacks
  uint64_t bishop_attacks = get_bishop_attacks(square, board->occupied);
  if (bishop_attacks & (board->pieces[by_color][PIECE_BISHOP] | board->pieces[by_color][PIECE_QUEEN])) return 1;
  
  // Check rook/queen straight attacks
  uint64_t rook_attacks = get_rook_attacks(square, board->occupied);
  if (rook_attacks & (board->pieces[by_color][PIECE_ROOK] | board->pieces[by_color][PIECE_QUEEN])) return 1;
  
  // Check king attacks
  uint64_t king_attacks = get_king_attacks(square);
  if (king_attacks & board->pieces[by_color][PIECE_KING]) return 1;
  
  return 0;
}

void add_move(MoveList *move_list, int from, int to, PieceType piece, PieceType captured) {
  if (move_list->count < 256) {
    Move *move = &move_list->moves[move_list->count++];
    move->from = from;
    move->to = to;
    move->piece = piece;
    move->captured = captured;
    move->is_castle = 0;
    move->is_en_passant = 0;
    move->promotion = PIECE_NONE;
  }
}

void add_promotion_moves(MoveList *move_list, int from, int to, PieceType captured) {
  PieceType promotions[] = {PIECE_QUEEN, PIECE_ROOK, PIECE_BISHOP, PIECE_KNIGHT};
  for (int i = 0; i < 4; i++) {
    if (move_list->count < 256) {
      Move *move = &move_list->moves[move_list->count++];
      move->from = from;
      move->to = to;
      move->piece = PIECE_PAWN;
      move->captured = captured;
      move->is_castle = 0;
      move->is_en_passant = 0;
      move->promotion = promotions[i];
    }
  }
}

void generate_piece_moves(ChessBoardState *board, MoveList *move_list, int color, PieceType piece_type) {
  uint64_t pieces = board->pieces[color][piece_type];
  uint64_t enemy_pieces = (color == WHITE) ? board->black_pieces : board->white_pieces;
  uint64_t own_pieces = (color == WHITE) ? board->white_pieces : board->black_pieces;
  
  while (pieces) {
    int from = __builtin_ctzll(pieces);
    pieces &= pieces - 1; // Clear the least significant bit
    
    uint64_t attacks = 0;
    
    switch (piece_type) {
      case PIECE_PAWN: {
        // Pawn moves
        uint64_t moves = get_pawn_moves(from, color, board->occupied);
        while (moves) {
          int to = __builtin_ctzll(moves);
          moves &= moves - 1;
          
          int to_rank = square_to_rank(to);
          if ((color == WHITE && to_rank == 7) || (color == BLACK && to_rank == 0)) {
            add_promotion_moves(move_list, from, to, PIECE_NONE);
          } else {
            add_move(move_list, from, to, PIECE_PAWN, PIECE_NONE);
          }
        }
        
        // Pawn captures
        uint64_t pawn_attacks = get_pawn_attacks(from, color);
        uint64_t captures = pawn_attacks & enemy_pieces;
        while (captures) {
          int to = __builtin_ctzll(captures);
          captures &= captures - 1;
          
          PieceType captured = PIECE_NONE;
          for (int p = PIECE_PAWN; p <= PIECE_KING; p++) {
            if (board->pieces[1-color][p] & (1ULL << to)) {
              captured = p;
              break;
            }
          }
          
          int to_rank = square_to_rank(to);
          if ((color == WHITE && to_rank == 7) || (color == BLACK && to_rank == 0)) {
            add_promotion_moves(move_list, from, to, captured);
          } else {
            add_move(move_list, from, to, PIECE_PAWN, captured);
          }
        }
        
        // En passant
        if (board->en_passant_square != -1) {
          uint64_t ep_attacks = pawn_attacks & (1ULL << board->en_passant_square);
          if (ep_attacks) {
            Move *move = &move_list->moves[move_list->count++];
            move->from = from;
            move->to = board->en_passant_square;
            move->piece = PIECE_PAWN;
            move->captured = PIECE_PAWN;
            move->is_castle = 0;
            move->is_en_passant = 1;
            move->promotion = PIECE_NONE;
          }
        }
        break;
      }
      case PIECE_KNIGHT:
        attacks = get_knight_attacks(from);
        break;
      case PIECE_BISHOP:
        attacks = get_bishop_attacks(from, board->occupied);
        break;
      case PIECE_ROOK:
        attacks = get_rook_attacks(from, board->occupied);
        break;
      case PIECE_QUEEN:
        attacks = get_queen_attacks(from, board->occupied);
        break;
      case PIECE_KING:
        attacks = get_king_attacks(from);
        break;
      default:
        break;
    }
    
    if (piece_type != PIECE_PAWN) {
      attacks &= ~own_pieces; // Can't capture own pieces
      
      while (attacks) {
        int to = __builtin_ctzll(attacks);
        attacks &= attacks - 1;
        
        PieceType captured = PIECE_NONE;
        if (enemy_pieces & (1ULL << to)) {
          for (int p = PIECE_PAWN; p <= PIECE_KING; p++) {
            if (board->pieces[1-color][p] & (1ULL << to)) {
              captured = p;
              break;
            }
          }
        }
        
        add_move(move_list, from, to, piece_type, captured);
      }
    }
  }
}

void generate_castling_moves(ChessBoardState *board, MoveList *move_list) {
  int color = board->to_move;
  int king_square = __builtin_ctzll(board->pieces[color][PIECE_KING]);
  
  // Kingside castling
  int kingside_bit = (color == WHITE) ? 0 : 2;
  if (board->castle_rights & (1 << kingside_bit)) {
    int rook_square = (color == WHITE) ? H1 : H8;
    int king_to = (color == WHITE) ? G1 : G8;
    int rook_to = (color == WHITE) ? F1 : F8;
    
    // Check if squares between king and rook are empty
    uint64_t between_mask = (color == WHITE) ? 0x0000000000000060ULL : 0x6000000000000000ULL;
    if (!(board->occupied & between_mask)) {
      // Check if king and squares it passes through are not attacked
      if (!is_square_attacked(board, king_square, 1 - color) &&
          !is_square_attacked(board, king_to - 1, 1 - color) &&
          !is_square_attacked(board, king_to, 1 - color)) {
        Move *move = &move_list->moves[move_list->count++];
        move->from = king_square;
        move->to = king_to;
        move->piece = PIECE_KING;
        move->captured = PIECE_NONE;
        move->is_castle = 1;
        move->is_en_passant = 0;
        move->promotion = PIECE_NONE;
      }
    }
  }
  
  // Queenside castling
  int queenside_bit = (color == WHITE) ? 1 : 3;
  if (board->castle_rights & (1 << queenside_bit)) {
    int rook_square = (color == WHITE) ? A1 : A8;
    int king_to = (color == WHITE) ? C1 : C8;
    int rook_to = (color == WHITE) ? D1 : D8;
    
    // Check if squares between king and rook are empty
    uint64_t between_mask = (color == WHITE) ? 0x000000000000000EULL : 0x0E00000000000000ULL;
    if (!(board->occupied & between_mask)) {
      // Check if king and squares it passes through are not attacked
      if (!is_square_attacked(board, king_square, 1 - color) &&
          !is_square_attacked(board, king_to + 1, 1 - color) &&
          !is_square_attacked(board, king_to, 1 - color)) {
        Move *move = &move_list->moves[move_list->count++];
        move->from = king_square;
        move->to = king_to;
        move->piece = PIECE_KING;
        move->captured = PIECE_NONE;
        move->is_castle = 1;
        move->is_en_passant = 0;
        move->promotion = PIECE_NONE;
      }
    }
  }
}

void generate_moves(ChessBoardState *board, MoveList *move_list) {
  move_list->count = 0;
  
  int color = board->to_move;
  
  // Generate moves for each piece type
  for (PieceType piece = PIECE_PAWN; piece <= PIECE_KING; piece++) {
    generate_piece_moves(board, move_list, color, piece);
  }
  
  // Generate castling moves
  generate_castling_moves(board, move_list);
}

int make_move(ChessBoardState *board, Move move) {
  int color = board->to_move;
  int enemy_color = 1 - color;
  
  // Store state for potential unmake
  uint64_t old_occupied = board->occupied;
  uint64_t old_white_pieces = board->white_pieces;
  uint64_t old_black_pieces = board->black_pieces;
  int old_castle_rights = board->castle_rights;
  int old_en_passant = board->en_passant_square;
  
  // Remove piece from source square
  board->pieces[color][move.piece] &= ~(1ULL << move.from);
  
  // Handle captures
  if (move.captured != PIECE_NONE) {
    board->pieces[enemy_color][move.captured] &= ~(1ULL << move.to);
  }
  
  // Handle en passant capture
  if (move.is_en_passant) {
    int captured_pawn_square = (color == WHITE) ? move.to - 8 : move.to + 8;
    board->pieces[enemy_color][PIECE_PAWN] &= ~(1ULL << captured_pawn_square);
  }
  
  // Place piece on destination square
  PieceType piece_to_place = (move.promotion != PIECE_NONE) ? move.promotion : move.piece;
  board->pieces[color][piece_to_place] |= (1ULL << move.to);
  
  // Handle castling
  if (move.is_castle) {
    if (move.to > move.from) { // Kingside
      int rook_from = (color == WHITE) ? H1 : H8;
      int rook_to = (color == WHITE) ? F1 : F8;
      board->pieces[color][PIECE_ROOK] &= ~(1ULL << rook_from);
      board->pieces[color][PIECE_ROOK] |= (1ULL << rook_to);
    } else { // Queenside
      int rook_from = (color == WHITE) ? A1 : A8;
      int rook_to = (color == WHITE) ? D1 : D8;
      board->pieces[color][PIECE_ROOK] &= ~(1ULL << rook_from);
      board->pieces[color][PIECE_ROOK] |= (1ULL << rook_to);
    }
  }
  
  // Update castling rights
  if (move.piece == PIECE_KING) {
    if (color == WHITE) {
      board->castle_rights &= ~0x3; // Clear white castling rights
    } else {
      board->castle_rights &= ~0xC; // Clear black castling rights
    }
  }
  
  if (move.piece == PIECE_ROOK) {
    if (move.from == A1) board->castle_rights &= ~0x2;
    else if (move.from == H1) board->castle_rights &= ~0x1;
    else if (move.from == A8) board->castle_rights &= ~0x8;
    else if (move.from == H8) board->castle_rights &= ~0x4;
  }
  
  if (move.captured == PIECE_ROOK) {
    if (move.to == A1) board->castle_rights &= ~0x2;
    else if (move.to == H1) board->castle_rights &= ~0x1;
    else if (move.to == A8) board->castle_rights &= ~0x8;
    else if (move.to == H8) board->castle_rights &= ~0x4;
  }
  
  // Update en passant square
  board->en_passant_square = -1;
  if (move.piece == PIECE_PAWN && abs(move.to - move.from) == 16) {
    board->en_passant_square = (move.from + move.to) / 2;
  }
  
  // Update move counters
  if (move.captured != PIECE_NONE || move.piece == PIECE_PAWN) {
    board->halfmove_clock = 0;
  } else {
    board->halfmove_clock++;
  }
  
  if (color == BLACK) {
    board->fullmove_number++;
  }
  
  // Switch turn
  board->to_move = enemy_color;
  
  // Update board state
  update_board_state(board);
  
  // Check if move puts own king in check (illegal move)
  int king_square = __builtin_ctzll(board->pieces[color][PIECE_KING]);
  if (is_square_attacked(board, king_square, enemy_color)) {
    // Restore state
    board->occupied = old_occupied;
    board->white_pieces = old_white_pieces;
    board->black_pieces = old_black_pieces;
    board->castle_rights = old_castle_rights;
    board->en_passant_square = old_en_passant;
    board->to_move = color;
    return 0; // Illegal move
  }
  
  return 1; // Legal move
}
