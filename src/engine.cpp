#include "engine.hpp"
#include <cctype>
#include <cstdint>
#include <ncurses.h>
#include <sstream>

ClessEngine::ClessEngine(const std::string &fen) : bitboards{}, lookup_table{} {
  std::istringstream fen_stream(fen);
  std::string piece_placement, active_color, castling, en_passant, halfmove_str,
      fullmove_str;

  fen_stream >> piece_placement >> active_color >> castling >> en_passant >>
      halfmove_str >> fullmove_str;

  // Piece placement
  int rank = 7, file = 0;
  for (char piece_char : piece_placement) {
    if (piece_char == '/') {
      rank--;
      file = 0;
      continue;
    }

    if (isdigit(piece_char)) {
      file += piece_char - '0';
      continue;
    }

    PieceColor color = isupper(piece_char) ? WHITE : BLACK;
    PieceType type;

    switch (toupper(piece_char)) {
      case 'P': type = PIECE_PAWN; break;
      case 'R': type = PIECE_ROOK; break;
      case 'N': type = PIECE_KNIGHT; break;
      case 'B': type = PIECE_BISHOP; break;
      case 'Q': type = PIECE_QUEEN; break;
      case 'K': type = PIECE_KING; break;
      default: type = PIECE_NONE; break;
    }

    if (type != PIECE_NONE)
      add_piece(color, type, static_cast<Square>(rank * 8 + file));

    file++;
  }

  // Active color
  to_move = (active_color == "w") ? WHITE : BLACK;

  // Castling rights (WIP)
  if (castling != "-") {
    // TODO: Implement castling rights parsing
  }

  // En passant square
  if (en_passant != "-") {
    int file_idx = en_passant[0] - 'a';
    int rank_idx = en_passant[1] - '1'; // -'1' for 0-based index
    int ep_square = rank_idx * 8 + file_idx;
    en_passant_square = static_cast<Square>(ep_square);
  } else {
    en_passant_square = std::nullopt;
  }

  // Halfmove clock and Fullmove counter
  halfmove_clock = halfmove_str.empty() ? 0 : std::stoi(halfmove_str);
  fullmove_counter = fullmove_str.empty() ? 1 : std::stoi(fullmove_str);
}

std::string ClessEngine::get_fen() const {
  std::string fen;

  // Piece placement
  for (int rank = 7; rank >= 0; rank--) {
    int empty_count = 0;

    for (int file = 0; file < 8; file++) {
      Square square = static_cast<Square>(rank * 8 + file);
      uint8_t encoded_piece = lookup_table[square];

      if (encoded_piece == 0) {
        empty_count++;
      } else {
        if (empty_count > 0) {
          fen += std::to_string(empty_count);
          empty_count = 0;
        }

        PieceColor color = decode_color(encoded_piece);
        PieceType type = decode_type(encoded_piece);

        char piece_char;
        switch (type) {
          case PIECE_PAWN: piece_char = 'P'; break;
          case PIECE_ROOK: piece_char = 'R'; break;
          case PIECE_KNIGHT: piece_char = 'N'; break;
          case PIECE_BISHOP: piece_char = 'B'; break;
          case PIECE_QUEEN: piece_char = 'Q'; break;
          case PIECE_KING: piece_char = 'K'; break;
          default: continue; // Skip invalid pieces
        }

        if (color == BLACK) { piece_char = tolower(piece_char); }

        fen += piece_char;
      }
    }

    if (empty_count > 0) { fen += std::to_string(empty_count); }

    if (rank > 0) { fen += '/'; }
  }

  // Active color
  fen += ' ';
  fen += (to_move == WHITE) ? 'w' : 'b';

  // TODO: Implement castling rights
  fen += " -";

  // En passant square
  fen += ' ';
  if (en_passant_square.has_value()) {
    Square ep_square = en_passant_square.value();
    int file = ep_square % 8;
    int rank = ep_square / 8;
    fen += static_cast<char>('a' + file);
    fen += static_cast<char>('1' + rank);
  } else {
    fen += '-';
  }

  // Halfmove clock
  fen += ' ';
  fen += std::to_string(halfmove_clock);

  // Fullmove counter
  fen += ' ';
  fen += std::to_string(fullmove_counter);

  return fen;
}

Piece ClessEngine::get_piece_at(Square square) {
  uint8_t encoded_piece = lookup_table[square];
  if (encoded_piece == 0) return Piece{WHITE, PIECE_NONE};

  PieceColor color = decode_color(encoded_piece);
  PieceType type = decode_type(encoded_piece);
  return Piece{color, type};
}

bool ClessEngine::move_piece(Square from, Square to) {
  uint8_t encoded_piece = lookup_table[from];
  if (encoded_piece == 0) return false;

  PieceColor piece_color = decode_color(encoded_piece);
  PieceType piece_type = decode_type(encoded_piece);

  if (!validate_move(piece_color, piece_type)) return false;
  if (piece_type == PIECE_PAWN) halfmove_clock = 0;

  remove_piece(piece_color, piece_type, from);
  add_piece(piece_color, piece_type, to);

  pass_turn();
  return true;
}

bool ClessEngine::validate_move(PieceColor piece_color, PieceType piece_type) {
  // No piece at the source square
  if (piece_type == PIECE_NONE) return false;

  // Not the player's turn
  if (piece_color != to_move) return false;

  return true;
}

void ClessEngine::add_piece(PieceColor color, PieceType piece, Square square) {
  uint64_t square_bit = square_to_bit(square);
  bitboards[color][piece] |= square_bit;
  bitboards[color][PIECE_NONE] &= ~square_bit;

  lookup_table[square] = encode_piece(color, piece);
}

void ClessEngine::remove_piece(PieceColor color, PieceType piece,
                               Square square) {
  uint64_t square_bit = square_to_bit(square);
  bitboards[color][piece] &= ~square_bit;
  bitboards[color][PIECE_NONE] |= square_bit;

  lookup_table[square] = 0;
}

void ClessEngine::pass_turn() {
  bool black_played = to_move == BLACK;
  if (black_played) fullmove_counter++;

  to_move = black_played ? WHITE : BLACK;
}
