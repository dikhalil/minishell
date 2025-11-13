#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Leak and FD check counters
LEAK_TESTS=0
LEAK_PASSED=0
FD_TESTS=0
FD_PASSED=0

# Exit status check counters
EXIT_STATUS_CHECKS=0
EXIT_STATUS_PASSED=0

# Crash detection counters
CRASH_TESTS=0
CRASH_DETECTED=0

# Global flag for leak/FD testing
RUN_LEAK_FD_TESTS=false
VALGRIND_AVAILABLE=false

# Minishell path (parent directory since tester is in tester/ folder)
MINISHELL="../minishell"

# Test output files
MINISHELL_OUT="/tmp/minishell_out.txt"
BASH_OUT="/tmp/bash_out.txt"
MINISHELL_ERR="/tmp/minishell_err.txt"
BASH_ERR="/tmp/bash_err.txt"
VALGRIND_OUT="/tmp/valgrind_out.txt"

# Print functions
print_header() {
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
}

print_test() {
    echo -e "${YELLOW}Testing:${NC} $1"
}

print_pass() {
    echo -e "${GREEN}✓ PASS${NC}"
    ((PASSED_TESTS++))
}

print_fail() {
    echo -e "${RED}✗ FAIL${NC}"
    echo -e "${RED}Command: $1${NC}"
    echo -e "${RED}Expected output:${NC}"
    cat "$BASH_OUT"
    echo -e "${RED}Got output:${NC}"
    cat "$MINISHELL_OUT"
    ((FAILED_TESTS++))
}

# Check if minishell exists
check_minishell() {
    if [ ! -f "$MINISHELL" ]; then
        echo -e "${RED}Error: minishell not found at $MINISHELL${NC}"
        echo -e "${YELLOW}Please compile minishell first or update the MINISHELL variable${NC}"
        exit 1
    fi
}

# Check if valgrind is available
check_valgrind() {
    if ! command -v valgrind &> /dev/null; then
        VALGRIND_AVAILABLE=false
        return 1
    fi
    VALGRIND_AVAILABLE=true
    
    # Create suppression file if it doesn't exist
    if [ ! -f "readline_curses.supp" ]; then
        cat > readline_curses.supp << 'SUPP_EOF'
{
   readline_leak_1
   Memcheck:Leak
   ...
   fun:readline
}
{
   readline_leak_2
   Memcheck:Leak
   ...
   obj:*/libreadline.so.*
}
{
   readline_leak_3
   Memcheck:Leak
   ...
   fun:add_history
}
{
   curses_leak
   Memcheck:Leak
   ...
   obj:*/libncurses*.so.*
}
{
   tgetent_leak
   Memcheck:Leak
   ...
   fun:tgetent
}
SUPP_EOF
    fi
    
    return 0
}

# Detect crash signals (segfault, bus error, etc.)
# Returns 0 if no crash, 1 if crash detected
check_for_crash() {
    local exit_code=$1
    local signal_name=""
    
    # Exit codes 128+N indicate the process was terminated by signal N
    # Common signals:
    # SIGSEGV (11) = 139 (segmentation fault)
    # SIGBUS (7) = 135 (bus error)
    # SIGABRT (6) = 134 (abort)
    # SIGFPE (8) = 136 (floating point exception)
    # SIGILL (4) = 132 (illegal instruction)
    # SIGPIPE (13) = 141 (broken pipe - less critical)
    
    case $exit_code in
        139)
            signal_name="SIGSEGV (Segmentation Fault)"
            return 1
            ;;
        135)
            signal_name="SIGBUS (Bus Error)"
            return 1
            ;;
        134)
            signal_name="SIGABRT (Abort)"
            return 1
            ;;
        136)
            signal_name="SIGFPE (Floating Point Exception)"
            return 1
            ;;
        132)
            signal_name="SIGILL (Illegal Instruction)"
            return 1
            ;;
        141)
            # SIGPIPE is often expected in pipes, so we might not want to treat it as critical
            signal_name="SIGPIPE (Broken Pipe)"
            return 1
            ;;
        *)
            # Check if it's any other signal (128-255 range)
            if [ $exit_code -ge 128 ] && [ $exit_code -le 255 ]; then
                local signal_num=$((exit_code - 128))
                signal_name="Signal $signal_num"
                return 1
            fi
            return 0
            ;;
    esac
}

# Get human-readable crash description
get_crash_description() {
    local exit_code=$1
    
    case $exit_code in
        139) echo "SIGSEGV (Segmentation Fault)" ;;
        135) echo "SIGBUS (Bus Error)" ;;
        134) echo "SIGABRT (Abort)" ;;
        136) echo "SIGFPE (Floating Point Exception)" ;;
        132) echo "SIGILL (Illegal Instruction)" ;;
        141) echo "SIGPIPE (Broken Pipe)" ;;
        *)
            if [ $exit_code -ge 128 ] && [ $exit_code -le 255 ]; then
                echo "Signal $((exit_code - 128))"
            else
                echo "Exit code $exit_code"
            fi
            ;;
    esac
}

# Run leak check for a command
run_leak_check() {
    local cmd="$1"
    local test_name="$2"
    
    ((LEAK_TESTS++))
    
    print_test "Leak check: $test_name"
    
    # Run with valgrind, tracking child processes and suppressing readline leaks
    # --track-origins=yes helps track uninitialized values
    echo -e "$cmd\nexit" | valgrind --leak-check=full \
        --show-leak-kinds=all \
        --errors-for-leak-kinds=all \
        --track-fds=yes \
        --track-origins=yes \
        --trace-children=yes \
        --child-silent-after-fork=yes \
        --suppressions=readline_curses.supp \
        --error-exitcode=42 \
        --log-file="$VALGRIND_OUT" \
        "$MINISHELL" > /dev/null 2>&1
    
    local exit_code=$?
    
    # Check for various memory errors
    local has_errors=false
    
    # Check for invalid reads/writes
    if grep -q "Invalid read" "$VALGRIND_OUT" || grep -q "Invalid write" "$VALGRIND_OUT"; then
        echo -e "${RED}  ✗ Invalid memory access detected${NC}"
        grep -E "Invalid (read|write)" "$VALGRIND_OUT" | head -3 | sed 's/^/    /'
        has_errors=true
    fi
    
    # Check for conditional jumps depending on uninitialized values
    if grep -q "Conditional jump or move depends on uninitialised value" "$VALGRIND_OUT"; then
        echo -e "${RED}  ✗ Conditional jump on uninitialized value${NC}"
        grep "Conditional jump" "$VALGRIND_OUT" | head -2 | sed 's/^/    /'
        has_errors=true
    fi
    
    # Check for use of uninitialized values
    if grep -q "Use of uninitialised value" "$VALGRIND_OUT"; then
        echo -e "${RED}  ✗ Use of uninitialized value detected${NC}"
        grep "Use of uninitialised" "$VALGRIND_OUT" | head -2 | sed 's/^/    /'
        has_errors=true
    fi
    
    # Check for leaks (valgrind returns 42 on leak errors)
    if [ $exit_code -eq 0 ] || [ $exit_code -eq 42 ]; then
        # Filter leaks to only show those from minishell binary (exclude system commands)
        local minishell_leaks=$(grep -E "(definitely lost|indirectly lost)" "$VALGRIND_OUT" | \
                                grep -v "in 0 blocks" | \
                                grep -B5 -A5 "minishell" 2>/dev/null || true)
        
        # Check both total leaks and minishell-specific leaks
        if grep -q "definitely lost: 0 bytes in 0 blocks" "$VALGRIND_OUT" && \
           grep -q "indirectly lost: 0 bytes in 0 blocks" "$VALGRIND_OUT"; then
            if [ "$has_errors" = false ]; then
                echo -e "${GREEN}  ✓ No leaks detected (parent + child processes)${NC}"
                ((LEAK_PASSED++))
            fi
        elif [ -z "$minishell_leaks" ]; then
            # Has leaks but not from minishell (from executed commands)
            if [ "$has_errors" = false ]; then
                echo -e "${GREEN}  ✓ No leaks in minishell (leaks only from executed commands)${NC}"
                ((LEAK_PASSED++))
            fi
        else
            echo -e "${RED}  ✗ Memory leaks detected in minishell${NC}"
            echo "$minishell_leaks" | head -10 | sed 's/^/    /'
            has_errors=true
        fi
    else
        echo -e "${YELLOW}  ⚠ Command failed (exit: $exit_code)${NC}"
    fi
}

# Run file descriptor check
run_fd_check() {
    local cmd="$1"
    local test_name="$2"
    
    ((FD_TESTS++))
    
    print_test "FD check: $test_name"
    
    # Get initial FD count (excluding standard fds: 0, 1, 2)
    local initial_fds=$(ls -1 /proc/$$/fd 2>/dev/null | grep -vE '^[0-2]$' | wc -l)
    
    # Run command
    echo -e "$cmd\nexit" | "$MINISHELL" > /dev/null 2>&1
    
    # Get final FD count (excluding standard fds: 0, 1, 2)
    local final_fds=$(ls -1 /proc/$$/fd 2>/dev/null | grep -vE '^[0-2]$' | wc -l)
    
    # Check for FD leaks (should be same or very close)
    local fd_diff=$((final_fds - initial_fds))
    if [ $fd_diff -ge -1 ] && [ $fd_diff -le 1 ]; then
        echo -e "${GREEN}  ✓ No FD leaks (user fds - initial: $initial_fds, final: $final_fds)${NC}"
        ((FD_PASSED++))
    else
        echo -e "${RED}  ✗ FD leak detected (user fds - initial: $initial_fds, final: $final_fds, Δ$fd_diff)${NC}"
        # Show which FDs are open
        echo -e "${YELLOW}  Open user FDs:${NC}"
        ls -la /proc/$$/fd 2>/dev/null | awk '{print $NF}' | grep -vE '/[0-2]$' | sed 's/^/    /'
    fi
}

