<div align="center">
<pre>
.  ██████╗██╗     ███████╗███████╗███████╗ .
. ██╔════╝██║     ██╔════╝██╔════╝██╔════╝ .
. ██║     ██║     █████╗  ███████╗███████╗ .
. ██║     ██║     ██╔══╝  ╚════██║╚════██║ .
. ╚██████╗███████╗███████╗███████║███████║ .
.  ╚═════╝╚══════╝╚══════╝╚══════╝╚══════╝ .
<strong>A lightweight terminal-based chess game written in C++</strong>
</pre>
</div>

Cless (short for CLI Chess, even though it's a TUI) is a terminal-based chess game written in C++ that allows you to play chess locally against another human or against UCI-compatible chess engines.

## Requirements

- **C++ Compiler**: Supporting C++17 standard
- **ncurses**: For terminal UI functionality
- **CMake**: Version 3.25 or higher
- **Make**: Build system

## Installation

### Building from Source

1. **Clone the repository:**

   ```bash
   git clone https://github.com/2viny/cless.git
   cd cless
   ```

2. **Create build directory and compile:**

   ```bash
   mkdir build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   make
   ```

3. **Run the executable:**
   ```bash
   ./cless
   ```

### Installing on Arch Linux

If you're using Arch Linux, you can install directly using the provided PKGBUILD:

```bash
makepkg -si
```

> Note: In the future it will be possible to install cless from the AUR, once that is possible this README will be updated.

## Usage

### Basic Usage

Start the game without any arguments for human vs human play:

```bash
cless
```

### Playing Against an Engine

To play against a UCI-compatible chess engine:

```bash
cless --engine "/path/to/your/engine"
```

Popular UCI engines you can use:

- **Stockfish**: `cless --engine "stockfish"`
- **GNU Chess**: `cless --engine "gnuchess"`

> Note: In the future cless is supposed to also allow options to be passed to the engine, this is a work in progress at the moment.

## Development

### Building for Development

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Future Planned Features

- **LAN Multiplayer Support**: Play against friends over the network
- **Game Analysis**: Move history and position evaluation (when an engine is provided)
- **Engine Configuration**: Adjustable difficulty settings

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
