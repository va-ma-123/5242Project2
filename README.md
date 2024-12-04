README for CSE 5242 Advanced DBMS Project 2


Compiler: GCC with support for AVX2.
Environment: Linux or Unix-based OS with SIMD-compatible Intel CPU.

How to Compile the Code
Use the provided Makefile to compile the code:
    make
Alternatively, you can compile manually:
    gcc -O3 -mavx2 -o db5242 db5242.c

Implemented Functions
    Binary Search Variants
        low_bin_search: Standard binary search.
        low_bin_nb_arithmetic: Optimized binary search without branching.
        low_bin_nb_mask: Further optimized with bitwise operations.
        low_bin_nb_4x: Parallel processing of 4 searches in one call.
        low_bin_nb_simd: SIMD-optimized binary search using AVX2.
    Band Join Variants
        band_join: Traditional band join leveraging binary search.
        band_join_simd: SIMD-optimized band join using AVX2.

how to Run the Program
    Binary Search Profiling
    (Uncomment the desired binary search function in bulk_bin_search or bulk_bin_search_4x.)
        Run the program with:
            ./db5242 <array_size> <dummy_X> <dummy_Y> <dummy_Z> <repeats>
        Example:
            ./db5242 1000000 10 10 10 1000
    Band Join Profiling
        Run the program with:
            ./db5242 <inner_size> <outer_size> <result_size> <bound>
        Exapmle:
            ./db5242 1000000 500000 100000 10000