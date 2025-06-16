#!/bin/bash

CERT_DIR="../certs"  # Katalog för in- och utdata

# Lista över certifikatfiler och motsvarande variabelnamn
declare -A certs=(
  ["ca.crt"]="ca_cert"
  ["client.crt"]="client_cert"
  ["client.key"]="client_key"
)

for filename in "${!certs[@]}"; do
  varname="${certs[$filename]}"
  input_file="${CERT_DIR}/${filename}"
  header_file="${CERT_DIR}/${varname}.h"

  echo "Converting $input_file to Linux format ..."

  if command -v dos2unix > /dev/null; then
    dos2unix "$input_file" 2>/dev/null
  else
    sed -i 's/\r$//' "$input_file"
  fi

  echo "Creating $header_file from $input_file ..."

  {
    echo "#ifndef ${varname^^}_H"
    echo "#define ${varname^^}_H"
    echo ""
    echo "static const char ${varname}[] ="

    while IFS= read -r line || [ -n "$line" ]; do
      printf '    "%s\\n"\n' "$line"
    done < "$input_file"

    echo ";"
    echo ""
    echo "#endif // ${varname^^}_H"
  } > "$header_file"
done

echo "All converted certificates saved in: $CERT_DIR"