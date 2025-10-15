#include "position.hpp"
#include "chess_types.hpp"
#include <sstream>

Position::Position(const std::string &fen) : bitboards{}, lookup_table{} {
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
      add_piece(color, type, indexes_to_square(rank, file));

    file++;
  }

  // Active color
  to_move = (active_color == "w") ? WHITE : BLACK;

  if (castling == "-") {
    castling_rights = 0;
  } else {
    for (char c : castling) {
      switch (c) {
        case 'K': castling_rights |= WHITE_CASTLE_KING; break;
        case 'Q': castling_rights |= WHITE_CASTLE_QUEEN; break;
        case 'k': castling_rights |= BLACK_CASTLE_KING; break;
        case 'q': castling_rights |= BLACK_CASTLE_QUEEN; break;
      }
    }
  }

  // En passant square
  if (en_passant == "-") {
    en_passant_square = std::nullopt;
  } else {
    int file_idx = en_passant[0] - 'a';
    int rank_idx = en_passant[1] - '1'; // -'1' for 0-based index
    en_passant_square = indexes_to_square(rank_idx, file_idx);
  }

  // Halfmove clock and Fullmove counter
  halfmove_clock = halfmove_str.empty() ? 0 : std::stoi(halfmove_str);
  fullmove_counter = fullmove_str.empty() ? 1 : std::stoi(fullmove_str);
}

std::string Position::get_fen() const {
  std::string fen;

  // Piece placement
  for (int rank = 7; rank >= 0; rank--) {
    int empty_count = 0;

    for (int file = 0; file < 8; file++) {
      Square square = indexes_to_square(rank, file);
      uint8_t encoded_piece = lookup_table[square];

      if (encoded_piece == 0) {
        empty_count++;
        continue;
      }

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
        default: continue;
      }

      if (color == BLACK) { piece_char = tolower(piece_char); }

      fen += piece_char;
    }
    if (empty_count > 0) { fen += std::to_string(empty_count); }

    if (rank > 0) { fen += '/'; }
  }

  // Active color
  fen += ' ';
  fen += (to_move == WHITE) ? 'w' : 'b';

  // Castling rights
  fen += ' ';
  if (castling_rights == 0) {
    fen += "-";
  } else {
    if (castling_rights & WHITE_CASTLE_KING) fen += "K";
    if (castling_rights & WHITE_CASTLE_QUEEN) fen += "Q";
    if (castling_rights & BLACK_CASTLE_KING) fen += "k";
    if (castling_rights & BLACK_CASTLE_QUEEN) fen += "q";
  }

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

Piece Position::get_piece_at(Square square) {
  uint8_t encoded_piece = lookup_table[square];
  if (encoded_piece == 0) return Piece{WHITE, PIECE_NONE};

  PieceColor color = decode_color(encoded_piece);
  PieceType type = decode_type(encoded_piece);
  return Piece{color, type};
}

Square Position::get_captured_square(const Move &move) const {
  if (move.is_en_passant()) {
    return static_cast<Square>((move.from + move.to) / 2);
  }

  return move.to;
}

void Position::make_move(const Move &move) {
  uint8_t encoded_piece = lookup_table[move.from];
  Piece piece = decode_piece(encoded_piece);
  Square captured_square = get_captured_square(move);

  if (move.is_capture()) {
    uint8_t encoded_captured_piece = lookup_table[captured_square];
    Piece captured_piece = decode_piece(encoded_captured_piece);
    remove_piece(captured_piece.color, captured_piece.type, captured_square);

    set_undo_info(move.from, move.to, captured_square, encoded_piece, encoded_captured_piece);
  } else {
    set_undo_info(move.from, move.to, captured_square, encoded_piece, 0);
  }

  en_passant_square.reset();
  if (piece.type == PIECE_PAWN) {
    halfmove_clock = 0;
    if (abs(square_rank(move.to) - square_rank(move.from)) == 2) {
      en_passant_square = static_cast<Square>((move.from + move.to) / 2);
    }
  }

  remove_piece(piece.color, piece.type, move.from);
  add_piece(piece.color, piece.type, move.to);
  pass_turn();
}

void Position::undo_move() {
  Square from = undo_info.from_square;
  Square to = undo_info.to_square;
  Square captured_square = undo_info.captured_square;

  Piece moved_piece = decode_piece(undo_info.moved_piece_encoded);
  Piece captured_piece = decode_piece(undo_info.captured_piece_encoded);

  remove_piece(moved_piece.color, moved_piece.type, to);
  add_piece(moved_piece.color, moved_piece.type, from);

  if (captured_piece.type != PIECE_NONE)
    add_piece(captured_piece.color, captured_piece.type, captured_square);

  castling_rights = undo_info.castling_rights;
  en_passant_square = undo_info.en_passant_square;
  halfmove_clock = undo_info.halfmove_clock;

  to_move = opposite_color(to_move);
}

void Position::add_piece(PieceColor color, PieceType piece, Square square) {
  uint64_t square_bit = square_to_bit(square);
  bitboards[bitboard_index(color, piece)] |= square_bit;
  occupancy[color] |= square_bit;
  occupancy[ANY] |= square_bit;

  lookup_table[square] = encode_piece(color, piece);
}

void Position::remove_piece(PieceColor color, PieceType piece, Square square) {
  uint64_t square_bit = square_to_bit(square);
  bitboards[bitboard_index(color, piece)] &= ~square_bit;
  occupancy[color] &= ~square_bit;
  occupancy[ANY] &= ~square_bit;

  lookup_table[square] = 0;
}

void Position::pass_turn() {
  bool black_played = to_move == BLACK;
  if (black_played) fullmove_counter++;

  to_move = opposite_color(to_move);
}

void Position::set_undo_info(Square from, Square to, Square captured_square,
                             uint8_t moved_piece_encoded,
                             uint8_t captured_piece_encoded) {
  undo_info.from_square = from;
  undo_info.to_square = to;
  undo_info.captured_square = captured_square;

  undo_info.moved_piece_encoded = moved_piece_encoded;
  undo_info.captured_piece_encoded = captured_piece_encoded;

  undo_info.castling_rights = castling_rights;
  undo_info.en_passant_square = en_passant_square;
  undo_info.halfmove_clock = halfmove_clock;
}
