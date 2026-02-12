#!/bin/bash

# Check for required arguments
if [ $# -ne 1 ]; then
    echo "Usage: $0 <file.(wav|mp3|flac)>"
    exit 1
fi

# Input file and base name
INPUT="$1"
BASENAME=$(basename "$INPUT" | sed 's/\.[^.]*$//')  # Remove the extension
OUTPUT_RAW="/tmp/${BASENAME}.raw"
OUTPUT_C="${BASENAME}.c"
OUTPUT_H="${BASENAME}.h"

# Check if the input file exists
if [ ! -f "$INPUT" ]; then
    echo "Error: File $INPUT does not exist."
    exit 1
fi

# Convert to RAW using SoX
echo "Converting $INPUT to $OUTPUT_RAW..."
sox "$INPUT" -r 16000 -c 1 -e unsigned-integer -b 8 "$OUTPUT_RAW" trim 0 45

# Check if SoX succeeded
if [ $? -ne 0 ]; then
    echo "Error: Conversion with SoX failed. Please ensure SoX supports your input file format."
    exit 1
fi

# Convert RAW data to a C array using xxd
echo "Converting $OUTPUT_RAW to $OUTPUT_C..."
xxd -i "$OUTPUT_RAW" > "$OUTPUT_C"

# remove raw file
rm -f "$OUTPUT_RAW"


# Replace variable names in the .c file
sed -i 's/unsigned char .*_raw\[\]/const uint8_t audio_samples[]/g' "$OUTPUT_C"
sed -i 's/unsigned int .*_raw_len/const unsigned int audio_samples_len/g' "$OUTPUT_C"

# Add array size declaration if missing
if ! grep -q "audio_samples_len" "$OUTPUT_C"; then
    echo -e "\nconst unsigned int audio_samples_len = sizeof(audio_samples);" >> "$OUTPUT_C"
fi

# Create the .h file
echo "Creating $OUTPUT_H..."
cat <<EOF > "$OUTPUT_H"
#pragma one

#include <stdint.h>

extern const uint8_t audio_samples[];
extern const unsigned int audio_samples_len;

#endif // AUDIO_SAMPLES_H
EOF

# Output success message
echo "Files generated:"
echo "  - $OUTPUT_C"
echo "  - $OUTPUT_H"