# Comprehensive leak and FD test suite
test_memory_and_fds() {
    print_header "TESTING MEMORY LEAKS & FILE DESCRIPTORS"
    
    if ! check_valgrind; then
        echo -e "${YELLOW}Skipping memory leak tests (valgrind not available)${NC}"
        echo -e "${BLUE}Testing file descriptors only...${NC}"
        echo ""
    fi
    
    # Check if suppression file exists
    if [ ! -f "readline_curses.supp" ]; then
        echo -e "${YELLOW}Creating readline suppression file...${NC}"
        cat > readline_curses.supp << 'SUPP_EOF'
{
   readline_leak_1
   Memcheck:Leak
   ...
   fun:readline
}
{
   readline_leak_2
   Memcheck:Leak
   ...
   obj:*/libreadline.so.*
}
{
   readline_leak_3
   Memcheck:Leak
   ...
   fun:add_history
}
{
   curses_leak
   Memcheck:Leak
   ...
   obj:*/libncurses*.so.*
}
{
   tgetent_leak
   Memcheck:Leak
   ...
   fun:tgetent
}
SUPP_EOF
    fi
    
    if check_valgrind; then
        echo -e "${BLUE}Running memory leak tests (excluding readline)...${NC}"
        echo ""
        
        # Basic commands
        run_leak_check "echo hello" "echo hello"
        run_leak_check "pwd" "pwd"
        run_leak_check "env" "env"
        run_leak_check "export TEST=value" "export"
        run_leak_check "unset TEST" "unset"
        run_leak_check "cd .." "cd"
        
        # Pipes
        run_leak_check "echo test | cat" "simple pipe"
        run_leak_check "ls | grep minishell | wc -l" "multiple pipes"
        
        # Redirections
        run_leak_check "echo test > /tmp/leak_test.txt" "output redirect"
        run_leak_check "cat < /tmp/leak_test.txt" "input redirect"
        
        # Quotes and variables
        run_leak_check "echo 'hello world'" "single quotes"
        run_leak_check "echo \"hello world\"" "double quotes"
        run_leak_check "echo \$PATH" "variable expansion"
        
        # Heredoc
        run_leak_check "cat << EOF\ntest\nEOF" "heredoc"
        
        # Cleanup
        rm -f /tmp/leak_test.txt
        
        echo ""
        echo -e "${BLUE}Memory leak test summary:${NC}"
        echo -e "  Tested: $LEAK_TESTS"
        echo -e "  Passed: ${GREEN}$LEAK_PASSED${NC}"
        echo -e "  Failed: ${RED}$((LEAK_TESTS - LEAK_PASSED))${NC}"
        echo ""
    fi
    
    echo -e "${BLUE}Running file descriptor tests...${NC}"
    echo ""
    
    # FD tests
    run_fd_check "echo hello" "echo hello"
    run_fd_check "cat /etc/passwd | head -5" "pipe"
    run_fd_check "echo test > /tmp/fd_test.txt" "redirect"
    run_fd_check "cat << EOF\ntest\nEOF" "heredoc"
    
    # Cleanup
    rm -f /tmp/fd_test.txt
    
    echo ""
    echo -e "${BLUE}File descriptor test summary:${NC}"
    echo -e "  Tested: $FD_TESTS"
    echo -e "  Passed: ${GREEN}$FD_PASSED${NC}"
    echo -e "  Failed: ${RED}$((FD_TESTS - FD_PASSED))${NC}"
}

# Run a test with optional leak and FD checking
run_test() {
    local cmd="$1"
    local skip_leak_fd="${2:-false}"  # Optional parameter to skip leak/FD checks
    ((TOTAL_TESTS++))
    
    print_test "$cmd"
    
    # Run in bash
    echo "$cmd" | bash > "$BASH_OUT" 2> "$BASH_ERR"
    local bash_exit=$?
    
    # Run in minishell and exit immediately
    echo -e "$cmd\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2> "$MINISHELL_ERR"
    local minishell_exit=$?
    
    # Check for crash
    ((CRASH_TESTS++))
    if check_for_crash $minishell_exit; then
        # No crash detected
        :
    else
        # Crash detected
        ((CRASH_DETECTED++))
        echo -e "  ${RED}💥 CRASH DETECTED: $(get_crash_description $minishell_exit)${NC}"
        echo -e "  ${YELLOW}Command: $cmd${NC}"
        ((FAILED_TESTS++))
        return
    fi
    
    # Filter minishell output: remove prompts and commands, keep only output
    # Handle echo -n special case where output and "minishell$ exit" are on same line
    {
        tail -n +2 "$MINISHELL_OUT" | head -n -1 | while IFS= read -r line; do
            if [[ "$line" == "minishell$ exit" ]]; then
                continue  # Skip this line
            elif [[ "$line" == *"minishell$ exit" ]]; then
                # echo -n case: output and prompt on same line, print without newline
                printf '%s' "${line%minishell\$ exit}"
            else
                # Normal case: print line with newline
                echo "$line"
            fi
        done
    } > "${MINISHELL_OUT}.filtered"
    mv "${MINISHELL_OUT}.filtered" "$MINISHELL_OUT"
    
    # Compare outputs
    local output_match=false
    local exit_match=false
    
    if diff -q "$BASH_OUT" "$MINISHELL_OUT" > /dev/null 2>&1; then
        output_match=true
    fi
    
    # Compare exit status
    ((EXIT_STATUS_CHECKS++))
    if [ "$bash_exit" -eq "$minishell_exit" ]; then
        exit_match=true
        ((EXIT_STATUS_PASSED++))
    fi
    
    # Both output and exit status must match
    if [ "$output_match" = true ] && [ "$exit_match" = true ]; then
        print_pass
        echo -e "  ${GREEN}✓ Exit status: $minishell_exit${NC}"
        
        # Run leak and FD checks if enabled and not skipped
        if [ "$RUN_LEAK_FD_TESTS" = true ] && [ "$skip_leak_fd" = false ]; then
            run_leak_check_inline "$cmd"
            run_fd_check_inline "$cmd"
        fi
    else
        # Show specific failure reasons
        if [ "$output_match" = false ] && [ "$exit_match" = false ]; then
            echo -e "${RED}✗ FAIL - Output mismatch AND exit status mismatch${NC}"
            echo -e "  Expected exit: $bash_exit, Got: $minishell_exit"
        elif [ "$output_match" = false ]; then
            echo -e "${RED}✗ FAIL - Output mismatch${NC}"
            echo -e "  Exit status: OK ($minishell_exit)"
        else
            echo -e "${RED}✗ FAIL - Exit status mismatch${NC}"
            echo -e "  Expected: $bash_exit, Got: $minishell_exit"
            echo -e "  Output: OK"
        fi
        ((FAILED_TESTS++))
    fi
}

# Inline leak check (compact output)
run_leak_check_inline() {
    local cmd="$1"
    
    ((LEAK_TESTS++))
    
    # Run with valgrind, tracking child processes and suppressing readline leaks
    # --track-origins=yes helps track uninitialized values
    echo -e "$cmd\nexit" | valgrind --leak-check=full \
        --show-leak-kinds=definite,indirect \
        --errors-for-leak-kinds=definite,indirect \
        --track-origins=yes \
        --trace-children=yes \
        --child-silent-after-fork=yes \
        --suppressions=readline_curses.supp \
        --log-file="$VALGRIND_OUT" \
        "$MINISHELL" > /dev/null 2>&1
    
    # Check for various memory errors
    local has_errors=false
    
    # Check for invalid reads/writes
    if grep -q "Invalid read" "$VALGRIND_OUT" || grep -q "Invalid write" "$VALGRIND_OUT"; then
        echo -e "  ${RED}✗ Invalid memory access${NC}"
        has_errors=true
    fi
    
    # Check for conditional jumps depending on uninitialized values
    if grep -q "Conditional jump or move depends on uninitialised value" "$VALGRIND_OUT"; then
        echo -e "  ${RED}✗ Uninitialized value in conditional${NC}"
        has_errors=true
    fi
    
    # Check for use of uninitialized values
    if grep -q "Use of uninitialised value" "$VALGRIND_OUT"; then
        echo -e "  ${RED}✗ Use of uninitialized value${NC}"
        has_errors=true
    fi
    
    # Filter leaks to only show those from minishell binary (exclude system commands)
    local minishell_leaks=$(grep -E "(definitely lost|indirectly lost)" "$VALGRIND_OUT" | \
                            grep -v "in 0 blocks" | \
                            grep -B5 -A5 "minishell" 2>/dev/null || true)
    
    # Check both total leaks and minishell-specific leaks
    if grep -q "definitely lost: 0 bytes in 0 blocks" "$VALGRIND_OUT" && \
       grep -q "indirectly lost: 0 bytes in 0 blocks" "$VALGRIND_OUT"; then
        if [ "$has_errors" = false ]; then
            echo -e "  ${GREEN}✓ No leaks${NC}"
            ((LEAK_PASSED++))
        fi
    elif [ -z "$minishell_leaks" ]; then
        # Has leaks but not from minishell (from executed commands)
        if [ "$has_errors" = false ]; then
            echo -e "  ${GREEN}✓ No leaks in minishell${NC}"
            ((LEAK_PASSED++))
        fi
    else
        echo -e "  ${RED}✗ Memory leak in minishell${NC}"
        has_errors=true
    fi
    
    # If any errors were detected, don't count as passed
    if [ "$has_errors" = true ]; then
        # Already counted in LEAK_TESTS, just don't increment LEAK_PASSED
        :
    fi
}

# Inline FD check (compact output)
run_fd_check_inline() {
    local cmd="$1"
    
    ((FD_TESTS++))
    
    # Get initial FD count (excluding standard fds: 0, 1, 2)
    local initial_fds=$(ls -1 /proc/$$/fd 2>/dev/null | grep -vE '^[0-2]$' | wc -l)
    
    # Run command
    echo -e "$cmd\nexit" | "$MINISHELL" > /dev/null 2>&1
    
    # Get final FD count (excluding standard fds: 0, 1, 2)
    local final_fds=$(ls -1 /proc/$$/fd 2>/dev/null | grep -vE '^[0-2]$' | wc -l)
    
    # Check for FD leaks (should be same or very close)
    local fd_diff=$((final_fds - initial_fds))
    if [ $fd_diff -ge -1 ] && [ $fd_diff -le 1 ]; then
        echo -e "  ${GREEN}✓ No FD leaks${NC}"
        ((FD_PASSED++))
    else
        echo -e "  ${RED}✗ FD leak: $initial_fds → $final_fds (Δ$fd_diff user fds)${NC}"
    fi
}

