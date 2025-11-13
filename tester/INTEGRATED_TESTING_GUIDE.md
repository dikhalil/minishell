# Integrated Memory Leak & FD Testing Guide

## Overview
The minishell tester now includes **integrated memory leak and file descriptor checking** that runs automatically for each test case when enabled. This provides comprehensive validation of your minishell implementation.

## How It Works

### Automatic Integration
When you enable leak/FD testing, **every functional test** automatically includes:
1. ✅ **Functional check** - Does output match bash?
2. 🔍 **Memory leak check** - Are there any memory leaks?
3. 📂 **File descriptor check** - Are all FDs properly closed?

### Example Output
```
Testing: echo Hello World
✓ PASS
  ✓ No leaks
  ✓ No FD leaks
```

If there's an issue:
```
Testing: echo $VAR
✓ PASS
  ✗ Memory leak detected
    definitely lost: 24 bytes in 1 blocks
  ✓ No FD leaks
```

## Running the Tester

### Quick Start
```bash
./minishell_tester.sh
```

You'll be prompted:
```
Run memory leak and FD checks for each test? (slower but comprehensive)
Answer (y/n):
```

- Answer **`y`** for complete testing (functional + leak + FD for ALL tests)
- Answer **`n`** for functional testing only (faster)

### What Happens When You Answer 'y'

1. **For each passing test**, the tester will:
   - Run valgrind with readline suppression
   - Check for memory leaks
   - Monitor file descriptor counts
   - Display results inline

2. **Test execution** will be slower (2-3x) but comprehensive

3. **Final summary** shows three categories:
   - Functional tests (96 tests)
   - Memory leak tests (per passing test)
   - FD leak tests (per passing test)

## Understanding Results

### ✓ PASS with ✓ No leaks + ✓ No FD leaks
**Perfect!** Your test passed all three checks:
- Output matches bash
- No memory leaks
- No file descriptor leaks

### ✓ PASS with ✗ Memory leak detected
**Partial success:** Command works but has memory leaks
- Fix: Review malloc/free pairs in your code
- Look at valgrind output for the leak location

### ✓ PASS with ✗ FD leak
**Partial success:** Command works but doesn't close all file descriptors
- Fix: Ensure all open() calls have matching close()
- Check pipe file descriptors are closed in both parent and child

### ✗ FAIL
**Test failed:** Output doesn't match bash
- No leak/FD checks run for failing tests (fix functionality first)

## Technical Details

