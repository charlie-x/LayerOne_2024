#!/bin/bash

# Navigate to the script directory
cd "$(dirname "$0")"

# Loop through all .png files in the current directory
for png in *.png; do
  # Remove the file extension to get the base name
  base_name="${png%.png}"

  # Define the output header file
  header_file="${base_name}.h"

  # Call the imageConvert tool
  ../../tools/imageConvert "$png" "$header_file" "$base_name"
done