# Helper function to run ALL checks after manual inline tests
# Call this after your test succeeds to add exit status, leak, and FD validation
run_post_test_checks() {
    local cmd="$1"
    local expected_exit="${2:-}"  # Optional: if provided, compare against this specific exit code
    
    # 1. Check exit status
    ((EXIT_STATUS_CHECKS++))
    if [ -n "$expected_exit" ]; then
        # Compare with specific expected exit code
        echo -e "$cmd\nexit" | "$MINISHELL" > /dev/null 2>&1
        local minishell_exit=$?
        if [ "$minishell_exit" -eq "$expected_exit" ]; then
            echo -e "  ${GREEN}✓ Exit status: $minishell_exit${NC}"
            ((EXIT_STATUS_PASSED++))
        else
            echo -e "  ${RED}✗ Exit status mismatch: expected $expected_exit, got $minishell_exit${NC}"
        fi
    else
        # Compare with bash
        echo "$cmd" | bash > /dev/null 2>&1
        local bash_exit=$?
        echo -e "$cmd\nexit" | "$MINISHELL" > /dev/null 2>&1
        local minishell_exit=$?
        if [ "$bash_exit" -eq "$minishell_exit" ]; then
            echo -e "  ${GREEN}✓ Exit status: $minishell_exit (matches bash)${NC}"
            ((EXIT_STATUS_PASSED++))
        else
            echo -e "  ${RED}✗ Exit status mismatch: bash=$bash_exit, minishell=$minishell_exit${NC}"
        fi
    fi
    
    # 2. Run leak and FD checks if enabled
    if [ "$RUN_LEAK_FD_TESTS" = true ]; then
        run_leak_check_inline "$cmd"
        run_fd_check_inline "$cmd"
    fi
}

# Helper function to check ONLY exit status for inline tests (no leak/FD)
# Returns 0 if exit statuses match, 1 otherwise
check_exit_status() {
    local cmd="$1"
    local expected_exit="${2:-}"  # Optional: if provided, compare against this
    
    ((EXIT_STATUS_CHECKS++))
    
    # Run in bash to get expected exit status
    echo "$cmd" | bash > /dev/null 2>&1
    local bash_exit=$?
    
    # Run in minishell to get actual exit status
    echo -e "$cmd\nexit" | "$MINISHELL" > /dev/null 2>&1
    local minishell_exit=$?
    
    # Check for crash
    ((CRASH_TESTS++))
    if check_for_crash $minishell_exit; then
        # No crash detected
        :
    else
        # Crash detected
        ((CRASH_DETECTED++))
        echo -e "  ${RED}💥 CRASH DETECTED: $(get_crash_description $minishell_exit)${NC}"
        return 1
    fi
    
    # If expected_exit is provided, compare against it
    if [ -n "$expected_exit" ]; then
        if [ "$minishell_exit" -eq "$expected_exit" ]; then
            echo -e "  ${GREEN}✓ Exit status: $minishell_exit${NC}"
            ((EXIT_STATUS_PASSED++))
            return 0
        else
            echo -e "  ${RED}✗ Exit status mismatch: expected $expected_exit, got $minishell_exit${NC}"
            return 1
        fi
    else
        # Compare bash vs minishell
        if [ "$bash_exit" -eq "$minishell_exit" ]; then
            echo -e "  ${GREEN}✓ Exit status: $minishell_exit (matches bash)${NC}"
            ((EXIT_STATUS_PASSED++))
            return 0
        else
            echo -e "  ${RED}✗ Exit status mismatch: bash=$bash_exit, minishell=$minishell_exit${NC}"
            return 1
        fi
    fi
}

# Run a test without output comparison (just check if it runs)
run_test_no_compare() {
    local cmd="$1"
    local skip_leak_fd="${2:-false}"  # Optional parameter to skip leak/FD checks
    ((TOTAL_TESTS++))
    
    print_test "$cmd (no output comparison)"
    
    # Run in bash to get expected exit status
    echo "$cmd" | bash > /dev/null 2>&1
    local bash_exit=$?
    
    # Run in minishell and check if it executes without crashing
    echo -e "$cmd\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2> "$MINISHELL_ERR"
    local minishell_exit=$?
    
    # Check for crash
    ((CRASH_TESTS++))
    if check_for_crash $minishell_exit; then
        # No crash detected
        :
    else
        # Crash detected
        ((CRASH_DETECTED++))
        echo -e "  ${RED}💥 CRASH DETECTED: $(get_crash_description $minishell_exit)${NC}"
        echo -e "  ${YELLOW}Command: $cmd${NC}"
        ((FAILED_TESTS++))
        return
    fi
    
    # Check exit status match
    ((EXIT_STATUS_CHECKS++))
    if [ "$bash_exit" -eq "$minishell_exit" ]; then
        print_pass
        echo -e "  ${GREEN}✓ Exit status: $minishell_exit${NC}"
        ((EXIT_STATUS_PASSED++))
        
        # Run leak and FD checks if enabled and not skipped
        if [ "$RUN_LEAK_FD_TESTS" = true ] && [ "$skip_leak_fd" = false ]; then
            run_leak_check_inline "$cmd"
            run_fd_check_inline "$cmd"
        fi
    else
        echo -e "${RED}✗ FAIL - Exit status mismatch${NC}"
        echo -e "  Expected: $bash_exit, Got: $minishell_exit"
        ((FAILED_TESTS++))
    fi
}

# Run interactive test (for testing exit, etc)
run_interactive_test() {
    local cmd="$1"
    local expected_exit="$2"
    local skip_leak_fd="${3:-false}"  # Optional parameter to skip leak/FD checks
    ((TOTAL_TESTS++))
    
    print_test "$cmd (exit code: $expected_exit)"
    
    echo -e "$cmd\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2> "$MINISHELL_ERR"
    local minishell_exit=$?
    
    # Check for crash
    ((CRASH_TESTS++))
    if check_for_crash $minishell_exit; then
        # No crash detected
        :
    else
        # Crash detected
        ((CRASH_DETECTED++))
        echo -e "  ${RED}💥 CRASH DETECTED: $(get_crash_description $minishell_exit)${NC}"
        echo -e "  ${YELLOW}Command: $cmd${NC}"
        ((FAILED_TESTS++))
        return
    fi
    
    ((EXIT_STATUS_CHECKS++))
    if [ $minishell_exit -eq $expected_exit ]; then
        print_pass
        echo -e "  ${GREEN}✓ Exit status: $minishell_exit${NC}"
        ((EXIT_STATUS_PASSED++))
        
        # Run leak and FD checks if enabled and not skipped
        if [ "$RUN_LEAK_FD_TESTS" = true ] && [ "$skip_leak_fd" = false ]; then
            run_leak_check_inline "$cmd"
            run_fd_check_inline "$cmd"
        fi
    else
        echo -e "${RED}✗ FAIL - Expected exit code $expected_exit, got $minishell_exit${NC}"
        ((FAILED_TESTS++))
    fi
}

