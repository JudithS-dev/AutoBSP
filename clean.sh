#!/bin/bash
OUTPUT="output"

echo -e "Starting cleanup process...\n"

# Delete AutoBSP.log if it exists
echo "Deleting AutoBSP.log..."
echo "rm -f $OUTPUT/AutoBSP.log"
rm -f $OUTPUT/AutoBSP.log
echo ""

# Execute 'make clean' in ./generator
echo "Running 'make clean' in ./generator..."
make -C ./generator clean
echo ""

# Delete ast-files (.gv, .png,)
# Check if the file removePNGandDOT.sh exists and is executable
echo "Removing old .png and .dot files..."
if [ -e "./$OUTPUT/removePNGandDOT.sh" ]; then
  if [ -x "./$OUTPUT/removePNGandDOT.sh" ]; then
    echo "Found removePNGandDOT.sh. Executing..."
    echo "./$OUTPUT/removePNGandDOT.sh"
    ./$OUTPUT/removePNGandDOT.sh
  else
    echo "Info: removePNGandDOT.sh. is not executable. Making it executable..."
    echo "chmod +x ./$OUTPUT/removePNGandDOT.sh"
    chmod +x ./$OUTPUT/removePNGandDOT.sh
    echo "Executing removePNGandDOT.sh..."
    echo "./$OUTPUT/removePNGandDOT.sh"
    ./$OUTPUT/removePNGandDOT.sh
  fi
else
  echo "removePNGandDOT.sh not found. Skipping..."
fi
echo ""

# Delete createPNGfromDOT.sh and removePNGandDOT.sh
echo "Removing createPNGfromDOT.sh and removePNGandDOT.sh..."
echo "rm -f $OUTPUT/createPNGfromDOT.sh $OUTPUT/removePNGandDOT.sh"
rm -f $OUTPUT/createPNGfromDOT.sh $OUTPUT/removePNGandDOT.sh
echo ""

# Delete generated bsp files if they exist
if [ -f "$OUTPUT/generated_bsp.c" ] || [ -f "$OUTPUT/generated_bsp.h" ]; then
  echo "Deleting generated bsp files..."
  echo "rm -f $OUTPUT/generated_bsp.c $OUTPUT/generated_bsp.h"
  rm -f $OUTPUT/generated_bsp.c $OUTPUT/generated_bsp.h
else
  echo "No generated bsp files found. Skipping..."
fi

echo -e "\nCleanup completed."
