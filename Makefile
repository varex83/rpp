init:
	if [ ! -d "out" ]; then mkdir out; fi

build-mpi:
	mpic++ -o out/mpi mpi.cpp

run-mpi:
	mpirun -np 4 out/mpi

build-openmp:
	g++-14 -fopenmp -o out/openmp openmp.cpp

run-openmp:
	./out/openmp

clean:
	rm -rf out/*

run-all:
	make build-all
	make build-openmp
	make run-openmp
	make build-mpi
	make run-mpi
	make clean

run-all-mpi:
	make init
	make build-mpi
	make run-mpi
	make clean

run-all-openmp:
	make init
	make build-openmp
	make run-openmp
	make clean

build-all:
	make init
	make build-mpi
	make build-openmp

compare:
	./compare.sh
