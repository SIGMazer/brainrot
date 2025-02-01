#!/bin/bash

for f in test_cases/*.brainrot; do
    echo "Running Valgrind on $f..."
    base=$(basename "$f" .brainrot)
    if [[ "$base" == slorp_int ]]; then
        echo 42 | valgrind --leak-check=full --error-exitcode=1 ./brainrot "$f"
    elif [[ "$base" == slorp_short ]]; then
        echo 69 | valgrind --leak-check=full --error-exitcode=1 ./brainrot "$f"
    elif [[ "$base" == slorp_float ]]; then
        echo "3.14" | valgrind --leak-check=full --error-exitcode=1 ./brainrot "$f"
    elif [[ "$base" == slorp_double ]]; then
        echo "3.141592" | valgrind --leak-check=full --error-exitcode=1 ./brainrot "$f"
    elif [[ "$base" == slorp_char ]]; then
        echo "c" | valgrind --leak-check=full --error-exitcode=1 ./brainrot "$f"
    elif [[ "$base" == slorp_string ]]; then
        echo "skibidi bop bop yes yes" | valgrind --leak-check=full --error-exitcode=1 ./brainrot "$f"
    else
        valgrind --leak-check=full --error-exitcode=1 ./brainrot "$f"
    fi
    echo
done