### Memory Leak Detection
- Uses `valgrind --leak-check=full`
- **Suppresses readline leaks** (they're not real leaks)
- Checks for "definitely lost" and "indirectly lost" memory
- Only real leaks in YOUR code are reported

### File Descriptor Detection
- Monitors `/proc/$$/fd` before and after command
- Allows variance of ±2 FDs (normal for shell operations)
- Detects unclosed pipes, files, and redirections

### What Gets Tested

#### Tests WITH leak/FD checks (when passing):
- All `run_test()` calls (96 tests)
- Examples:
  - `echo Hello World`
  - `cat file.txt`
  - `ls | grep minishell`
  - `echo test > output.txt`

#### Tests WITHOUT leak/FD checks:
- Tests with `run_test_no_compare()` (system-dependent output)
- Interactive tests with `run_interactive_test()` (exit codes)
- Custom validation tests (grep-based checks)

## Performance

### Timing Comparison
- **Functional only**: ~5-10 seconds for 96 tests
- **With leak/FD**: ~30-45 seconds for 96 tests + leak/FD checks

### Optimization
The tester only runs leak/FD checks on **passing tests**. If a test fails functionally, it skips the memory/FD checks to save time.

## Requirements

### Essential
- `bash` - For comparison testing
- Compiled `./minishell` binary

### For Leak/FD Testing
- `valgrind` - Memory leak detection
- `/proc` filesystem - FD monitoring (Linux)

### Install Valgrind
```bash
# Ubuntu/Debian
sudo apt-get install valgrind

# Fedora
sudo dnf install valgrind

# Arch
sudo pacman -S valgrind

# macOS (via Homebrew)
brew install valgrind
```

## Suppression File

The tester automatically creates `readline_curses.supp` to suppress false positives from:
- `readline()` function
- `add_history()` function
- `libreadline.so`
- `libncurses*.so`
- `tgetent()` function

These libraries intentionally cache data and don't free it until process exit. The suppression file prevents them from showing as "leaks" in your code.

## Interpreting Summary

### Example Output
```
═══════════════════════════════════════════════════════════════════════════
  FUNCTIONAL TESTS
═══════════════════════════════════════════════════════════════════════════
Total tests: 96
Passed: 86
Failed: 10
Success rate: 89%

═══════════════════════════════════════════════════════════════════════════
  MEMORY & FILE DESCRIPTOR TESTS
═══════════════════════════════════════════════════════════════════════════
Memory Leak Tests:
  Total: 86
  Passed: 84
  Failed: 2
  Success rate: 97%

File Descriptor Tests:
  Total: 86
  Passed: 85
  Failed: 1
  Success rate: 98%
```

### What This Means
- **86 functional tests passed** - Commands work correctly
- **84 have no memory leaks** - 2 commands leak memory
- **85 have no FD leaks** - 1 command leaks file descriptors

## Debugging Tips

### Memory Leak Found
1. Scroll up to find the specific test that leaked
2. Note the leak details (e.g., "24 bytes in 1 blocks")
3. Run manually with full valgrind:
   ```bash
   echo "your_command" | valgrind --leak-check=full \
       --show-leak-kinds=all \
       --suppressions=readline_curses.supp \
       ./minishell
   ```
4. Look for the stack trace showing where memory was allocated
5. Add corresponding `free()` call

### FD Leak Found
1. Find which test showed the FD leak
2. Check if command uses:
   - Pipes (need close in both parent/child)
   - Redirections (need close after dup2)
   - Heredoc (need close temporary file)
3. Use `lsof` to see open files:
   ```bash
   # In one terminal
   ./minishell
   
   # In another terminal
   lsof -p $(pgrep minishell)
   ```

### False Positives
If you see consistent FD variance of 1-2:
- This is **normal** for shell operations
- The tester allows this variance
- Only larger differences (3+) indicate real leaks

## Best Practices

### Development Workflow
1. **First pass**: Run without leak/FD checks (`n`)
   - Fast iteration
   - Focus on functionality
   
2. **Second pass**: Run with leak/FD checks (`y`)
   - Once tests pass functionally
   - Verify resource management
   
3. **Final validation**: All green
   - ✓ All functional tests pass
   - ✓ No memory leaks
   - ✓ No FD leaks

### CI/CD Integration
```bash
# For CI pipelines
echo "y" | ./minishell_tester.sh

# Check exit code
if [ $? -eq 0 ]; then
    echo "All tests passed!"
else
    echo "Some tests failed"
    exit 1
fi
```

## Troubleshooting

### "valgrind not found"
**Solution**: Install valgrind (see Requirements section)
**Workaround**: Answer `n` to skip leak/FD tests

### Tests hang/timeout
**Cause**: Valgrind makes tests 2-3x slower
**Solution**: 
- Ensure commands complete properly
- Check for infinite loops in heredoc/pipe handling
- Use `timeout` wrapper: `timeout 60 ./minishell_tester.sh`

### Too many false FD leaks
**Cause**: System-dependent FD behavior
**Solution**: The tester already allows ±2 FD variance. If you see many false positives, the threshold may need adjustment for your system.

### Readline leaks still showing
**Cause**: Suppression file not working
**Solution**: 
1. Delete `readline_curses.supp`
2. Run tester again (it will recreate)
3. Verify readline library path matches suppression patterns

## Advanced Usage

### Run Only Leak Tests (no FD)
Edit the `run_test` function to comment out:
```bash
# run_fd_check_inline "$cmd"
```

### Run Only FD Tests (no leaks)
Edit the `run_test` function to comment out:
```bash
# run_leak_check_inline "$cmd"
```

### Custom Valgrind Options
Edit `run_leak_check_inline` function to add options:
```bash
valgrind --leak-check=full \
    --track-origins=yes \        # Add this
    --verbose \                   # Add this
    --show-leak-kinds=definite,indirect \
    ...
```

## Files Created

- `readline_curses.supp` - Valgrind suppression file (auto-created)
- `/tmp/minishell_out.txt` - Minishell output (temp)
- `/tmp/bash_out.txt` - Bash output (temp)
- `/tmp/valgrind_out.txt` - Valgrind results (temp)

All temporary files are cleaned up automatically.

## Summary

**Integrated testing** = One command, comprehensive results

Instead of:
```
./minishell_tester.sh          # Functional
./run_leak_tests.sh            # Leaks
./run_fd_tests.sh              # FDs
```

You get:
```
./minishell_tester.sh          # All three!
Answer: y
```

**Result**: Every passing test is validated for correctness, memory safety, and resource management. 🎯