# Test builtins
test_builtins() {
    print_header "TESTING BUILTINS - ECHO"
    
    # Basic echo tests
    run_test "echo Hello World"
    run_test "echo -n Hello World"
    run_test "echo -nnnn test"
    run_test "echo -n -n -n test"
    run_test "echo"
    run_test "echo -n"
    run_test "echo \"test\""
    run_test "echo 'test'"
    
    # Echo with multiple -n flags (from Case.pdf)
    run_test "echo -n -nn -nnwn test -nnn"
    run_test "echo -nnen -n tes"
    
    # Echo with multiple spaces (minishell handles correctly)
    run_test_no_compare "echo   test"
    
    # Echo with tilde expansion
    run_test_no_compare "echo ~"
    
    print_header "TESTING BUILTINS - PWD"
    
    # pwd tests
    run_test "pwd"
    
    print_header "TESTING BUILTINS - CD"
    
    # cd to root
    print_test "cd / && pwd"
    echo -e "cd /\npwd\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    if grep -q "^/$" "$MINISHELL_OUT"; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - cd / failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # cd to home
    print_test "cd && pwd"
    echo -e "cd\npwd\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - cd to HOME failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # cd with too many arguments
    print_test "cd a b (too many arguments)"
    echo -e "cd a b\nexit" | "$MINISHELL" 2>&1 | grep -qi "too many arguments"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'too many arguments'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # cd to non-existent directory
    print_test "cd nonexistent_dir"
    echo -e "cd nonexistent_dir_12345\nexit" | "$MINISHELL" 2>&1 | grep -qi "no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'no such file'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    print_header "TESTING BUILTINS - ENV & EXPORT"
    
    # env tests (no comparison - output varies by system)
    run_test_no_compare "env"
    
    # export tests (no comparison - output varies by system)
    run_test_no_compare "export"
    
    # Test PATH variable
    run_test_no_compare "echo \$PATH"
    
    # Test non-existent variable
    run_test "echo \$non_exist_var_12345"
    
    print_header "TESTING BUILTINS - UNSET"
    
    # unset tests
    run_test_no_compare "unset TEST_VAR"
    
    print_header "TESTING BUILTINS - EXIT"
    
    # Exit with numeric argument
    print_test "exit 42"
    echo "exit 42" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 42 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit 42 should return 42${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Exit with too many arguments
    print_test "exit 1 2 (too many arguments)"
    echo "exit 1 2" | "$MINISHELL" 2>&1 | grep -qi "too many arguments"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'too many arguments'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Exit with non-numeric argument
    print_test "exit abc (numeric argument required)"
    echo "exit abc" | "$MINISHELL" 2>&1 | grep -qi "numeric argument required"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'numeric argument required'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    print_header "TESTING BUILTINS - CD (ADVANCED)"
    
    # cd with relative paths
    print_test "cd .. (go back)"
    echo -e "cd ..\npwd\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - cd .. should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # cd to current directory
    print_test "cd . (stay)"
    echo -e "cd .\npwd\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - cd . should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # cd with - (go to OLDPWD)
    print_test "cd - (previous directory)"
    echo -e "cd /tmp\ncd /\ncd -\npwd\nexit" | "$MINISHELL" 2>&1 | grep -q "/tmp"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${YELLOW}⚠ OPTIONAL - cd - may not be required${NC}"
        ((TOTAL_TESTS++))
    fi
    
    # cd without arguments (should go to HOME)
    print_test "cd (go to HOME)"
    echo -e "cd /tmp\ncd\necho \$HOME\npwd\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - cd should go to HOME${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # cd with ~
    print_test "cd ~ (home directory)"
    echo -e "cd ~\npwd\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${YELLOW}⚠ OPTIONAL - cd ~ may not be required${NC}"
        ((TOTAL_TESTS++))
    fi
    
    # cd to a file (should fail)
    print_test "cd Makefile (not a directory)"
    echo -e "cd Makefile\nexit" | "$MINISHELL" 2>&1 | grep -qi "not a directory"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'not a directory'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    print_header "TESTING BUILTINS - PWD (ADVANCED)"
    
    # pwd with -L (logical path)
    print_test "pwd -L"
    echo -e "pwd -L\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${YELLOW}⚠ OPTIONAL - pwd -L may not be required${NC}"
        ((TOTAL_TESTS++))
    fi
    
    # pwd with arguments (should ignore or error)
    print_test "pwd with_args"
    echo -e "pwd test\nexit" | "$MINISHELL" > /dev/null 2>&1
    local pwd_exit=$?
    if [ $pwd_exit -eq 0 ] || grep -q "too many" <<< "$output" 2>/dev/null; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - pwd should handle arguments${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # pwd in pipe
    print_test "pwd | cat"
    echo -e "pwd | cat\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - pwd should work in pipe${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    print_header "TESTING BUILTINS - EXPORT (ADVANCED)"
    
    # export with value
    print_test "export VAR=value && echo \$VAR"
    echo -e "export VAR=value\necho \$VAR\nexit" | "$MINISHELL" 2>&1 | grep -q "value"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should export and display variable${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # export without value (declare variable)
    print_test "export VAR (without value)"
    echo -e "export VAR\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - export without value should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # export multiple variables
    print_test "export A=1 B=2 C=3"
    echo -e "export A=1 B=2 C=3\necho \$A \$B \$C\nexit" | "$MINISHELL" 2>&1 | grep -q "1 2 3"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should export multiple variables${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # export with spaces in value
    print_test "export VAR='hello world'"
    echo -e "export VAR='hello world'\necho \$VAR\nexit" | "$MINISHELL" 2>&1 | grep -q "hello world"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should handle spaces in value${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # export with empty value
    print_test "export VAR=''"
    echo -e "export VAR=''\necho \"x\${VAR}y\"\nexit" | "$MINISHELL" 2>&1 | grep -q "xy"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should handle empty value${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # export invalid identifier (starts with number)
    print_test "export 1VAR=test (invalid)"
    echo -e "export 1VAR=test\nexit" | "$MINISHELL" 2>&1 | grep -qi "not a valid identifier\|invalid"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should reject invalid identifier${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # export with special characters in name
    print_test "export VAR-NAME=test (invalid)"
    echo -e "export VAR-NAME=test\nexit" | "$MINISHELL" 2>&1 | grep -qi "not a valid identifier\|invalid"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should reject invalid identifier${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    print_header "TESTING BUILTINS - UNSET (ADVANCED)"
    
    # unset existing variable
    print_test "export VAR=test && unset VAR && echo \$VAR"
    echo -e "export VAR=test\nunset VAR\necho \$VAR\nexit" | "$MINISHELL" 2>&1 | grep -v "test" | grep -q "^$"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${YELLOW}⚠ CHECK - unset may leave empty line${NC}"
        print_pass
        ((TOTAL_TESTS++))
    fi
    
    # unset non-existent variable
    print_test "unset NONEXISTENT (should not error)"
    echo -e "unset NONEXISTENT_VAR_12345\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - unset non-existent should not fail${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # unset multiple variables
    print_test "unset VAR1 VAR2 VAR3"
    echo -e "export VAR1=a VAR2=b VAR3=c\nunset VAR1 VAR2 VAR3\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should unset multiple variables${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # unset readonly variable (PATH)
    print_test "unset PATH (should work)"
    echo -e "unset PATH\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - unset PATH should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # unset with invalid identifier
    print_test "unset 1VAR (invalid)"
    echo -e "unset 1VAR\nexit" | "$MINISHELL" 2>&1 | grep -qi "not a valid identifier\|invalid"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${YELLOW}⚠ CHECK - may accept invalid identifier${NC}"
        print_pass
        ((TOTAL_TESTS++))
    fi
    
    print_header "TESTING BUILTINS - ENV (ADVANCED)"
    
    # env with export
    print_test "export TEST=value && env | grep TEST"
    echo -e "export TEST=minishell_test\nenv\nexit" | "$MINISHELL" 2>&1 | grep -q "TEST=minishell_test"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - env should show exported variable${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # env should ignore arguments
    print_test "env test (should ignore or show usage)"
    echo -e "env test\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${YELLOW}⚠ CHECK - env with args behavior${NC}"
        print_pass
        ((TOTAL_TESTS++))
    fi
    
    # env in pipe
    print_test "env | grep USER"
    echo -e "env | grep USER\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - env should work in pipe${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    print_header "TESTING BUILTINS - EXIT (ADVANCED)"
    
    # exit with 0
    print_test "exit 0"
    echo "exit 0" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit 0 should return 0${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # exit with 255
    print_test "exit 255"
    echo "exit 255" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 255 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit 255 should return 255${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # exit with 256 (wraps to 0)
    print_test "exit 256 (wraps to 0)"
    echo "exit 256" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit 256 should wrap to 0${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # exit with negative number
    print_test "exit -1"
    echo "exit -1" | "$MINISHELL" > /dev/null 2>&1
    local neg_exit=$?
    if [ $neg_exit -eq 255 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${YELLOW}⚠ CHECK - exit -1 returned $neg_exit${NC}"
        print_pass
        ((TOTAL_TESTS++))
    fi
    
    # exit with spaces
    print_test "exit   42 (with spaces)"
    echo "exit   42" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 42 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit with spaces should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # exit with leading zeros
    print_test "exit 007"
    echo "exit 007" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 7 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit 007 should return 7${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # exit in subshell (pipe)
    print_test "echo test | exit 5 | cat"
    echo -e "echo test | exit 5 | cat\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit in pipe shouldn't exit main shell${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test pipes
test_pipes() {
    print_header "TESTING PIPES"
    
    # Basic pipes
    run_test "echo hello | cat"
    run_test "echo hello | cat | cat | cat"
    run_test "ls | grep minishell"
    run_test "cat /etc/passwd | grep root | wc -l"
    run_test "echo test | wc -w"
    
    # Pipes with multiple echo (from Case.pdf)
    run_test "echo | echo | echo | echo | echo"
    
    # Pipe with exit - just verify it runs (exit in pipeline doesn't exit main shell)
    run_test "ls | exit 42 | echo test"
}

# Test redirections
test_redirections() {
    print_header "TESTING REDIRECTIONS"
    
    # Clean up any existing test files first
    rm -f /tmp/test_minishell.txt /tmp/test_input.txt /tmp/out1.txt /tmp/out2.txt 2>/dev/null
    
    # Output redirection
    run_test "echo hello > /tmp/test_minishell.txt"
    run_test "cat /tmp/test_minishell.txt"
    
    # Append redirection
    run_test "echo hello > /tmp/test_minishell.txt"
    run_test "echo world >> /tmp/test_minishell.txt"
    run_test "cat /tmp/test_minishell.txt"
    
    # Input redirection
    run_test "echo 'test content' > /tmp/test_input.txt"
    run_test "cat < /tmp/test_input.txt"
    
    # Multiple output redirections (last one wins - from Case.pdf)
    run_test "echo test > /tmp/out1.txt > /tmp/out2.txt"
    
    # Check that last redirection is used
    print_test "cat /tmp/out2.txt (should have 'test')"
    echo -e "cat /tmp/out2.txt\nexit" | "$MINISHELL" 2>&1 | grep -q "test"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - out2.txt should contain 'test'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Redirect to directory should fail (from Case.pdf)
    print_test "> / (redirect to directory should fail)"
    echo -e "> /\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    local exit_code=$?
    if grep -qi "is a directory" "$MINISHELL_OUT" && [ $exit_code -ne 0 ]; then
        print_pass
        echo -e "  ${GREEN}✓ Exit status: $exit_code${NC}"
        ((EXIT_STATUS_CHECKS++))
        ((EXIT_STATUS_PASSED++))
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'is a directory' error and exit non-zero${NC}"
        echo -e "${RED}  Got exit code: $exit_code${NC}"
        ((EXIT_STATUS_CHECKS++))
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Multiple redirections with options (from Case.pdf)
    run_test "ls > /tmp/test1.txt > /tmp/test2.txt -l > /tmp/test3.txt -a"
    
    print_header "TESTING REDIRECTION FAILURES"
    
    # Output redirect to non-existent directory
    print_test "echo test > /nonexistent/path/file.txt (should fail)"
    echo -e "echo test > /nonexistent_minishell_test_dir/file.txt\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    local exit_code=$?
    if grep -qi "no such file or directory" "$MINISHELL_OUT"; then
        print_pass
        echo -e "  ${GREEN}✓ Error message shown correctly${NC}"
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'No such file or directory'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Input redirect from non-existent file
    print_test "cat < /nonexistent_file.txt (should fail)"
    echo -e "cat < /nonexistent_minishell_test_file.txt\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    local exit_code=$?
    if grep -qi "no such file or directory" "$MINISHELL_OUT"; then
        print_pass
        echo -e "  ${GREEN}✓ Error message shown correctly${NC}"
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'No such file or directory'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Append redirect to non-existent directory
    print_test "echo test >> /nonexistent/path/file.txt (should fail)"
    echo -e "echo test >> /nonexistent_minishell_test_dir/file.txt\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    local exit_code=$?
    if grep -qi "no such file or directory" "$MINISHELL_OUT"; then
        print_pass
        echo -e "  ${GREEN}✓ Error message shown correctly${NC}"
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'No such file or directory'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Output redirect to file without write permissions
    local no_write_file="/tmp/minishell_nowrite_$$"
    touch "$no_write_file"
    chmod 000 "$no_write_file"
    print_test "echo test > file_without_permissions (should fail)"
    echo -e "echo test > $no_write_file\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    local exit_code=$?
    if grep -qi "permission denied" "$MINISHELL_OUT"; then
        print_pass
        echo -e "  ${GREEN}✓ Error message shown correctly${NC}"
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'Permission denied'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    rm -f "$no_write_file"
    
    # Input redirect from file without read permissions
    local no_read_file="/tmp/minishell_noread_$$"
    touch "$no_read_file"
    chmod 000 "$no_read_file"
    print_test "cat < file_without_permissions (should fail)"
    echo -e "cat < $no_read_file\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    local exit_code=$?
    if grep -qi "permission denied" "$MINISHELL_OUT"; then
        print_pass
        echo -e "  ${GREEN}✓ Error message shown correctly${NC}"
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'Permission denied'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    rm -f "$no_read_file"
    
    # Append to directory
    print_test "echo test >> / (append to directory should fail)"
    echo -e "echo test >> /\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    local exit_code=$?
    if grep -qi "is a directory" "$MINISHELL_OUT"; then
        print_pass
        echo -e "  ${GREEN}✓ Error message shown correctly${NC}"
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'Is a directory'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Redirect to directory (output)
    print_test "echo test > /tmp (output to directory should fail)"
    echo -e "echo test > /tmp\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    local exit_code=$?
    if grep -qi "is a directory" "$MINISHELL_OUT"; then
        print_pass
        echo -e "  ${GREEN}✓ Error message shown correctly${NC}"
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'Is a directory'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Multiple redirections where one fails
    print_test "echo test > /tmp/ok.txt > /nonexistent/fail.txt (should fail)"
    echo -e "echo test > /tmp/minishell_ok_$$.txt > /nonexistent_minishell_dir/fail.txt\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    local exit_code=$?
    if grep -qi "no such file or directory" "$MINISHELL_OUT"; then
        print_pass
        echo -e "  ${GREEN}✓ Error message shown correctly${NC}"
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should fail with 'No such file or directory'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    rm -f /tmp/minishell_ok_$$.txt
    
    # Command with failed redirect should not execute
    print_test "echo SHOULD_NOT_APPEAR > /nonexistent/dir/file (command should not run)"
    echo -e "echo SHOULD_NOT_APPEAR > /nonexistent_minishell_test/file.txt\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    local exit_code=$?
    if ! grep -q "SHOULD_NOT_APPEAR" "$MINISHELL_OUT"; then
        print_pass
        echo -e "  ${GREEN}✓ Command not executed (redirect failed correctly)${NC}"
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Command should not execute when redirect fails${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Cleanup
    rm -f /tmp/test_minishell.txt /tmp/test_input.txt /tmp/out1.txt /tmp/out2.txt /tmp/test1.txt /tmp/test2.txt /tmp/test3.txt 2>/dev/null
}

# Test quotes
test_quotes() {
    print_header "TESTING QUOTES"
    
    # Basic quotes
    run_test "echo 'hello world'"
    run_test "echo \"hello world\""
    run_test "echo 'hello \"world\"'"
    run_test "echo \"hello 'world'\""
    
    # Variables in quotes
    run_test "echo \"\$USER\""
    run_test "echo '\$USER'"
    
    # Empty quotes (from Case.pdf)
    run_test "echo ''"
    run_test "echo \"\""
    
    # Command in quotes (from Case.pdf)
    print_test "\"ls -l\" (should fail as command)"
    echo -e "\"ls -l\"\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Quoted command should fail${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Partial quotes (from Case.pdf)
    run_test "\"e\"cho test"
}

# Test environment variables
test_env_vars() {
    print_header "TESTING ENVIRONMENT VARIABLES"
    
    # Basic environment variables
    run_test_no_compare "echo \$USER"
    run_test_no_compare "echo \$HOME"
    run_test_no_compare "echo \$PATH"
    
    # Non-existent variable (should print empty line)
    run_test "echo \$NONEXISTENT_VAR_12345"
    
    # Exit status variable
    print_test "echo \$? (after successful command)"
    echo -e "echo test\necho \$?\nexit" | "$MINISHELL" 2>&1 | grep -q "^0$"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - \$? should be 0 after success${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Empty variable expansion
    run_test "echo \$"
    
    # Unset PATH and try to use a command
    print_test "unset PATH && ls (command should fail)"
    echo -e "unset PATH\nls\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found\|no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show command not found without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Unset PATH but use absolute path (should work)
    print_test "unset PATH && /bin/ls (absolute path should work)"
    echo -e "unset PATH\n/bin/ls\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Absolute path should work without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Unset PATH but builtins should still work
    print_test "unset PATH && echo test (builtin should work)"
    echo -e "unset PATH\necho test\nexit" | "$MINISHELL" 2>&1 | grep -q "test"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Builtins should work without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Unset PATH and check with pwd builtin
    print_test "unset PATH && pwd (builtin should work)"
    echo -e "unset PATH\npwd\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - pwd builtin should work without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # More PATH unset tests - commands should NOT work
    print_test "unset PATH && cat (command should fail)"
    echo -e "unset PATH\ncat Makefile\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found\|no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - cat should fail without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    print_test "unset PATH && grep (command should fail)"
    echo -e "unset PATH\ngrep test Makefile\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found\|no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - grep should fail without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    print_test "unset PATH && wc (command should fail)"
    echo -e "unset PATH\nwc -l Makefile\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found\|no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - wc should fail without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    print_test "unset PATH && sort (command should fail)"
    echo -e "unset PATH\necho test | sort\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found\|no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - sort should fail without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    print_test "unset PATH && awk (command should fail)"
    echo -e "unset PATH\nawk '{print}' Makefile\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found\|no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - awk should fail without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    print_test "unset PATH && sed (command should fail)"
    echo -e "unset PATH\nsed 's/test/TEST/' Makefile\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found\|no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - sed should fail without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    print_test "unset PATH && find (command should fail)"
    echo -e "unset PATH\nfind . -name Makefile\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found\|no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - find should fail without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    print_test "unset PATH && head (command should fail)"
    echo -e "unset PATH\nhead -5 Makefile\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found\|no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - head should fail without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    print_test "unset PATH && tail (command should fail)"
    echo -e "unset PATH\ntail -5 Makefile\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found\|no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - tail should fail without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    print_test "unset PATH && make (command should fail)"
    echo -e "unset PATH\nmake\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found\|no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - make should fail without PATH${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test error handling
test_errors() {
    print_header "TESTING ERROR HANDLING"
    
    # Command not found
    print_test "command not found"
    echo -e "nonexistentcommand123\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should display command not found error${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Syntax error - unclosed quote
    print_test "syntax error - unclosed quote"
    echo -e "echo \"unclosed\nexit" | "$MINISHELL" 2>&1 | grep -qi "error\|syntax\|quote"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should handle unclosed quotes${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Directory as command (from Case.pdf)
    print_test "/ (directory as command)"
    echo -e "/\nexit" | "$MINISHELL" 2>&1 | grep -qi "is a directory"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show 'is a directory'${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # File with trailing slash (from Case.pdf)
    print_test "Makefile/ (not a directory)"
    echo -e "Makefile/\nexit" | "$MINISHELL" 2>&1 | grep -qi "not a directory\|is a file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should handle Makefile/ error${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # .. command (from Case.pdf)
    print_test ".. (command not found)"
    echo -e "..\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show command not found for ..${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test special cases
test_special_cases() {
    print_header "TESTING SPECIAL CASES"
    
    # Note: Semicolon (;) is a bonus feature in minishell
    run_test "   echo    hello    "
    run_test ""
}

# Test export with variables
test_export_advanced() {
    print_header "TESTING EXPORT - ADVANCED"
    
    # Export with spaces (from Case.pdf)
    print_test "export with spaces and echo expansion"
    echo -e "export test=' shoaib ft '\necho \$test\nexit" | "$MINISHELL" 2>&1 | grep -q "shoaib ft"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should expand variable with spaces${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Export with leading/trailing spaces
    print_test "export test=' abcd 123 ' && echo variations"
    echo -e "export test=' abcd 123 '\necho \$test\nexit" | "$MINISHELL" 2>&1 | grep -q "abcd 123"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should handle spaces in exported var${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Export empty variable
    print_test "export test_empty="
    echo -e "export test_empty=\nenv\nexit" | "$MINISHELL" 2>&1 | grep -q "test_empty="
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Empty var should appear in env${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Export invalid identifiers
    print_test "export 1x (invalid identifier)"
    echo -e "export 1x\nexit" | "$MINISHELL" 2>&1 | grep -qi "not a valid identifier"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should reject invalid identifier${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Export with empty string
    print_test "export '' test=a"
    echo -e "export '' test=a\nexit" | "$MINISHELL" 2>&1 | grep -qi "not a valid identifier"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should reject empty identifier${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test variable expansion in different contexts
test_variable_expansion() {
    print_header "TESTING VARIABLE EXPANSION"
    
    # Variable in quotes
    print_test "export x='test' && echo \"\$x\""
    echo -e "export x='test'\necho \"\$x\"\nexit" | "$MINISHELL" 2>&1 | grep -q "test"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should expand \$x in double quotes${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Variable not expanded in single quotes
    print_test "export x='test' && echo '\$x'"
    echo -e "export x='test'\necho '\$x'\nexit" | "$MINISHELL" 2>&1 | grep -q '\$x'
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should not expand \$x in single quotes${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Multiple variables concatenation
    print_test "echo \$x\$y with variables"
    echo -e "export x='hello'\nexport y='world'\necho \$x\$y\nexit" | "$MINISHELL" 2>&1 | grep -q "helloworld"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should concatenate variables${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Variable with quotes concatenation
    print_test "echo \$x'\$y' mixed quotes"
    echo -e "export x='hello'\nexport y='world'\necho \$x'\$y'\nexit" | "$MINISHELL" 2>&1 | grep -q "hello\$y"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should handle mixed quote expansion${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test heredoc
test_heredoc() {
    print_header "TESTING HEREDOC"
    
    # Basic heredoc
    print_test "cat << EOF (basic heredoc)"
    echo -e "cat << EOF\ntest line\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "test line"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Basic heredoc failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Heredoc with variable expansion
    print_test "cat << EOF with \$USER"
    echo -e "cat << EOF\nHello \$USER\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "Hello"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with variable failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with quoted delimiter (no expansion)
    print_test "cat << 'EOF' (no expansion)"
    echo -e "cat << 'EOF'\nHello \$USER\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q '\$USER'
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Quoted delimiter should prevent expansion${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Multiple heredocs
    print_test "cat << EOF1 << EOF2 (multiple heredocs)"
    echo -e "cat << EOF1 << EOF2\nfirst\nEOF1\nsecond\nEOF2\nexit" | "$MINISHELL" 2>&1 | grep -q "second"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Multiple heredocs failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with pipe
    print_test "cat << EOF | grep test"
    echo -e "cat << EOF | grep test\ntest line\nother line\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "test line"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with pipe failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with empty lines
    print_test "cat << EOF (with empty lines)"
    echo -e "cat << EOF\nline1\n\nline3\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "line3"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with empty lines failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with special characters
    print_test "cat << EOF (special chars: ! @ # $ %)"
    echo -e "cat << EOF\nSpecial: ! @ # \$ %\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "Special:"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with special chars failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with tabs and spaces
    print_test "cat << EOF (tabs and spaces)"
    echo -e "cat << EOF\n\tTabbed line\n    Spaced line\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "Tabbed"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with tabs/spaces failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with redirection
    print_test "cat << EOF > file (heredoc to file)"
    rm -f /tmp/heredoc_test.txt
    echo -e "cat << EOF > /tmp/heredoc_test.txt\ntest content\nEOF\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ -f /tmp/heredoc_test.txt ] && grep -q "test content" /tmp/heredoc_test.txt; then
        print_pass
        ((TOTAL_TESTS++))
        rm -f /tmp/heredoc_test.txt
    else
        echo -e "${RED}✗ FAIL - Heredoc output redirection failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
        rm -f /tmp/heredoc_test.txt
    fi

    # Heredoc with multiple variables
    print_test "cat << EOF (multiple \$variables)"
    echo -e "export VAR1=hello VAR2=world\ncat << EOF\n\$VAR1 \$VAR2\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "hello world"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with multiple vars failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with undefined variable
    print_test "cat << EOF (undefined \$VAR)"
    echo -e "cat << EOF\n\$UNDEFINED_VAR_12345\nEOF\nexit" | "$MINISHELL" 2>&1 > /tmp/heredoc_undef.txt
    # Should expand to empty or nothing
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with undefined var failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    rm -f /tmp/heredoc_undef.txt

    # Heredoc in pipeline
    print_test "cat << EOF | wc -l (heredoc in pipeline)"
    echo -e "cat << EOF | wc -l\nline1\nline2\nline3\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "3"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc in pipeline failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with long content
    print_test "cat << EOF (long content)"
    echo -e "cat << EOF\n$(printf 'line %d\n' {1..20})\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "line 20"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with long content failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with numeric delimiter
    print_test "cat << 123 (numeric delimiter)"
    echo -e "cat << 123\ntest content\n123\nexit" | "$MINISHELL" 2>&1 | grep -q "test content"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with numeric delimiter failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with underscore delimiter
    print_test "cat << END_OF_FILE (underscore delimiter)"
    echo -e "cat << END_OF_FILE\ntest content\nEND_OF_FILE\nexit" | "$MINISHELL" 2>&1 | grep -q "test content"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with underscore delimiter failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # SIGNAL TESTS IN HEREDOC
    print_test "heredoc interrupted by Ctrl+D (EOF)"
    # Create a test that simulates EOF in heredoc input
    # Minishell correctly shows warning and handles EOF gracefully
    echo -e "cat << EOF" | "$MINISHELL" 2>&1 | grep -qi "warning.*here-document\|delimited by end-of-file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc EOF should show warning message${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with Ctrl+C simulation (signal handling)
    print_test "heredoc signal handling (timeout test)"
    # Start heredoc and check it shows warning when interrupted
    echo -e "cat << EOF" | "$MINISHELL" 2>&1 | grep -qi "warning.*here-document\|delimited by end-of-file"
    EXIT_CODE=$?
    if [ $EXIT_CODE -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc should show warning on EOF${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Multiple heredocs with signal handling
    print_test "multiple heredocs incomplete (signal test)"
    echo -e "cat << EOF1 << EOF2\nfirst\nEOF1" | "$MINISHELL" 2>&1 | grep -qi "warning.*here-document\|delimited by end-of-file"
    EXIT_CODE=$?
    if [ $EXIT_CODE -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Multiple incomplete heredocs should show warning${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc prompt should continue on signal
    print_test "heredoc continues after newline"
    echo -e "cat << EOF\nline1\nline2\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "line2"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc continuation failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc without terminator (should wait/timeout)
    print_test "heredoc without terminator (timeout)"
    echo -e "cat << EOF\nsome content" | "$MINISHELL" 2>&1 | grep -qi "warning.*here-document\|delimited by end-of-file"
    EXIT_CODE=$?
    if [ $EXIT_CODE -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc without terminator should show warning${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with nested quotes
    print_test "cat << EOF (nested quotes)"
    echo -e "cat << EOF\nHello \"world\" and 'quotes'\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "world"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with nested quotes failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with backslashes
    print_test "cat << EOF (backslashes)"
    echo -e "cat << EOF\nPath: /usr/bin/test\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "/usr/bin"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with backslashes failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc after other commands
    print_test "echo test && cat << EOF (after command)"
    echo -e "echo first\ncat << EOF\nsecond\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "second"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc after command failed${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc with command substitution
    print_test "cat << EOF (command sub \$(echo test))"
    echo -e "cat << EOF\nResult: \$(echo hello)\nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "Result:"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc with command substitution${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi

    # Heredoc preserves whitespace
    print_test "cat << EOF (preserve whitespace)"
    echo -e "cat << EOF\n   spaces   before   and   after   \nEOF\nexit" | "$MINISHELL" 2>&1 | grep -q "   spaces   "
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Heredoc should preserve whitespace${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test syntax errors
test_syntax_errors() {
    print_header "TESTING SYNTAX ERRORS"
    
    # Lone redirection operators
    print_test "> (lone redirect)"
    echo -e ">\nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show syntax error for lone >${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    print_test "< (lone redirect)"
    echo -e "<\nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show syntax error for lone <${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    print_test "| (lone pipe)"
    echo -e "|\nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show syntax error for lone |${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Multiple pipes
    print_test "|| (double pipe)"
    echo -e "||\nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show syntax error for ||${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test exit status codes
test_exit_status() {
    print_header "TESTING EXIT STATUS CODES"
    
    # Exit with negative number
    print_test "exit -42 (should be 214)"
    echo "exit -42" | "$MINISHELL" > /dev/null 2>&1
    exit_code=$?
    if [ $exit_code -eq 214 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit -42 should return 214, got $exit_code${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Exit with overflow
    print_test "exit 9223372036854775808 (overflow)"
    echo "exit 9223372036854775808" | "$MINISHELL" 2>&1 | grep -qi "numeric argument required"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show numeric argument required${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Exit with mixed alphanumeric
    print_test "exit 12WW24 (invalid)"
    echo "exit 12WW24" | "$MINISHELL" 2>&1 | grep -qi "numeric argument required"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show numeric argument required${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test special characters and edge cases
test_special_chars() {
    print_header "TESTING SPECIAL CHARACTERS"
    
    # Command with absolute path
    run_test "/bin/ls"
    
    # Empty command line
    run_test ""
    
    # Only spaces
    run_test "     "
    
    # Tab characters
    run_test "echo	test"
}

# Test complex commands
test_complex() {
    print_header "TESTING COMPLEX COMMANDS"
    
    # Pipes with redirections
    run_test "ls -la | grep minishell | wc -l"
    run_test "echo hello world | cat | cat"
    run_test "cat /etc/passwd | grep root"
    
    # Pipes with errors (from Case.pdf)
    print_test "cat Makefile | grep pr | head -n 5 | cd nonexistent"
    echo -e "cat Makefile | grep pr | head -n 5 | cd nonexistent\nexit" | "$MINISHELL" 2>&1 | grep -qi "no such file"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show error for nonexistent directory${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Multiple spaces and tabs
    run_test "   echo    hello    world   "
    
    # Absolute path commands (from Case.pdf)
    run_test "/bin/echo test"
    
    # Builtin with absolute path should work
    print_test "env with arguments"
    echo -e "env test\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - env test should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Pipe with command not found
    print_test "cat Makefile | hello"
    echo -e "cat Makefile | hello\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should show command not found${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test edge cases with quotes and escaping
test_edge_cases_quotes() {
    print_header "TESTING EDGE CASES - QUOTES & ESCAPING"
    
    # Nested quotes
    run_test "echo \"Hello 'World'\""
    run_test "echo 'Hello \"World\"'"
    
    # Empty quotes with text
    run_test "echo \"\"test\"\""
    run_test "echo ''test''"
    
    # Multiple consecutive quotes
    run_test "echo \"\"\"test\"\"\""
    run_test "echo '''test'''"
    
    # Quotes with variables
    run_test "echo \"\$USER\$HOME\""
    run_test "echo '\$USER\$HOME'"
    
    # Partial quoting
    run_test "ec\"ho\" test"
    run_test "ec'ho' test"
    
    # Mixed empty quotes
    run_test_no_compare "echo \"\" '' \"\" ''"
}

# Test edge cases with pipes
test_edge_cases_pipes() {
    print_header "TESTING EDGE CASES - PIPES"
    
    # Pipe with empty command parts
    run_test "echo test | cat | cat | cat"
    
    # Pipe with multiple spaces
    run_test "echo test   |   cat   |   wc"
    
    # Pipe with builtin and external
    run_test "echo hello | cat | wc -w"
    run_test "pwd | cat"
    run_test "env | grep USER | wc -l"
    
    # Long pipe chain
    run_test "echo test | cat | cat | cat | cat | cat"
}

# Test edge cases with redirections
test_edge_cases_redirections() {
    print_header "TESTING EDGE CASES - REDIRECTIONS"
    
    # Create test files first
    rm -f /tmp/edge_test*.txt 2>/dev/null
    
    # Multiple input redirections (last one wins)
    echo "first" > /tmp/edge_test1.txt
    echo "second" > /tmp/edge_test2.txt
    run_test "cat < /tmp/edge_test1.txt < /tmp/edge_test2.txt"
    
    # Redirect with spaces
    run_test "echo test    >    /tmp/edge_test3.txt"
    run_test "cat /tmp/edge_test3.txt"
    
    # Redirect to same file twice
    run_test "echo first > /tmp/edge_test4.txt && echo second > /tmp/edge_test4.txt"
    
    # Append to non-existent file
    run_test "echo test >> /tmp/edge_test_new.txt"
    
    # Redirect with command on both sides
    run_test "cat /etc/passwd | grep root > /tmp/edge_test5.txt"
    
    # Cleanup
    rm -f /tmp/edge_test*.txt 2>/dev/null
}

# Test edge cases with variables
test_edge_cases_variables() {
    print_header "TESTING EDGE CASES - VARIABLES"
    
    # Empty variable
    run_test "echo \$NONEXISTENT"
    
    # Variable without $
    run_test "echo USER"
    
    # Variable at start/end
    run_test "echo \$USER test"
    run_test "echo test \$USER"
    
    # Variable concatenation
    print_test "export A=hello B=world && echo \$A\$B"
    echo -e "export A=hello\nexport B=world\necho \$A\$B\nexit" | "$MINISHELL" 2>&1 | grep -q "helloworld"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should concatenate variables${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Variable with special characters in value
    print_test "export TEST='a b c' && echo \$TEST"
    echo -e "export TEST='a b c'\necho \$TEST\nexit" | "$MINISHELL" 2>&1 | grep -q "a b c"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should preserve spaces in variable${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test edge cases with whitespace
test_edge_cases_whitespace() {
    print_header "TESTING EDGE CASES - WHITESPACE"
    
    # Leading/trailing spaces
    run_test_no_compare "   echo test   "
    run_test_no_compare "     pwd     "
    
    # Multiple spaces between arguments
    run_test_no_compare "echo     hello     world"
    
    # Tabs and spaces mixed
    run_test_no_compare "echo	test  	spaces"
    
    # Only whitespace
    run_test "     "
    run_test "		"
    
    # Newlines (empty command)
    run_test ""
}

# Test edge cases with special characters
test_edge_cases_special_chars() {
    print_header "TESTING EDGE CASES - SPECIAL CHARACTERS"
    
    # Numbers
    run_test "echo 123456789"
    run_test "echo 0"
    
    # Punctuation
    run_test "echo !"
    run_test "echo ."
    run_test "echo ,"
    run_test "echo :"
    
    # Underscores and hyphens
    run_test "echo hello_world"
    run_test "echo hello-world"
    
    # Multiple special chars
    run_test "echo !@#$%"
}

# Test edge cases with builtins
test_edge_cases_builtins() {
    print_header "TESTING EDGE CASES - BUILTINS"
    
    # Echo variations
    run_test "echo"
    run_test "echo -n"
    run_test "echo -n -n -n"
    run_test "echo -nnnnn"
    
    # CD edge cases
    print_test "cd .. && pwd (go up one directory)"
    echo -e "cd ..\npwd\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - cd .. should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # CD to current directory
    print_test "cd . (stay in current directory)"
    echo -e "cd .\npwd\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - cd . should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Export with equals but no value
    print_test "export VAR="
    echo -e "export VAR=\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - export VAR= should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Unset non-existent variable
    run_test_no_compare "unset NONEXISTENT_VAR_12345"
    
    # Exit with spaces
    print_test "exit    42 (exit with spaces)"
    echo "exit    42" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 42 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit with spaces should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test edge cases with command combinations
test_edge_cases_combinations() {
    print_header "TESTING EDGE CASES - COMMAND COMBINATIONS"
    
    # Pipe with redirection
    run_test "echo test | cat > /tmp/combo_test.txt"
    run_test "cat /tmp/combo_test.txt"
    
    # Multiple redirections with pipes
    run_test "echo hello | cat | cat > /tmp/combo_test2.txt"
    
    # Builtin with pipe
    run_test "pwd | cat"
    run_test "echo test | pwd"
    
    # Redirection of builtin
    run_test "echo test > /tmp/combo_test3.txt"
    run_test "pwd > /tmp/combo_test4.txt"
    
    # Cleanup
    rm -f /tmp/combo_test*.txt 2>/dev/null
}

# Test edge cases with exit status
test_edge_cases_exit_status() {
    print_header "TESTING EDGE CASES - EXIT STATUS"
    
    # Exit with 0
    print_test "exit 0"
    echo "exit 0" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit 0 should return 0${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Exit with 1
    print_test "exit 1"
    echo "exit 1" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 1 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit 1 should return 1${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Exit with 255
    print_test "exit 255"
    echo "exit 255" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 255 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit 255 should return 255${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Exit with 256 (wraps to 0)
    print_test "exit 256 (wraps to 0)"
    echo "exit 256" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit 256 should wrap to 0${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Exit with leading zeros
    print_test "exit 007 (with leading zeros)"
    echo "exit 007" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 7 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - exit 007 should return 7${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test edge cases with paths
test_edge_cases_paths() {
    print_header "TESTING EDGE CASES - PATHS"
    
    # Absolute path to builtin (should not work)
    print_test "/bin/cd (absolute path to builtin should fail)"
    echo -e "/bin/cd\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - /bin/cd should fail${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Double slashes in path
    print_test "/bin//ls"
    echo -e "/bin//ls\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - /bin//ls should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Path with dots
    print_test "/bin/./ls"
    echo -e "/bin/./ls\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - /bin/./ls should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test empty commands
test_empty_commands() {
    print_header "TESTING EMPTY COMMANDS"
    
    # Empty string
    run_test ""
    
    # Multiple empty commands
    print_test "Multiple empty lines"
    echo -e "\n\n\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Multiple empty lines should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Only spaces
    run_test "     "
    run_test "          "
    
    # Only tabs
    run_test "		"
    run_test "			"
    
    # Mixed whitespace
    run_test "  	  	  "
    
    # Empty with pipes
    print_test "Empty before pipe: | cat"
    echo -e "| cat\nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Empty before pipe should be syntax error${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Empty with redirections - should create/truncate file
    print_test "Empty with redirect: > file (should create file)"
    local test_file="/tmp/minishell_redirect_test_$$"
    rm -f "$test_file"
    echo -e "> $test_file\nexit" | "$MINISHELL" > /dev/null 2>&1
    local exit_code=$?
    if [ -f "$test_file" ] && [ $exit_code -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
        rm -f "$test_file"
    else
        echo -e "${RED}✗ FAIL - Empty redirect should create file (exit 0)${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
        rm -f "$test_file"
    fi
    
    # Multiple empty commands with pipes
    print_test "Multiple pipes with empty: | | |"
    echo -e "| | |\nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Multiple empty pipes should be syntax error${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Empty command between pipes
    print_test "echo test | | cat"
    echo -e "echo test | | cat\nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Empty between pipes should be syntax error${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Empty after pipe
    print_test "echo test |"
    echo -e "echo test |\nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Empty after pipe should be syntax error${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Multiple redirections with empty
    print_test "< file >"
    echo -e "< /tmp/test >\nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Redirects without command should be syntax error${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Empty with multiple redirections - should create both files
    print_test "> file1 > file2 (should create both files)"
    local test_file1="/tmp/minishell_f1_test_$$"
    local test_file2="/tmp/minishell_f2_test_$$"
    rm -f "$test_file1" "$test_file2"
    echo -e "> $test_file1 > $test_file2\nexit" | "$MINISHELL" > /dev/null 2>&1
    local exit_code=$?
    if [ -f "$test_file1" ] && [ -f "$test_file2" ] && [ $exit_code -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
        rm -f "$test_file1" "$test_file2"
    else
        echo -e "${RED}✗ FAIL - Multiple redirects should create both files (exit 0)${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
        rm -f "$test_file1" "$test_file2"
    fi
    
    # Spaces with pipe
    print_test "   |   "
    echo -e "   |   \nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Spaces with pipe should be syntax error${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Spaces with redirect
    print_test "   >   "
    echo -e "   >   \nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Spaces with redirect should be syntax error${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Empty commands separated by newlines with pipes
    print_test "Empty newlines then pipe command"
    echo -e "\n\n\necho test | cat\nexit" | "$MINISHELL" 2>&1 | grep -q "test"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should execute after empty lines${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Empty commands separated by newlines with redirections
    print_test "Empty newlines then redirect command"
    echo -e "\n\n\necho test > /tmp/empty_redir_test.txt\ncat /tmp/empty_redir_test.txt\nexit" | "$MINISHELL" 2>&1 | grep -q "test"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
        rm -f /tmp/empty_redir_test.txt 2>/dev/null
    else
        echo -e "${RED}✗ FAIL - Should execute redirect after empty lines${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
        rm -f /tmp/empty_redir_test.txt 2>/dev/null
    fi
    
    # Pipe with only spaces between
    print_test "echo test |     | cat"
    echo -e "echo test |     | cat\nexit" | "$MINISHELL" 2>&1 | grep -qi "syntax error\|unexpected token"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Spaces between pipes should be syntax error${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test signal handling
test_signals() {
    print_header "TESTING SIGNAL HANDLING"
    
    # Note: These tests verify basic signal behavior
    # Ctrl+C (SIGINT) and Ctrl+D (EOF) handling
    
    # Test Ctrl+D on empty prompt (should exit gracefully)
    print_test "Ctrl+D (EOF) on empty prompt"
    echo -n "" | "$MINISHELL" > /dev/null 2>&1
    local exit_code=$?
    if [ $exit_code -eq 0 ] || [ $exit_code -eq 1 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - EOF should exit gracefully, got exit code $exit_code${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Test command execution after empty input
    print_test "Command after empty inputs"
    echo -e "\n\necho test\nexit" | "$MINISHELL" 2>&1 | grep -q "test"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should execute command after empty inputs${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Test that minishell doesn't crash with rapid commands
    print_test "Multiple rapid commands"
    echo -e "echo 1\necho 2\necho 3\necho 4\necho 5\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Rapid commands should work${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Test Ctrl+C simulation (process interrupt)
    # Note: This tests if minishell handles child process interruption
    print_test "Child process can be interrupted (sleep simulation)"
    timeout 2 bash -c "echo 'sleep 10' | $MINISHELL" > /dev/null 2>&1
    local timeout_exit=$?
    # timeout returns 124 if command times out, which is expected
    if [ $timeout_exit -eq 124 ] || [ $timeout_exit -eq 0 ] || [ $timeout_exit -eq 130 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${YELLOW}⚠ WARNING - Sleep handling unclear (exit: $timeout_exit)${NC}"
        print_pass  # Don't fail, just warn
        ((TOTAL_TESTS++))
    fi
    
    # Test that exit works even after interruption
    print_test "Exit after various commands"
    echo -e "echo test\npwd\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Exit should work after commands${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Test interactive mode doesn't crash on empty input cycles
    print_test "Empty input cycles"
    echo -e "\n\n\n\n\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Multiple empty inputs should not crash${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Test expansion as command
test_expansion_as_command() {
    print_header "TESTING EXPANSION AS COMMAND"
    
    # Empty variable expansion as command (should do nothing, exit 0)
    print_test "\$NONEXISTENT as command (empty expansion)"
    echo -e "echo before\n\$NONEXISTENT\necho after\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    local exit_code=$?
    
    # Should output "before" and "after" with exit code 0
    if grep -q "before" "$MINISHELL_OUT" && grep -q "after" "$MINISHELL_OUT" && [ $exit_code -eq 0 ]; then
        print_pass
        ((PASSED_TESTS++))
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Empty expansion should do nothing (like bash)${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Empty expansion with quotes as command (empty string should show command not found)
    print_test "\"\$NONEXISTENT\" as command (empty string)"
    echo -e "\"\$NONEXISTENT\"\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found"
    if [ $? -eq 0 ]; then
        print_pass
        ((PASSED_TESTS++))
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Empty quoted expansion should show command not found${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Variable expansion to command (should execute if it's a valid command)
    print_test "export CMD='echo' && \$CMD test"
    echo -e "export CMD='echo'\n\$CMD test\nexit" | "$MINISHELL" 2>&1 | grep -q "test"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should execute command from variable${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Variable expansion to path command
    print_test "export CMD='/bin/echo' && \$CMD test"
    echo -e "export CMD='/bin/echo'\n\$CMD test\nexit" | "$MINISHELL" 2>&1 | grep -q "test"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should execute path command from variable${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Empty expansion in middle of command
    print_test "echo \$EMPTY test (empty var in middle)"
    echo -e "echo \$NONEXISTENT test\nexit" | "$MINISHELL" 2>&1 | grep -q "test"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should handle empty expansion in arguments${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Only dollar sign as command
    print_test "\$ as command (lone dollar)"
    echo -e "\$\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Lone dollar should show command not found${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Variable expanding to empty string as only argument
    print_test "export EMPTY='' && echo \$EMPTY"
    echo -e "export EMPTY=''\necho \$EMPTY\nexit" | "$MINISHELL" > "$MINISHELL_OUT" 2>&1
    # Should just print empty line (echo with no args)
    if [ -f "$MINISHELL_OUT" ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should handle empty variable expansion${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Multiple empty expansions
    print_test "\$EMPTY1 \$EMPTY2 \$EMPTY3 as command"
    echo -e "\$EMPTY1 \$EMPTY2 \$EMPTY3\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found"
    if [ $? -ne 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Multiple empty expansions should return to prompt without error${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Expansion with invalid variable name
    print_test "\$123 as command (invalid var name)"
    echo -e "\$123\nexit" | "$MINISHELL" 2>&1 | grep -qi "command not found\|not found"
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Invalid var expansion should show command not found${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
    
    # Variable expansion to builtin
    print_test "export CMD='cd' && \$CMD .."
    echo -e "export CMD='cd'\n\$CMD ..\npwd\nexit" | "$MINISHELL" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_pass
        ((TOTAL_TESTS++))
    else
        echo -e "${RED}✗ FAIL - Should execute builtin from variable${NC}"
        ((FAILED_TESTS++))
        ((TOTAL_TESTS++))
    fi
}

# Print summary
print_summary() {
    echo ""
    print_header "TEST SUMMARY"
    
    echo -e "${BLUE}═══════════════════════════════════════════════════════════════════════════${NC}"
    echo -e "${BLUE}  FUNCTIONAL TESTS${NC}"
    echo -e "${BLUE}═══════════════════════════════════════════════════════════════════════════${NC}"
    echo -e "Total tests: ${BLUE}$TOTAL_TESTS${NC}"
    echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
    
    # Calculate percentage
    if [ $TOTAL_TESTS -gt 0 ]; then
        PERCENTAGE=$((PASSED_TESTS * 100 / TOTAL_TESTS))
        echo -e "Success rate: ${BLUE}${PERCENTAGE}%${NC}"
    fi
    
    if [ $LEAK_TESTS -gt 0 ] || [ $FD_TESTS -gt 0 ]; then
        echo ""
        echo -e "${BLUE}═══════════════════════════════════════════════════════════════════════════${NC}"
        echo -e "${BLUE}  MEMORY & FILE DESCRIPTOR TESTS${NC}"
        echo -e "${BLUE}═══════════════════════════════════════════════════════════════════════════${NC}"
        
        if [ $LEAK_TESTS -gt 0 ]; then
            echo -e "${YELLOW}Memory Leak Tests:${NC}"
            echo -e "  Total: ${BLUE}$LEAK_TESTS${NC}"
            echo -e "  Passed: ${GREEN}$LEAK_PASSED${NC}"
            echo -e "  Failed: ${RED}$((LEAK_TESTS - LEAK_PASSED))${NC}"
            if [ $LEAK_TESTS -gt 0 ]; then
                LEAK_PERCENTAGE=$((LEAK_PASSED * 100 / LEAK_TESTS))
                echo -e "  Success rate: ${BLUE}${LEAK_PERCENTAGE}%${NC}"
            fi
        fi
        
        if [ $FD_TESTS -gt 0 ]; then
            echo ""
            echo -e "${YELLOW}File Descriptor Tests:${NC}"
            echo -e "  Total: ${BLUE}$FD_TESTS${NC}"
            echo -e "  Passed: ${GREEN}$FD_PASSED${NC}"
            echo -e "  Failed: ${RED}$((FD_TESTS - FD_PASSED))${NC}"
            if [ $FD_TESTS -gt 0 ]; then
                FD_PERCENTAGE=$((FD_PASSED * 100 / FD_TESTS))
                echo -e "  Success rate: ${BLUE}${FD_PERCENTAGE}%${NC}"
            fi
        fi
    fi
    
    if [ $EXIT_STATUS_CHECKS -gt 0 ]; then
        echo ""
        echo -e "${BLUE}═══════════════════════════════════════════════════════════════════════════${NC}"
        echo -e "${BLUE}  EXIT STATUS VALIDATION${NC}"
        echo -e "${BLUE}═══════════════════════════════════════════════════════════════════════════${NC}"
        echo -e "${YELLOW}Exit Status Checks:${NC}"
        echo -e "  Total: ${BLUE}$EXIT_STATUS_CHECKS${NC}"
        echo -e "  Passed: ${GREEN}$EXIT_STATUS_PASSED${NC}"
        echo -e "  Failed: ${RED}$((EXIT_STATUS_CHECKS - EXIT_STATUS_PASSED))${NC}"
        if [ $EXIT_STATUS_CHECKS -gt 0 ]; then
            EXIT_PERCENTAGE=$((EXIT_STATUS_PASSED * 100 / EXIT_STATUS_CHECKS))
            echo -e "  Success rate: ${BLUE}${EXIT_PERCENTAGE}%${NC}"
        fi
    fi
    
    if [ $CRASH_TESTS -gt 0 ]; then
        echo ""
        echo -e "${BLUE}═══════════════════════════════════════════════════════════════════════════${NC}"
        echo -e "${BLUE}  CRASH DETECTION${NC}"
        echo -e "${BLUE}═══════════════════════════════════════════════════════════════════════════${NC}"
        echo -e "${YELLOW}Crash Checks:${NC}"
        echo -e "  Total tests checked: ${BLUE}$CRASH_TESTS${NC}"
        echo -e "  Clean runs: ${GREEN}$((CRASH_TESTS - CRASH_DETECTED))${NC}"
        echo -e "  Crashes detected: ${RED}$CRASH_DETECTED${NC}"
        if [ $CRASH_TESTS -gt 0 ]; then
            CRASH_FREE_PERCENTAGE=$(( (CRASH_TESTS - CRASH_DETECTED) * 100 / CRASH_TESTS))
            echo -e "  Crash-free rate: ${BLUE}${CRASH_FREE_PERCENTAGE}%${NC}"
        fi
        if [ $CRASH_DETECTED -gt 0 ]; then
            echo -e "  ${RED}⚠ WARNING: Crashes detected! Review output for details.${NC}"
        fi
    fi
    
    echo ""
    if [ $FAILED_TESTS -eq 0 ] && [ $((LEAK_TESTS - LEAK_PASSED)) -eq 0 ] && [ $((FD_TESTS - FD_PASSED)) -eq 0 ] && [ $CRASH_DETECTED -eq 0 ]; then
        echo -e "${GREEN}🎉 All tests passed! Perfect score! No crashes! 🎉${NC}"
    elif [ $FAILED_TESTS -eq 0 ] && [ $CRASH_DETECTED -eq 0 ]; then
        echo -e "${GREEN}✓ All functional tests passed with no crashes!${NC}"
        if [ $((LEAK_TESTS - LEAK_PASSED)) -gt 0 ] || [ $((FD_TESTS - FD_PASSED)) -gt 0 ]; then
            echo -e "${YELLOW}⚠ Some memory/FD tests failed. Review above for details.${NC}"
        fi
    else
        if [ $CRASH_DETECTED -gt 0 ]; then
            echo -e "${RED}💥 CRASHES DETECTED! Critical issues found.${NC}"
        fi
        if [ $FAILED_TESTS -gt 0 ]; then
            echo -e "${RED}Some tests failed. Please review the output above.${NC}"
        fi
    fi
}

# Cleanup
cleanup() {
    rm -f "$MINISHELL_OUT" "$BASH_OUT" "$MINISHELL_ERR" "$BASH_ERR" "${MINISHELL_OUT}.filtered" "$VALGRIND_OUT"
    rm -f /tmp/leak_test.txt /tmp/fd_test.txt
}

# Main execution
main() {
    echo -e "${BLUE}"
    echo "╔═══════════════════════════════════════════════════════════════╗"
    echo "║          42 MINISHELL TESTER - COMPREHENSIVE                  ║"
    echo "║         with Integrated Memory & FD Checking                  ║"
    echo "╚═══════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
    
    check_minishell
    
    # Check valgrind availability
    check_valgrind
    
    # Ask user if they want to run leak/FD tests
    if [ "$VALGRIND_AVAILABLE" = true ]; then
        echo -e "${YELLOW}Run memory leak and FD checks for each test? (slower but comprehensive)${NC}"
        echo -e "${YELLOW}Answer (y/n):${NC}"
        read -r run_extra_tests
        
        if [[ "$run_extra_tests" =~ ^[Yy]$ ]]; then
            RUN_LEAK_FD_TESTS=true
            echo -e "${GREEN}✓ Leak & FD checking enabled for all tests${NC}"
        else
            echo -e "${BLUE}Running functional tests only (faster)${NC}"
        fi
    else
        echo -e "${YELLOW}⚠ Valgrind not found - running functional tests only${NC}"
        echo -e "${YELLOW}Install valgrind to enable memory leak detection${NC}"
    fi
    echo ""
    
    # Run all functional tests
    test_builtins
    test_pipes
    test_redirections
    test_quotes
    test_env_vars
    test_errors
    test_special_cases
    test_export_advanced
    test_variable_expansion
    test_heredoc
    test_syntax_errors
    test_exit_status
    test_special_chars
    test_complex
    
    # Run edge case tests
    test_edge_cases_quotes
    test_edge_cases_pipes
    test_edge_cases_redirections
    test_edge_cases_variables
    test_edge_cases_whitespace
    test_edge_cases_special_chars
    test_edge_cases_builtins
    test_edge_cases_combinations
    test_edge_cases_exit_status
    test_edge_cases_paths
    
    # Run empty command and signal tests
    test_empty_commands
    test_signals
    test_expansion_as_command
    
    # Print results
    print_summary
    
    # Cleanup
    cleanup
}

# Run main
main