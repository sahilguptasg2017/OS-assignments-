#!/bin/bash
out_directory="/home/sahilg/Desktop/OS-assignments-/operating_system/Assignment-1/Question3/Result"

out_file="$out_directory/output.txt"

mkdir -p "$out_directory"

> "$out_file"
#echo "$out_file"
while IFS= read -r line || [ -n "$line" ]; do
    if [ -n "$line" ]; then
        x=$(echo "$line" | awk '{print $1}')
        y=$(echo "$line" | awk '{print $2}')
        operation=$(echo "$line" | awk '{print $3}')
        #echo "$operation"
        if [ "$operation" = "xor" ]; then       
            result=$((y^x))
            echo "Result of xor $x $y : $result" >> "$out_file"
        elif [ "$operation" = "product" ]; then
            result=$((x * y))
            echo "Result of product $x $y : $result" >> "$out_file"
        else
            if [ "$x" -gt "$y" ]; then
                echo "Result of compare $x $y : $x" >> "$out_file"
            else 
                echo "Result of compare $x $y : $y" >> "$out_file"
            fi
        fi
    fi
done < "/home/sahilg/Desktop/OS-assignments-/operating_system/Assignment-1/Question3/input.txt"
