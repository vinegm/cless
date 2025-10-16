#include "uci_interface.hpp"
#include <iostream>
#include <sstream>

void ClessUCI::run() {
  std::string line;

  while (std::getline(std::cin, line)) {
    std::vector<std::string> tokens = split_string(line);

    if (tokens.empty()) continue;

    call_command(tokens);
  }
}

void ClessUCI::call_command(const std::vector<std::string> &tokens) {
  std::string command = tokens[0];

  if (command == "uci") return handle_uci();
  if (command == "debug") return handle_debug(tokens);
  if (command == "isready") return handle_isready();
  if (command == "setoption") return handle_setoption(tokens);
  if (command == "register") return handle_register(tokens);
  if (command == "ucinewgame") return handle_ucinewgame();
  if (command == "position") return handle_position(tokens);
  if (command == "go") return handle_go(tokens);
  if (command == "stop") return handle_stop();
  if (command == "ponderhit") return handle_ponderhit();
}

void ClessUCI::handle_uci() {
  send_response("id name Cless Engine");
  send_response("id author vinegm");
  send_response("uciok");
}

void ClessUCI::handle_debug(const std::vector<std::string> &tokens) {
  if (tokens.size() >= 2) debug_mode = (tokens[1] == "on");
}

void ClessUCI::handle_isready() { send_response("readyok"); }

void ClessUCI::handle_setoption(const std::vector<std::string> &tokens) {
  throw std::runtime_error("Not implemented");
}

void ClessUCI::handle_register(const std::vector<std::string> &tokens) {
  throw std::runtime_error("Not implemented");
}

void ClessUCI::handle_ucinewgame() { engine = std::make_unique<ClessEngine>(); }

void ClessUCI::handle_position(const std::vector<std::string> &tokens) {
  throw std::runtime_error("Not implemented");
}

// Only handles perft atm
void ClessUCI::handle_go(const std::vector<std::string> &tokens) {
  if (tokens[1] == "perft") {
    int depth = std::stoi(tokens[2]);

    auto results = engine->perft_divide(depth);

    int total_nodes = 0;
    for (const auto &[move, nodes] : results) {
      std::string move_str = move_to_uci(move);
      send_response(move_str + ": " + std::to_string(nodes));
      total_nodes += nodes;
    }

    send_response("\nNodes searched: " + std::to_string(total_nodes));
    return;
  }

  throw std::runtime_error(
      "Go command not fully implemented - only perft is supported");
}

void ClessUCI::handle_stop() { throw std::runtime_error("Not implemented"); }

void ClessUCI::handle_ponderhit() {
  throw std::runtime_error("Not implemented");
}

std::vector<std::string> ClessUCI::split_string(const std::string &str) {
  std::vector<std::string> tokens;
  std::istringstream iss(str);
  std::string token;

  while (iss >> token) {
    tokens.push_back(token);
  }

  return tokens;
}

void ClessUCI::send_response(const std::string &response) {
  std::cout << response << std::endl;
  std::cout.flush();
}

void ClessUCI::send_info(const std::string &info) {
  std::cout << "info " << info << std::endl;
  std::cout.flush();
}

std::string ClessUCI::square_to_uci(Square square) {
  int file = square % 8;
  int rank = square / 8;

  std::string result;
  result += ('a' + file);
  result += ('1' + rank);

  return result;
}

Square ClessUCI::uci_to_square(const std::string &uci) {
  if (uci.length() < 2) {
    throw std::invalid_argument("Invalid UCI square notation");
  }

  char file_char = std::tolower(uci[0]);
  char rank_char = uci[1];

  if (file_char < 'a' || file_char > 'h' || rank_char < '1' ||
      rank_char > '8') {
    throw std::invalid_argument("Invalid UCI square notation");
  }

  int file = file_char - 'a';
  int rank = rank_char - '1';

  return static_cast<Square>(rank * 8 + file);
}

std::string ClessUCI::move_to_uci(const Move &move) {
  std::string result = square_to_uci(move.from) + square_to_uci(move.to);

  if (move.is_promotion()) {
    result += piece_type_to_char(move.promotion_piece);
  }

  return result;
}

char ClessUCI::piece_type_to_char(PieceType type) {
  switch (type) {
    case PIECE_QUEEN: return 'q';
    case PIECE_ROOK: return 'r';
    case PIECE_BISHOP: return 'b';
    case PIECE_KNIGHT: return 'n';
    default: return 'q';
  }
}

PieceType ClessUCI::char_to_piece_type(char c) {
  switch (std::tolower(c)) {
    case 'q': return PIECE_QUEEN;
    case 'r': return PIECE_ROOK;
    case 'b': return PIECE_BISHOP;
    case 'n': return PIECE_KNIGHT;
    default: return PIECE_QUEEN;
  }
}
