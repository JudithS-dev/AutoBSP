# AutoBSP

AutoBSP is a domain-specific, configuration-based code generation tool for the automated generation of Board Support Package (BSP) code for microcontroller-based embedded systems.

## Project Overview

This repository contains:

- The implementation of the `AutoBSP` generator (Flex/Bison + C)
- Example DSL input files for the supported target platforms
- Application code used in the cross-platform evaluation presented in the associated Master's thesis:
  - ESP32 (ESP-IDF)
  - STM32F446RE (STM32CubeIDE project structure)

## Supported Platforms

- STM32F446RE
- ESP32

## Requirements

Minimum tools:

- `gcc` / `make`
- `flex`
- `bison`
- `valgrind` (optional, for memory leak checks)
- Graphviz (`dot`) for rendering generated AST `.dot` files to `.png` using helper scripts

## Usage

### Option 1: Run the testing script

The provided testing script builds the generator (if necessary) and allows selection of a demo program (`STM32.dsl` or `ESP32.dsl`). The generated BSP code can then be copied into the corresponding example projects.

```bash
./testing.sh
```

### Option 2: Run the generator directly

```bash
./generator/AutoBSP <configuration_file> [output_path]
```

If no `output_path` is provided, the tool uses a default output directory.

## Clean Build Artifacts

To remove build artifacts and generated output files, run:

```bash
./clean.sh
```

## Additional Information

For further details on the system architecture, design decisions, and evaluation results, please refer to the thesis documentation.