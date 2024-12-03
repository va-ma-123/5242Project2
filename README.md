README for CSE 5242 Advanced DBMS Project 2


Compiler: GCC with support for AVX2.
Environment: Linux or Unix-based OS with SIMD-compatible Intel CPU.
Libraries: Standard C libraries, including <immintrin.h> for AVX2 intrinsics.
How to Compile the Code
Use the provided Makefile to compile the code:
    make
Alternatively, you can compile manually:
    gcc -O3 -mavx2 -o db5242 db5242.c

how to Run the Program
The program accepts the following command-line arguments:
    ./db5242 <inner_size> <outer_size> <result_size> <bound> [repeats]

inner_size: Number of elements in the inner table.
outer_size: Number of elements in the outer table.
result_size: Size of the output result buffer.
bound: Band join range.
repeats: (Optional) Number of repetitions for performance profiling.
Example:
    ./db5242 1000000 500000 100000 10000 10