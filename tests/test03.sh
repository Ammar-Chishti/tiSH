#!/bin/bash

TSTD="./variables"

# Test/Success/Failure colors
TESTCOLOR="\033[0;36m"
SUCCESS="\033[0;32m"
FAILURE="\033[0;31m"
NC="\033[0m"

cd /home/achishti/hw3-achishti/tests

echo "Testing tiSH variables"

echo -e "${TESTCOLOR} Test 1: Correct return value when executing built-int function${NC}"
$TSTD/return_val.sh
if [ $? -eq 0 ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi

echo -e "${TESTCOLOR} Test 2: TiSH correctly inherits environment variable${NC}"
$TSTD/inherit_env_variables.sh
if [ $? -eq 0 ] && [ "$(cat stdout_file.txt)" = "achishti " ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi

echo -e "${TESTCOLOR} Test 3: Correctly able to create new variables${NC}"
OUTPUT=$($TSTD/new_variables.sh)
if [ $? -eq 0 ] && [ "$OUTPUT" = "One Two Three " ]; then
    echo -e "${SUCCESS} PASSED!${NC}"
else
    echo -e "${FAILURE} FAILED! ${NC}"
fi

rm *.txt