# GEM++ Subgraph Matching Tool

A standalone C program for subgraph matching using libgem++ and GLPK.

## Prerequisites

1. **libgem++ library**: Must be built first from the main GEM++ project
2. **GLPK**: GNU Linear Programming Kit
3. **Qt5**: Required by libgem++
4. **CMake**: For building this project

## Building libgem++

First, build the main GEM++ library:

```bash
# In the root GEM++ directory
qmake GEM++.pro
make
```

This will create the libGEM++.so (Linux/macOS) or GEM++.dll (Windows) library.

## Building gempp-sub

```bash
cd gempp-sub
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./gempp-sub [OPTIONS] input.txt
```

### Options

- `-t, --tolerance LEVEL`: Tolerance level (e/l/t)
  - `e` or `exact`: Perfect matching
  - `l` or `label`: Tolerant to label errors
  - `t` or `topology`: Tolerant to label and topology errors (default)
- `-i, --induced`: Perform induced subgraph matching
- `-s, --substitution FILE`: Substitution weights file
- `-c, --creation FILE`: Creation weights file
- `-n, --number NUM`: Number of best solutions to search (default: 1)
- `-e, --explore PERCENT`: Upper bound approximation (0-100%, default: 100)
- `--time LIMIT`: Maximum solving time in seconds
- `--solver SOLVER`: Solver to use (default: GLPK)
- `-p, --program FILE`: Output program to file
- `-o, --solution FILE`: Output solution to file
- `-v, --verbose`: Enable verbose output
- `-h, --help`: Show help
- `--version`: Show version

### Input Format

The input file must be a `.txt` file containing adjacency matrices for two graphs:

```
3
0 1 0
1 0 1
0 1 0
4
0 1 1 0
1 0 0 0
1 0 0 1
0 0 1 0
```

First line: number of vertices in first graph
Next lines: adjacency matrix for first graph
Next line: number of vertices in second graph
Remaining lines: adjacency matrix for second graph

## Cross-platform Notes

- **Linux/macOS**: Uses `.so` shared libraries
- **Windows**: Uses `.dll` files
- The CMakeLists.txt automatically detects the platform and sets appropriate library paths

## Dependencies

- libgem++ (built from main project)
- GLPK 4.65+
- Qt5 Core, XML, GUI, Widgets
- CMake 3.10+