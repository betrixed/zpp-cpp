# Copilot Instructions for PHP-CPP WCC Extension

## Project Overview
This repository implements a PHP extension (WCC) using C++ and integrates with PHP via custom modules. The codebase is organized for building, testing, and extending PHP functionality with C++ performance and type safety.

## Key Directories & Files
- `wcc.cpp`, `include/`, `ds/`, `modules/`: Core C++ source and headers for the extension.
- `stub/`: Auto-generated PHP arginfo and stub files for function signatures.
- `php/`: PHP-side test and utility scripts.
- `tests/`: Test cases and sample data for extension validation.
- `build/`: Autotools and build system scripts (e.g., `libtool.m4`, `Makefile.global`).
- `config.h`, `config.m4`, `configure.ac`: Build configuration for PHP extension.

## Build & Test Workflow
- **Configure:** Run `./configure` to generate Makefiles and config headers.
- **Build:** Run `make` to compile the extension. Output is `modules/wcc.so`.
- **Test:** Use `run-tests.php` or `php/phpunit` for running tests in `tests/`.
- **Debug:** Use `debug.sh` for debug builds or troubleshooting.
- **Restart:** Use `restart.sh` to reload/restart the PHP extension in your environment.

## Project-Specific Patterns
- **Stubs Generation:** `.stub.php` and `*_arginfo.h` files in `stub/` are generated and should not be edited manually.
- **C++/PHP Integration:** All extension entry points and PHP-exposed functions are declared in `wcc.cpp` and headers in `include/`.
- **Test Data:** TOML and XML files in `tests/` are used for data-driven tests.
- **Custom Scripts:** Scripts like `build.sh`, `pvid.sh`, and `debug.sh` encapsulate common workflows—prefer these over manual commands.

## Conventions
- **Header Guards:** All headers use `#ifndef`/`#define` guards.
- **Namespace:** C++ code uses project-specific namespaces (e.g., `wcc`, `ds`).
- **No Direct Edits:** Do not edit files in `stub/` or `modules/` directly.
- **PHP Integration:** All PHP-visible symbols must be registered in the extension entry points.

## Examples
- To add a new PHP function, declare it in a `.stub.php` file, run the stub generator, then implement in C++.
- To run all tests: `php run-tests.php tests/`
- To rebuild after code changes: `./build.sh` or `make`

## External Dependencies
- Requires PHP development headers and build tools (autotools, libtool, etc.).
- Uses PHP-Parser for some PHP-side utilities (see `php/`).

---
For more details, see `wcc.cpp`, `include/`, and `tests/` for canonical patterns.
