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

# List of programs to execute
PROGRAMS=("testBSP")
LENGTH=${#PROGRAMS[@]}

# Check if program list is empty
if (( LENGTH == 0 )); then
    echo "Error: No programs available to execute."
    exit 1
fi

# If only one program, execute it directly
if (( LENGTH == 1 )); then
    PROGRAM="${PROGRAMS[0]}"
else
    # Multiple programs: ask user
    echo "Which program would you like to execute?"
    for i in "${!PROGRAMS[@]}"; do  # "PROGRAM[@]": access all elements of array; '!' returns indices instead of values of array
        echo "($((i + 1))) ${PROGRAMS[$i]}"
    done

    echo -n "Enter the number corresponding to your choice: " #-n no newline
    read -r CHOICE #-r treats everything as normal character: "\n" => "\n" and not newline

    # Validate user input: only numbers between 1 and length
    if ! [[ "$CHOICE" =~ ^[0-9]+$ ]] || (( CHOICE < 1 || CHOICE > LENGTH )); then
        echo "Error: Invalid choice."
        exit 1
    fi

    PROGRAM="${PROGRAMS[$((CHOICE - 1))]}"
fi

# Execute the selected program
PROGRAM="${PROGRAM}"
COMMAND="./generator/AutoBSP ./demo_files/$PROGRAM.absp"
echo -e -n "Executing: $COMMAND\n>" #-e allows \n to be interpreted as newline and not as '\' 'n'
$COMMAND
EXIT_STATUS=$?  # Capture the exit status

if [ $EXIT_STATUS -eq 1 ]; then
  echo "Error: The program exited with status 1. Stopping script."
  exit 1
fi

echo -e "<"

echo -e "\nFinished executing!"
