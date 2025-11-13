#!/bin/bash
# Quick test of leak/FD integration

echo "Testing just echo command with leak/FD checks..."
echo "y" | timeout 30 bash minishell_tester.sh 2>&1 | head -40
