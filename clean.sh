#!/bin/bash

# Delete AutoBSP.log if it exists
echo "Deleting AutoBSP.log..."
echo "rm -f AutoBSP.log"
rm -f AutoBSP.log
echo ""

# Execute 'make clean' in ./generator
echo "Running 'make clean' in ./generator..."
make -C ./generator clean

echo -e "\nCleanup completed."
