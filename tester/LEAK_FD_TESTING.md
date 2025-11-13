# Memory Leak & File Descriptor Testing

## Overview
The enhanced tester now includes comprehensive memory leak detection and file descriptor leak checking, with proper suppression of readline library leaks.

## Features

### 1. Memory Leak Detection
- Uses **Valgrind** for leak detection
- Automatically suppresses **readline** and **ncurses** leaks
- Tests all major command types:
  - Builtins (echo, pwd, cd, env, export, unset)
  - Pipes
  - Redirections
  - Heredocs
  - Variable expansion
  - Quotes

### 2. File Descriptor Leak Detection
- Monitors open file descriptors before and after commands
- Detects FD leaks that could cause resource exhaustion
- Tests critical operations:
  - Pipe handling
  - File redirections
  - Heredoc operations

### 3. Readline Leak Suppression
The tester automatically creates a `readline_curses.supp` file that suppresses known leaks from:
- `readline` library
- `libreadline.so`
- `add_history` function
- `ncurses` library
- `tgetent` function

## Usage

### Running All Tests (Including Leaks/FDs)
```bash
./minishell_tester.sh
# Answer 'y' when prompted for leak/FD tests
```

### Running Only Functional Tests
```bash
./minishell_tester.sh
# Answer 'n' when prompted
```

### Manual Leak Check
```bash
echo -e "your command\nexit" | valgrind --leak-check=full \
    --suppressions=readline_curses.supp \
    --error-exitcode=42 \
    --log-file=valgrind.log \
    ./minishell
```

## Output Format

### Memory Leak Test Output
```
Testing: Leak check: echo hello
  ✓ No leaks detected
```

or

```
Testing: Leak check: complex command
  ✗ Memory leaks detected
  ==12345== definitely lost: 40 bytes in 1 blocks
  ==12345== indirectly lost: 100 bytes in 5 blocks
```

### File Descriptor Test Output
```
Testing: FD check: echo hello
  ✓ No FD leaks (initial: 4, final: 4)
```

or

```
Testing: FD check: pipe command
  ✗ FD leak detected (initial: 4, final: 7)
```

## Test Summary

The final summary includes separate statistics for:

1. **Functional Tests**
   - Total tests
   - Passed/Failed
   - Success rate %

2. **Memory Leak Tests**
   - Total leak tests
   - Passed/Failed
   - Success rate %

3. **File Descriptor Tests**
   - Total FD tests
   - Passed/Failed
   - Success rate %

Example:
```
═══════════════════════════════════════════════════════════════════════════
  FUNCTIONAL TESTS
═══════════════════════════════════════════════════════════════════════════
Total tests: 98
Passed: 87
Failed: 11
Success rate: 88%

═══════════════════════════════════════════════════════════════════════════
  MEMORY & FILE DESCRIPTOR TESTS
═══════════════════════════════════════════════════════════════════════════
Memory Leak Tests:
  Total: 14
  Passed: 14
  Failed: 0
  Success rate: 100%

File Descriptor Tests:
  Total: 4
  Passed: 4
  Failed: 0
  Success rate: 100%

🎉 All tests passed! Perfect score! 🎉
```

## Requirements

### For Memory Leak Testing
- **Valgrind** must be installed
  ```bash
  # Ubuntu/Debian
  sudo apt-get install valgrind
  
  # Fedora/RHEL
  sudo dnf install valgrind
  
  # Arch
  sudo pacman -S valgrind
  ```

### For FD Testing
- Linux system with `/proc` filesystem (standard on most Linux distributions)
- No additional dependencies

## Tested Commands

### Memory Leak Tests (14 tests)
1. `echo hello` - Basic builtin
2. `pwd` - Directory command
3. `env` - Environment display
4. `export TEST=value` - Variable export
5. `unset TEST` - Variable removal
6. `cd ..` - Directory change
7. `echo test | cat` - Simple pipe
8. `ls | grep minishell | wc -l` - Multiple pipes
9. `echo test > /tmp/leak_test.txt` - Output redirect
10. `cat < /tmp/leak_test.txt` - Input redirect
11. `echo 'hello world'` - Single quotes
12. `echo "hello world"` - Double quotes
13. `echo $PATH` - Variable expansion
14. `cat << EOF\ntest\nEOF` - Heredoc

### File Descriptor Tests (4 tests)
1. `echo hello` - Basic command
2. `cat /etc/passwd | head -5` - Pipe operation
3. `echo test > /tmp/fd_test.txt` - Redirect operation
4. `cat << EOF\ntest\nEOF` - Heredoc operation

## Why Suppress Readline Leaks?

Readline is a library provided by the system that handles command-line input. It has known "leaks" that are:
1. **Intentional** - The library caches data for performance
2. **Not real leaks** - Memory is reused across readline calls
3. **Freed on exit** - The OS reclaims all memory when the program exits

Suppressing these leaks allows us to focus on **actual memory leaks** in your minishell code.

## Interpreting Results

### Perfect Score
```
✓ All tests passed! Perfect score!
```
- No functional failures
- No memory leaks
- No FD leaks
- Production ready!

### Functional Pass, Leak/FD Issues
```
✓ All functional tests passed!
⚠ Some memory/FD tests failed
```
- Commands work correctly
- But there are resource management issues
- Fix leaks before considering complete

### Functional Failures
```
✗ Some tests failed
```
- Fix functional issues first
- Then address leak/FD problems

## Tips for Fixing Leaks

### Memory Leaks
1. Check `malloc`/`free` pairs
2. Verify all error paths free memory
3. Use `valgrind` directly on specific commands
4. Look for strdup/calloc without matching free

### File Descriptor Leaks
1. Check all `open`/`close` pairs
2. Verify pipes are closed in parent and child
3. Check error handling paths close FDs
4. Use `lsof -p <pid>` to see open FDs

## Advanced Usage

### Run Leak Check on Specific Command
```bash
# Create test function
run_leak_check() {
    echo -e "$1\nexit" | valgrind --leak-check=full \
        --suppressions=readline_curses.supp \
        --log-file=valgrind.log \
        ./minishell > /dev/null 2>&1
    grep "definitely lost" valgrind.log
}

# Test specific command
run_leak_check "your command here"
```

### Monitor FDs During Execution
```bash
# In one terminal
./minishell

# In another terminal (get PID first)
watch -n 1 'lsof -p <PID> | wc -l'
```

## Troubleshooting

### "valgrind not found"
Install valgrind using your package manager (see Requirements section).

### "Too many false positives"
The suppression file should handle most false positives from readline. If you see others, add them to `readline_curses.supp`.

### "FD tests always fail"
Some systems have different baseline FD counts. The test allows up to 2 FDs difference. If needed, adjust the threshold in `run_fd_check()`.

## Files Created

1. **readline_curses.supp** - Valgrind suppression file (auto-generated)
2. **/tmp/valgrind_out.txt** - Detailed valgrind output
3. **/tmp/leak_test.txt** - Temporary test file (cleaned up)
4. **/tmp/fd_test.txt** - Temporary test file (cleaned up)

All temporary files are automatically cleaned up after testing.
