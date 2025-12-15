#!/bin/bash

# Check if rebuilding is necessary
if ! make -C ./generator --question; then
  echo "Changes in generator detected. Rebuilding..."
  if ! make -C ./generator; then
    echo "Error: 'make' failed. Unable to execute 'AutoBSP'."
    exit 1
  else
    echo ""
  fi
  
else
  echo -e "Everything is up to date. No need to rebuild. \n"
fi

# Check if 'AutoBSP' exists, shouldn't be necessary but better save than sorry
if ! [ -x "./generator/AutoBSP" ]; then
  echo "Error: Executable 'AutoBSP' not found. Try running 'make' first."
  exit 1
fi

# Check if the testBSP program file exists
if ! [ -f "./demo_files/testBSP" ]; then
  echo "Error: Program file './demo_files/testBSP' not found."
  exit 1
fi

echo -e -n "valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./generator/AutoBSP ./demo_files/testBSP\n>\n" #-e allows \n to be interpreted as newline and not as '\' 'n'
valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./generator/AutoBSP ./demo_files/testBSP

echo -e "<"

echo -e "\nFinished executing!"