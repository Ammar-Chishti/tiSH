#!/bin/bash

TSTD="./redirection"

# Test/Success/Failure colors
TESTCOLOR="\033[0;36m"
SUCCESS="\033[0;32m"
FAILURE="\033[0;31m"
NC="\033[0m"

cd /home/achishti/hw3-achishti/tests

echo "Testing tiSH redirection capability"

echo -e "${TESTCOLOR} Test 1: Built_in cmd to stdout and empty stderr${NC}"
$TSTD/built_in_stdout.sh
if [ $? -eq 0 ] && [ "$(cat stdout_file.txt)" = "/home/achishti/hw3-achishti/tests" ] && [ "$(cat stderr_file.txt)" = "" ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi

echo -e "${TESTCOLOR} Test 2: external cmd to stdout and empty stderr${NC}"
$TSTD/stdout.sh
if [ $? -eq 0 ] && [ "$(cat stdout_file.txt)" = "2 redirection/stdout.sh" ] && [ "$(cat stderr_file.txt)" = "" ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi

echo -e "${TESTCOLOR} Test 3: stdin to external cmd to stdout and empty stderr${NC}"
$TSTD/stdin_stdout.sh
if [ $? -eq 0 ] && [ "$(cat stdout_file.txt)" = "2" ] && [ "$(cat stderr_file.txt)" = "" ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi

echo -e "${TESTCOLOR} Test 4: external cmd to stdout and stderr${NC}"
$TSTD/stdout_stderr.sh
if [ $? -eq 0 ] && [ "$(cat stdout_file.txt)" = "Hello Stdout" ] && [ "$(cat stderr_file.txt)" = "Hello Stderr" ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi

echo -e "${TESTCOLOR} Test 5: stdin to external cmd to stdout and stderr${NC}"
$TSTD/stdin_stdout_stderr.sh
if [ $? -eq 0 ] && [ "$(cat stdout_file.txt)" = "Hello from Stdin" ] && [ "$(cat stderr_file.txt)" = "Hello Stderr" ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi
rm *.txt