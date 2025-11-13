#!/bin/bash
# Quick test of leak checking functionality

echo "Testing leak check on simple command..."
echo -e "echo hello\nexit" | valgrind --leak-check=full \
    --show-leak-kinds=all \
    --errors-for-leak-kinds=all \
    --suppressions=readline_curses.supp \
    --error-exitcode=42 \
    --log-file=/tmp/test_valgrind.txt \
    ./minishell > /dev/null 2>&1

echo ""
echo "Valgrind output:"
cat /tmp/test_valgrind.txt | grep -A 5 "LEAK SUMMARY"
echo ""
echo "Definitely lost:"
grep "definitely lost" /tmp/test_valgrind.txt
echo ""
rm -f /tmp/test_valgrind.txt
