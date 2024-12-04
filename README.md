# README for CSE 5242 Advanced DBMS Project 2

## How to Compile the Code
Use the provided Makefile to compile the code:
```bash
make
```
Alternatively, you can compile manually:
```bash
gcc -O3 -mavx2 -o db5242 db5242.c
```

## Implemented Functions
**Binary Search Variants**  
- low_bin_search: Standard binary search.
- low_bin_nb_arithmetic: Optimized binary search without branching.
- low_bin_nb_mask: Further optimized with bitwise operations.
- low_bin_nb_4x: Parallel processing of 4 searches in one call.
- low_bin_nb_simd: SIMD-optimized binary search using AVX2.  

**Band Join Variants**  
- band_join: Traditional band join leveraging binary search.
- band_join_simd: SIMD-optimized band join using AVX2.

## How to Run the Program
Uncomment the desired algorithm type in the main function to run by itself. In the submission, only the first options are uncommented, so please uncomment the others based on these instructions.

**Binary Search Profiling**  
For low_bin_search, low_bin_nb_arithmetic, and low_bin_nb_mask only uncomment lines 607-613 to run bulk_bin_search (default).  
Then in bulk_bin_search uncomment the line that corresponds to the implementation being tested.  
For the low_bin_nb_4x and low_bin_nb_simd variants, only uncomment lines 617-623 to run bulk_bin_search_4x.  
Then in bulk_bin_search_4x uncomment the lines corresponding to the implementation being tested.
>For both cases, run the program with:
```bash
./db5242 <array_size> <dummy_X> <dummy_Y> <dummy_Z> <repeats>
```
>Example:
```bash 
./db5242 1000000 1 2 3 1000
```
**Band Join Profiling**  
To test one of the band join functions, uncomment lines 626-634, and comment out the other function.  
>Then run the program with:
```bash
./db5242 <inner_size> <outer_size> <result_size> <bound>
```
>Example:
```bash     
./db5242 10000 100000 1000000 1000
```
## Contributors
- Varun Mangla
- Houssam Djerd