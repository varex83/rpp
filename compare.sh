#! /bin/bash

make build-all

measure_time() {
    start_time=$(date +%s.%N)
    "$@"
    end_time=$(date +%s.%N)
    elapsed=$(echo "$end_time - $start_time" | bc)
    echo "Time taken: $elapsed seconds"
}

echo "Running MPI..."
measure_time make run-all-mpi

echo "Running OpenMP..."
measure_time make run-all-openmp

echo "Cleaning up..."
make clean

echo "Done!"
