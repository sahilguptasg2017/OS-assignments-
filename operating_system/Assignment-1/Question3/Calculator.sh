#!/bin/bash

out_directory="/mnt/c/Sahil/Python/Sahil/operating_system/Assignment-1/Question3/Result"

out_file="$out_directory/output.txt"

mkdir -p "$out_directory"


while IFS= read -r line || [ -n "$line" ]; do
    if [ -n "$line" ]; then
        x=$(echo "$line" | awk '{print $1}')
        y=$(echo "$line" | awk '{print $2}')
        operation=$(echo "$line" | awk '{print $3}')
        echo "$operation"
        if [ "$operation" = "xor" ]; then
            result=$((y^x))
            echo "$result" >> "$out_file"
        elif [ "$operation" = "product" ]; then
            result=$((x * y))
            echo "$result" >> "$out_file"
        else
            if [ "$x" -gt "$y" ]; then
                echo "$x" >> "$out_file
            else 
                echo "$y" >> "$out_file"
            fi
        fi
    fi
done < "/mnt/c/Sahil/Python/Sahil/operating_system/Assignment-1/Question3/input.txt"

#working 