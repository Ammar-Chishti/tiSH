#!/bin/bash

TSTD="./built_in_cmds"

# Test/Success/Failure colors
TESTCOLOR="\033[0;36m"
SUCCESS="\033[0;32m"
FAILURE="\033[0;31m"
NC="\033[0m"

cd /home/achishti/hw3-achishti/tests

echo "Testing built-in tiSH functions"

echo -e "${TESTCOLOR} Test 1: tiSH cd test${NC}"
OUTPUT=$($TSTD/tish_cd.sh)
if [ $? -eq 0 ] && [ "$OUTPUT" = "4 tish_cd.sh" ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi

echo -e "${TESTCOLOR} Test 2: tiSH echo test${NC}"
OUTPUT=$($TSTD/tish_echo.sh)
if [ $? -eq 0 ] && [ "$OUTPUT" = "Hello CSE-376 " ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi

echo -e "${TESTCOLOR} Test 3: tiSH pwd test${NC}"
OUTPUT=$($TSTD/tish_pwd.sh)
if [ $? -eq 0 ] && [ "$OUTPUT" = "/home/achishti/hw3-achishti/tests" ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi

echo -e "${TESTCOLOR} Test 4: tiSH exit test${NC}"
OUTPUT=$($TSTD/tish_exit.sh)
if [ $? -eq 0 ] && [ "$OUTPUT" = "" ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi