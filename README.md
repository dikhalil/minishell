# minishell

A minimal Unix shell implementation written in C, built as part of the [42](https://42.fr/) curriculum at **42 Amman**.

## Overview

`minishell` is a simplified shell that reads commands from standard input, parses them, and executes them. It supports command pipelines, input/output redirections, environment variable expansion, heredocs, and a set of built-in commands.

## Features

- **Interactive prompt** using GNU Readline with history support
- **Command execution** via `fork`/`execve` with `PATH` resolution
- **Pipelines** (`|`) — chain multiple commands with inter-process pipes
- **Redirections:**
  - `<` — input from file
  - `>` — output to file (truncate)
  - `>>` — output to file (append)
  - `<<` — heredoc
- **Environment variable expansion** (`$VAR`, `$?` for exit status)
- **Quote handling** — single quotes (literal) and double quotes (expand `$`)
- **Built-in commands:**
  - `cd` — change directory (with `-`, `~`, `OLDPWD` support)
  - `echo` — print arguments (supports `-n`)
  - `env` — display environment variables
  - `exit` — exit the shell
  - `export` — set/export environment variables
  - `unset` — remove environment variables
  - `pwd` — print working directory
- **Signal handling** — `SIGINT` (Ctrl+C), `SIGQUIT` (Ctrl+\\) handled appropriately in interactive, heredoc, and child process modes
- **Exit status tracking** — `$?` reflects the last command's exit code

## Requirements

- **C compiler** (gcc/clang)
- **GNU Readline** library (`libreadline-dev` on Debian/Ubuntu, `readline` on macOS)
- **Make**

### Installing Readline

**Debian/Ubuntu:**
```sh
sudo apt-get install libreadline-dev
```

**macOS (Homebrew):**
```sh
brew install readline
```

## Building

```sh
make
```

This compiles the project and produces the `minishell` binary.

### Cleanup

```sh
make clean     # remove object files
make fclean    # remove object files + binary + libft.a
make re        # fclean + all
```

## Usage

```sh
./minishell
```

You will be presented with a `minishell$ ` prompt. Type commands as you would in a standard Unix shell.

## Project Structure

```
minishell/
├── include/
│   └── minishell.h          # Main header — types, globals, prototypes
├── libft/                   # Custom C library (libft.a)
│   ├── libft.h
│   └── ... (~60 source files)
├── src/
│   ├── clean/
│   │   └── cleanup.c        # Memory deallocation and exit
│   ├── env/                 # Environment variable management
│   ├── executor/            # Command execution, piping, redirections
│   │   └── builtins/        # Built-in command implementations
│   ├── expand/              # Variable expansion ($VAR)
│   ├── heredoc/             # Here-document processing
│   ├── lexer/               # Tokenizer / lexical analysis
│   ├── parser/              # Syntax analysis / AST construction
│   ├── shell/               # Main loop and entry point
│   └── signals/             # Signal handler setup
├── Makefile
└── README.md
```

### Data Flow

```
readline() → lexer() → parser() → heredoc() → executor() → free_all()
```

## Design

The shell processes input through a pipeline of stages:

1. **Lexer** — tokenizes the input string into tokens (`WORD`, `PIPE`, `IN_REDIR`, `OUT_REDIR`, `APPEND`, `HEREDOC`), tracking quote context
2. **Parser** — builds a linked list of command structures (`t_cmd`) with their arguments (`t_arg`) and redirections (`t_redir`)
3. **Heredoc** — processes here-documents by reading lines until the delimiter is encountered
4. **Expander** — expands environment variable references in words
5. **Executor** — forks child processes, sets up pipes and redirections, resolves command paths, and waits for completion

## Authors

- **dikhalil** — [dikhalil@student.42amman.com](mailto:dikhalil@student.42amman.com)
- **aalkhaso** — [aalkhaso@student.42amman.com](mailto:aalkhaso@student.42amman.com)

## License

This project is part of the 42 School curriculum and is provided for educational purposes.
