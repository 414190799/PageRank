# PageRank_Multicore_Project
The multicore project about page rank for NYU course 'Multicore Processors: Architecture &amp; Programming' in spring 2020

## Before you compile
You should run the programs on NYU Linux server like: crunchy1.cims.nyu.edu
Before you start compiling the programs as the instructions below, you should load some necessary libraries.
```
module load mpi/openmpi-x86_64
```
```
module load gcc-9.2
```

## Generate data

compile program which helps us generate input data by using on Linux server:
```
g++ -std=c++11 page.cpp generate_data.cpp -o main
```

run the program:
 ```
 ./main 10 4 10 
 ```
 
 The first parameter is total pages, the second is max in links per page, and the third is page counts per chunk
 
 ## Compute page rank 
 
 ### Serial Version
 
 Compile program which calculates page ranks by using:
 ```
 g++ -std=c++11 serial_version.cpp metadata.cpp page.cpp -o serial
 ```
 
 Run the program:
 ```
 ./serial
 ```
 
 ### OpenMP only version
 
Compile OpenMP only version by:
```
g++ -fopenmp -std=c++11 openmp_version.cpp metadata.cpp page.cpp -o openmp
```

Run the program:
```
./openmp 2
```

 ### MPI only version
 
Compile MPI only version by:
```
mpic++ mpi_version.cpp metadata.cpp page.cpp -o mpi
```

Run the program:
```
mpiexec -n 2 ./mpi
```

 ### MPI and OpenMP hybrid version
 
Compile the hybrid version by:
```
mpic++ -fopenmp hybrid.cpp metadata.cpp page.cpp -o hybrid
```

Run the program:
```
mpiexec -n 2 ./hybrid 2
```
