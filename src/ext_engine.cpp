#include "ext_engine.hpp"

#include <chrono>
#include <fcntl.h>
#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

ExtEngine::ExtEngine(const std::string &command) : command(command), childPid(-1) {
  int in_fd[2];
  int out_fd[2];

  if (pipe(in_fd) == -1 || pipe(out_fd) == -1) {
    throw std::runtime_error("Failed to create pipes for UCI engine.");
  }

  childPid = fork();
  if (childPid == -1) {
    close(in_fd[0]);
    close(in_fd[1]);
    close(out_fd[0]);
    close(out_fd[1]);
    throw std::runtime_error("Failed to fork process for UCI engine.");
  }

  if (childPid == 0) {
    dup2(in_fd[0], STDIN_FILENO);
    dup2(out_fd[1], STDOUT_FILENO);
    dup2(out_fd[1], STDERR_FILENO);

    close(in_fd[0]);
    close(in_fd[1]);
    close(out_fd[0]);
    close(out_fd[1]);

    execlp(command.c_str(), command.c_str(), nullptr);
    _exit(1);
  }

  close(in_fd[0]);
  close(out_fd[1]);

  engineIn.reset(fdopen(in_fd[1], "w"));
  engineOut.reset(fdopen(out_fd[0], "r"));

  if (!engineIn || !engineOut) {
    throw std::runtime_error("Failed to create file streams for UCI engine.");
  }

  setvbuf(engineIn.get(), nullptr, _IOLBF, 0);
  setvbuf(engineOut.get(), nullptr, _IOLBF, 0);

  send_command("uci");
  std::string response = read_until("uciok", 1500);
  if (response.empty()) {
    throw std::runtime_error("UCI engine failed to respond with uciok within timeout.");
  }
}

ExtEngine::~ExtEngine() {
  send_command("quit");

  if (childPid > 0) {
    kill(childPid, SIGTERM);
    waitpid(childPid, nullptr, 0);
    childPid = -1;
  }
}

void ExtEngine::send_command(const std::string &command) {
  fprintf(engineIn.get(), "%s\n", command.c_str());
  fflush(engineIn.get());
}

void ExtEngine::set_position(const std::string &fen) { send_command("position fen " + fen); }

std::string ExtEngine::get_best_move(int depth, int timeMs) {
  std::string goCommand = "go";
  if (depth > 0) goCommand += " depth " + std::to_string(depth);
  if (timeMs > 0) goCommand += " movetime " + std::to_string(timeMs);

  send_command(goCommand);
  std::string bestmove_line = read_until("bestmove", timeMs + 15000);
  if (bestmove_line.empty()) return "";

  std::istringstream string_stream(bestmove_line);
  std::string token;
  string_stream >> token;
  if (string_stream >> token) return token;

  return "";
}

std::string ExtEngine::read_line() {
  if (!engineOut) { return ""; }

  char buffer[1024];
  if (fgets(buffer, sizeof(buffer), engineOut.get())) {
    std::string line(buffer);
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) {
      line.pop_back();
    }

    return line;
  }

  return "";
}

std::string ExtEngine::read_until(const std::string &expected_response, int timeout_ms) {
  auto start_time = std::chrono::steady_clock::now();
  auto timeout_duration = std::chrono::milliseconds(timeout_ms);

  while (true) {
    std::string line = read_line();

    if (!line.empty() && line.find(expected_response) != std::string::npos) return line;

    auto current_time = std::chrono::steady_clock::now();
    if (current_time - start_time > timeout_duration) return "";
  }
}
