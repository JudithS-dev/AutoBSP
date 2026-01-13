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

# Check if the selected program file exists
if ! [ -f "./demo_files/$PROGRAM" ]; then
  echo "Error: Program file './demo_files/$PROGRAM' not found."
  exit 1
fi
# Execute the selected program
PROGRAM="${PROGRAM}"
OUTPUT="output"

if ! [ -d "$OUTPUT" ]; then
  echo "Output directory '$OUTPUT' does not exist. Creating it..."
  mkdir -p "$OUTPUT"
fi

# If OUTPUT == "output", call command without output argument
if [ "$OUTPUT" == "output" ]; then
  COMMAND="./generator/AutoBSP ./demo_files/$PROGRAM"
else
  COMMAND="./generator/AutoBSP ./demo_files/$PROGRAM $OUTPUT"
fi

echo -e -n "Executing: $COMMAND\n>" #-e allows \n to be interpreted as newline and not as '\' 'n'
$COMMAND
EXIT_STATUS=$?  # Capture the exit status

if [ $EXIT_STATUS -eq 1 ]; then
  echo "Error: The program exited with status 1. Stopping script."
  exit 1
fi

echo -e "<\n"

# Check if the file createPNGfromDOT.sh exists and is executable
echo "Generate PNGs from DOT files..."
if [ -e "./$OUTPUT/createPNGfromDOT.sh" ]; then
  if [ -x "./$OUTPUT/createPNGfromDOT.sh" ]; then
    echo "Found createPNGfromDOT.sh. Executing..."
    echo "./$OUTPUT/createPNGfromDOT.sh"
    ./$OUTPUT/createPNGfromDOT.sh
  else
    echo "Info: createPNGfromDOT.sh. is not executable. Making it executable..."
    echo "chmod +x ./$OUTPUT/createPNGfromDOT.sh"
    chmod +x ./$OUTPUT/createPNGfromDOT.sh
    echo "Executing createPNGfromDOT.sh..."
    echo "./$OUTPUT/createPNGfromDOT.sh"
    ./$OUTPUT/createPNGfromDOT.sh
  fi
else
  echo "createPNGfromDOT.sh not found. Skipping..."
fi

# Ask user to choose if the generated code should be copied into a specific directory
echo "Would you like to copy the generated code into the demo directory?"
echo " (0) No"
echo " (1) Yes, into STM32CubeIDE project directory"
echo -n ": "
read -r COPY_CHOICE

if [ "$COPY_CHOICE" == "1" ]; then
  # Check if project directory exists (BSP_test_STM32/Core/Src/ and BSP_test_STM32/Core/Inc/)
  SRC_DIR="BSP_test_STM32/Core/Src/"
  INC_DIR="BSP_test_STM32/Core/Inc/"
  
  if [ -d "$SRC_DIR" ] && [ -d "$INC_DIR" ]; then
    echo "Copying generated source file to '$SRC_DIR'..."
    cp "$OUTPUT/generated_bsp.c" "$SRC_DIR"
    echo "Copying generated header file to '$INC_DIR'..."
    cp "$OUTPUT/generated_bsp.h" "$INC_DIR"
    echo "Files copied successfully."
  else
    echo "Error: Project directories '$SRC_DIR' or '$INC_DIR' do not exist. Cannot copy files."
  fi
fi

echo -e "\nFinished executing!"
