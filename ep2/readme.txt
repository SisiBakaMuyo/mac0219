Como rodar o paranauê:

mpirun -np 4 ./mandelbrot_mpi_omp -2.5 1.5 -2.0 2.0 8000

mpirun -np 4 ./mandelbrot_mpi -2.5 1.5 -2.0 2.0 8000

o 4 é o número de threads

Full Picture: -2.5 1.5 -2.0 2.0 8000
Seahorse Valley: -0.8 -0.7 0.05 0.15 8000
Elephant Valley: 0.175 0.375 -0.1 0.1 8000
Triple Spiral Valley: -0.188 -0.012 0.554 0.754 8000
