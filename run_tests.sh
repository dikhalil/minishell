#!/bin/bash

# Convenience script to run minishell tests from project root
# Simply run: ./run_tests.sh

cd "$(dirname "$0")/tester" && ./minishell_tester.sh
