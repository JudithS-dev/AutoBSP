#!/bin/bash
echo -e "Starting cleanup process...\n"

# Delete AutoBSP.log if it exists
echo "Deleting AutoBSP.log..."
echo "rm -f AutoBSP.log"
rm -f AutoBSP.log
echo ""

# Execute 'make clean' in ./generator
echo "Running 'make clean' in ./generator..."
make -C ./generator clean
echo ""

# Delete ast-files (.gv, .png,)
# Check if the file removePNGandDOT.sh exists and is executable
echo "Removing old .png and .dot files..."
if [ -e "./removePNGandDOT.sh" ]; then
  if [ -x "./removePNGandDOT.sh" ]; then
    echo "Found removePNGandDOT.sh. Executing..."
    echo "./removePNGandDOT.sh"
    ./removePNGandDOT.sh
  else
    echo "Info: removePNGandDOT.sh. is not executable. Making it executable..."
    echo "chmod +x removePNGandDOT.sh"
    chmod +x removePNGandDOT.sh
    echo "Executing removePNGandDOT.sh..."
    echo "./removePNGandDOT.sh"
    ./removePNGandDOT.sh
  fi
else
  echo "removePNGandDOT.sh not found. Skipping..."
fi
echo ""

# Delete createPNGfromDOT.sh and removePNGandDOT.sh
echo "Removing createPNGfromDOT.sh and removePNGandDOT.sh..."
echo "rm -f createPNGfromDOT.sh removePNGandDOT.sh"
rm -f createPNGfromDOT.sh removePNGandDOT.sh

echo -e "\nCleanup completed."
