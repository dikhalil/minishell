# Minishell Tester Suite

Comprehensive testing framework for the minishell project with 294+ tests covering syntax, execution, builtins, redirections, pipes, signals, memory leaks, and file descriptor leaks.

## Quick Start

```bash
# From the tester directory
./minishell_tester.sh

# Or from the project root
./tester/minishell_tester.sh
```

## Features

### ✅ Test Coverage (294+ Tests)
- **Basic Commands**: echo, pwd, env, exit
- **Builtins**: cd, export, unset, exit
- **Redirections**: `<`, `>`, `>>`, heredoc (`<<`)
- **Pipes**: Single and multiple pipes
- **Quotes**: Single quotes, double quotes, mixed
- **Environment Variables**: Expansion, special variables (`$?`, `$_`)
- **Edge Cases**: Empty commands, whitespace, PATH unset
- **Signals**: CTRL+C, CTRL+D, CTRL+\ handling
- **Heredoc**: With and without expansion, signal handling

### ✅ 4-Dimensional Validation
1. **Output Correctness**: Compares with bash output
2. **Exit Status**: Validates exit codes match bash
3. **Memory Leaks**: Valgrind with child process tracking
4. **FD Leaks**: Monitors file descriptor leaks

## Requirements

```bash
# Required
- bash
- minishell binary (in parent directory)

# Optional (for leak/FD tests)
- valgrind
- /proc filesystem (Linux)
```

## Usage

### Run All Tests
```bash
./minishell_tester.sh
```

### Run Specific Test Suites
```bash
# Edit minishell_tester.sh and uncomment desired test functions
# At the bottom of the file:

# test_basic_commands        # Basic echo, pwd, env
# test_builtins             # cd, export, unset, exit
# test_redirections         # Input/output redirections
# test_pipes                # Pipe functionality
# test_quotes               # Quote handling
# test_environment          # Environment variables
# test_edge_cases           # Edge cases and errors
# test_empty_commands       # Empty input handling
# test_signals              # Signal handling
# test_expansions           # Variable expansion
# test_advanced_builtins    # Advanced builtin tests
# test_heredoc              # Heredoc functionality
```

### Enable/Disable Leak & FD Tests

Edit the configuration at the top of `minishell_tester.sh`:

```bash
# Set to true to enable leak and FD checks (requires valgrind)
RUN_LEAK_FD_TESTS=true   # or false to disable
```

## Test Output Format

```
Testing: echo hello
  ✓ PASS
  ✓ Exit status: 0
  ✓ No leaks
  ✓ No FD leaks

Testing: cd /nonexistent
  ✗ FAIL - Exit status mismatch
  Expected: 1, Got: 0
```

## Files

- **minishell_tester.sh**: Main test script
- **readline_curses.supp**: Valgrind suppressions for readline/curses
- **test_leak_check.sh**: Standalone leak testing script
- **test_leak_integration.sh**: Integration leak tests
- **TESTER_COMPREHENSIVE.md**: Detailed documentation
- **INTEGRATED_TESTING_GUIDE.md**: Integration guide
- **LEAK_FD_TESTING.md**: Leak/FD testing documentation
- **test_cases_added.md**: Test case additions log
- **TESTS_ADDED_SUMMARY.txt**: Summary of added tests
- **Case.pdf**: Test case reference document

## Leak Detection

The tester includes advanced leak detection:

### Features
- **Child Process Tracking**: Monitors forked processes
- **Intelligent Filtering**: Excludes system command leaks
- **Readline Suppression**: Filters out readline/curses leaks
- **Comprehensive Coverage**: Parent and child process leaks

### Valgrind Options Used
```bash
valgrind --leak-check=full \
         --trace-children=yes \
         --child-silent-after-fork=yes \
         --suppressions=readline_curses.supp
```

### What Gets Checked
✓ Parent process memory allocation  
✓ Child process memory allocation  
✓ Pipe and redirection handling  
✗ System command leaks (excluded)  
✗ Readline library leaks (suppressed)

## Statistics Summary

After running tests, you'll see:

```
════════════════════════════════════════
            TEST SUMMARY
════════════════════════════════════════

STANDARD TESTS:
  Total Tests:     294
  Passed:          286
  Failed:          8
  Success Rate:    97.3%

EXIT STATUS VALIDATION:
  Total Checks:    294
  Passed:          290
  Failed:          4
  Success Rate:    98.6%

MEMORY LEAK CHECKS:
  Total Tests:     188
  Passed:          185
  Failed:          3
  Success Rate:    98.4%

FILE DESCRIPTOR CHECKS:
  Total Tests:     188
  Passed:          187
  Failed:          1
  Success Rate:    99.5%
```

## Troubleshooting

### Valgrind Not Found
```bash
# Install valgrind
sudo apt-get install valgrind  # Debian/Ubuntu
sudo yum install valgrind      # RedHat/CentOS
```

### Minishell Binary Not Found
```bash
# Ensure minishell is compiled in parent directory
cd ..
make
cd tester
```

### Permission Denied
```bash
chmod +x minishell_tester.sh
```

## Contributing

When adding new tests:
1. Use appropriate test function (`run_test`, `run_test_no_compare`, etc.)
2. Add comments explaining what's being tested
3. Group related tests in sections
4. Update this README with new test categories

## Documentation

- **TESTER_COMPREHENSIVE.md**: Complete tester documentation
- **LEAK_FD_TESTING.md**: Leak and FD detection details
- **INTEGRATED_TESTING_GUIDE.md**: Integration testing guide

## License

Part of the 42 School minishell project.
