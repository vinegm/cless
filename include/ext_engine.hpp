#pragma once

#include <cstdio>
#include <memory>
#include <string>
#include <sys/types.h>

struct FileDeleter {
  void operator()(FILE *file) const {
    if (file) fclose(file);
  }
};

using FilePtr = std::unique_ptr<FILE, FileDeleter>;

class ExtEngine {
public:
  ExtEngine(const std::string &command);
  ~ExtEngine();

  void send_command(const std::string &command);
  void set_position(const std::string &fen);
  std::string get_best_move(int depth = 1, int timeMs = 1000);

private:
  std::string command;
  pid_t childPid;
  FilePtr engineIn;
  FilePtr engineOut;

  std::string read_line();
  std::string read_until(const std::string &expected_response, int timeout_ms);
};
