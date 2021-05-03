# MPI

### Ubuntu 20.04

### Install MPI packages
sudo apt-get install mpich libmpich-dev

### Check MPI version
mpiexec --version

### Check how many CPUs can use
cat /proc/cpuifo | grep "physical id" | sort | uniq | wc -l

### Compile
mpicc -o filename filename.c
without setting filename => default filename of exe is a.out

### Run
mpirun -np _ ./filename
_ denotes how many CPUs will be active.

***

## origin_circuit.c
### No parallel computing version

### Compile
mpicc -o origin_circuit origin_circuit.c

### Run 
mpirun -np 1 ./origin_circuit
1 denotes no parallel computing.

---

## circuit.c
### Parallel computing version

### Compile
mpicc -o origin_circuit origin_circuit.c

### Run 
mpirun -np 2 ./origin_circuit
1 denotes no parallel computing.

***

## MPI_Floyd_algo.c
### Parallel computing version: use MPI_Bcast

### Compile
mpicc -o MPI_Floyd_algo MPI_Floyd_algo.c

### Run 
mpirun -np 2 ./MPI_Floyd_algo

### =========================================================================================

