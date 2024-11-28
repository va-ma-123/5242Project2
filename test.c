#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

// Function to compare two int64_t values for qsort
static int compare(const void *p1, const void *p2) {
    int64_t a, b;
    a = *(int64_t *)p1;
    b = *(int64_t *)p2;
    if (a < b) return -1;
    if (a == b) return 0;
    return 1;
}
inline int64_t low_bin_nb_arithmetic(int64_t* data, int64_t size, int64_t target)
{
  /* this binary search variant
     (a) does no comparisons in the inner loop by using multiplication and addition to convert control dependencies
         to data dependencies
     (b) doesn't require an exact match; instead it returns the index of the first key >= the search key.
         That's good in a DB context where we might be doing a range search, and using binary search to
	 identify the first key in the range.
     (c) If the search key is bigger than all keys, it returns size.
  */
  int64_t left=0;
  int64_t right=size;
  int64_t mid;

  while(left<right) {

  /* YOUR CODE HERE */
    mid = (left + right) / 2;

    // Calculate whether the target is less than or equal to data[mid]
    // Comparison result is 1 if true, 0 if false
    int64_t is_ge = (data[mid] >= target);

    // Update right or left based on is_ge without branching

    right = mid * is_ge + right * (1 - is_ge);  // If is_ge: right = mid; else: right = right
    left = left * is_ge + (mid + 1) * (1 - is_ge);  // If is_ge: left = left; else: left = mid + 1
  }
  return right;
}


inline int64_t low_bin_nb_mask(int64_t* data, int64_t size, int64_t target)
{
  /* this binary search variant
     (a) does no comparisons in the inner loop by using bit masking operations to convert control dependencies
         to data dependencies
     (b) doesn't require an exact match; instead it returns the index of the first key >= the search key.
         That's good in a DB context where we might be doing a range search, and using binary search to
	 identify the first key in the range.
     (c) If the search key is bigger than all keys, it returns size.
  */
  int64_t left=0;
  int64_t right=size;


  /* YOUR CODE HERE */
  int64_t mid;
  while (left < right)
  {
    mid = (left + right)>>1; // Bitwise shift to divide by 2
    int64_t is_ge = -(data[mid] >= target); // Check if target is greater than or equal to data[mid]

    /* 
    here -1 and 0 are used instead of 1 and 0. This is because -1 is represented by all 1s in binary (2's complement)
    in previous function, we use * operator, and multiplying by 1 is how we keep result same as starting number
    but when doing bitwise &, all the bits have to be 1 to get the result to be the same as starting number (left/right) 
    0 is all 0s in binary, so bitwise & with 0 will always result in 0, like in previous function
    */

    right = (right & ~is_ge) | (mid & is_ge); // If is_ge: right = mid, else: right = right
    left = (left & is_ge) | ((mid+1) & ~is_ge); // If is_ge: left = left, else: left = mid + 1
  }
  return right; // Return right
}


inline void low_bin_nb_4x(int64_t* data, int64_t size, int64_t* targets, int64_t* right)
{
  /* this binary search variant
     (a) does no comparisons in the inner loop by using bit masking operations instead
     (b) doesn't require an exact match; instead it returns the index of the first key >= the search key.
         That's good in a DB context where we might be doing a range search, and using binary search to
	 identify the first key in the range.
     (c) If the search key is bigger than all keys, it returns size.
     (d) does 4 searches at the same time in an interleaved fashion, so that an out-of-order processor
         can make progress even when some instructions are still waiting for their operands to be ready.

     Note that we're using the result array as the "right" elements in the search so no need for a return statement
  */

    /* YOUR CODE HERE */

    int64_t left[] = {0,0,0,0};
    int64_t mid[4];
    int tracker = 0b0000; // 1 = completed, 0 = ongoing
    // use 4 bits to track each of the 4 searches' completions
    // more optimal than using another 4 integer array
    while( tracker != 0b1111){
        for(int i=0; i<4; i++){
            if (tracker & (1<<i)){
                continue; // skip completed search
            }
            mid[i] = (right[i] + left[i])>>1;
            int64_t is_ge = -(data[mid[i]] >= targets[i]);

            right[i] = (right[i] & ~is_ge) | (mid[i] & is_ge);
            left[i] = (left[i]& is_ge) | ((mid[i]+1) & ~is_ge); 
            if(left[i]>= right[i]){
                tracker |= (1<<i);
            }
        }
    }
}

/* The following union type is handy to output the contents of AVX512 data types */
union int8x4 {
  __m256i a;
  int64_t b[4];
};

void printavx(char* name, __m256i v) {
  union int8x4 n;

  n.a=v;
  printf("Value in %s is [%ld %ld %ld %ld ]\n",name,n.b[0],n.b[1],n.b[2],n.b[3]);
}

inline void low_bin_nb_simd(int64_t* data, int64_t size, __m256i target, __m256i* result)
{
  /* this binary search variant
     (a) does no comparisons in the inner loop by using bit masking operations instead
     (b) doesn't require an exact match; instead it returns the index of the first key >= the search key.
         That's good in a DB context where we might be doing a range search, and using binary search to
	 identify the first key in the range.
     (c) If the search key is bigger than all keys, it returns size.
     (d) does 4 searches at the same time using AVX2 intrinsics

     See https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-8/intrinsics-for-avx2.html
     for documentation of the AVX512 intrinsics

     Note that we're using the result array as the "right" elements in the search, and that searchkey is being passed
     as an __m256i value rather than via a pointer.
  */


 /* YOUR CODE HERE */
    // use the __m256i data type because intrinsics take these
    __m256i left = _mm256_setzero_si256(); // initialize left to 4 0's
    __m256i tracker = _mm256_set1_epi64x(-1); // initialize tracker to 4 -1's (all 1s in binary), 0 for completed

    while (!_mm256_testz_si256(tracker, tracker)) { // testz is 0 if there is at least one non-zero bit when taking arg1 & arg2
        // calculate mid index for all 4 parallel searches
        __m256i mid = _mm256_add_epi64(left, *result); 
        mid = _mm256_srli_epi64(mid, 1);

        // gather the separate mid indices into one vector
        __m256i gathered_mid = _mm256_i64gather_epi64((const long long *) data, mid, 1); // have to cast pointer to this type

        // compare target and mid
        // cmpgt intrinsic returns 1 when gathered_mid > target
        __m256i is_g = _mm256_cmpgt_epi64(gathered_mid, target);
        // cmpeq intrinsic returns 1 when gathered_mid = target
        __m256i is_e = _mm256_cmpeq_epi64(gathered_mid, target);
        // combine both with bitwise OR operator, to get gathered_mid >= target
        __m256i is_ge = _mm256_or_si256(is_g, is_e);

        // update right depending on is_ge using blend
        // if mask is 0, copy from 1st argument, if mask 1, copy from 2nd argument
        *result = _mm256_blendv_epi8(gathered_mid, *result, is_ge);
        // update left depending on is_ge 
        left = _mm256_blendv_epi8(left, _mm256_add_epi64(mid, _mm256_set1_epi64x(1)), is_ge);

        // update tracker
        __m256i searching = _mm256_cmpgt_epi64(*result, left); // if right > left, search is ongoing
        tracker = _mm256_and_si256(tracker, searching); // if not searching, it will be 0, so tracker will become 0 for that process
    }
}

int64_t band_join(int64_t* inner, int64_t inner_size, int64_t* outer, int64_t outer_size, int64_t* inner_results, int64_t* outer_results, int64_t result_size, int64_t bound)
{
  /* In a band join we want matches within a range of values.  If p is the probe value from the outer table, then all
     reccords in the inner table with a key in the range [p-bound,p+bound] inclusive should be part of the result.

     Results are returned via two arrays. outer_results stores the index of the outer table row that matches, and
     inner_results stores the index of the inner table row that matches.  result_size tells you the size of the
     output array that has been allocated. You should make sure that you don't exceed this size.  If there are
     more results than can fit in the result arrays, then return early with just a prefix of the results in the result
     arrays. The return value of the function should be the number of output results.

  */

    /* YOUR CODE HERE */
    int64_t count = 0;
    // declare these arrays with fixed size 4 outside of the loop
    int64_t lower_bounds[4];
    int64_t upper_bounds[4];
    int64_t left[4];
    int64_t right[4]; 
    for (int64_t i=0; i < outer_size; i += 4) { // iterate 4 at a time
        // get batch size, which is 4, or less if this is the last batch
        int64_t batch_size = (outer_size - i > 4) ? 4 : (outer_size - i);
        
        for (int64_t j=0; j < batch_size; j++) {
            lower_bounds[j] = outer[i+j] - bound;
            upper_bounds[j] = outer[i+j] + bound + 1;
            // the +1 is because both linear search functions look for >= but we want > here
            // >= n+1 is the same as > n
            left[j] = inner_size; // default to the end of the inner array 
            right[j] = inner_size; 
        }

        if(batch_size == 4) {
            low_bin_nb_4x(inner, inner_size, lower_bounds, left);
            low_bin_nb_4x(inner, inner_size, upper_bounds, right);
        } else {
            for( int64_t j = 0; j < batch_size; j++) {
                left[j] = low_bin_nb_mask(inner, inner_size, lower_bounds[j]);
                right[j] = low_bin_nb_mask(inner, inner_size, upper_bounds[j]);
            } 
        }

        // now each left[j] is the index of the first record in inner that is >= lower_bounds[j]
        // and each right[j] is the index of the first record in inner that is > upper_bounds[j]
        for (int64_t j=0; j < batch_size; j++) {
            // add each index that is >= lower bound and <= upper_bound to results
            // this means from left[j] to right[j] - 1
            for (int64_t k=left[j]; k < right[j]; k++) {
                if(count >= result_size) {
                    return count; // exit early
                }
                outer_results[count] = i + j; // index in the outer table is given by i + j
                inner_results[count] = k; // index in the inner table is given by k
                count++;
            }
        }
    }
    return count;
}

int64_t band_join_simd(int64_t* inner, int64_t inner_size, int64_t* outer, int64_t outer_size, int64_t* inner_results, int64_t* outer_results, int64_t result_size, int64_t bound)
{
  /* In a band join we want matches within a range of values.  If p is the probe value from the outer table, then all
     reccords in the inner table with a key in the range [p-bound,p+bound] inclusive should be part of the result.

     Results are returned via two arrays. outer_results stores the index of the outer table row that matches, and
     inner_results stores the index of the inner table row that matches.  result_size tells you the size of the
     output array that has been allocated. You should make sure that you don't exceed this size.  If there are
     more results than can fit in the result arrays, then return early with just a prefix of the results in the result
     arrays. The return value of the function should be the number of output results.

     To do the binary search, you could use the low_bin_nb_simd you just implemented to search for the lower bounds in parallel

     Once you've found the lower bounds, do the following for each of the 4 search keys in turn:
        scan along the sorted inner array, generating outputs for each match, and making sure not to exceed the output array bounds.

     This inner scanning code does not have to use SIMD.
  */

    /* YOUR CODE HERE */
    int64_t count = 0;
    // declare these arrays with fixed size 4 outside of the loop
    int64_t lower_bounds[4];
    int64_t upper_bounds[4];
    int64_t left[4];
    int64_t right[4]; 
    for (int64_t i=0; i < outer_size; i += 4) { // iterate 4 at a time
        // get batch size, which is 4, or less if this is the last batch
        int64_t batch_size = (outer_size - i > 4) ? 4 : (outer_size - i);
        
        for (int64_t j=0; j < batch_size; j++) {
            lower_bounds[j] = outer[i+j] - bound;
            upper_bounds[j] = outer[i+j] + bound + 1;
            // the +1 is because both linear search functions look for >= but we want > here
            // >= n+1 is the same as > n
            left[j] = inner_size;
            right[j] = inner_size;
        }

        if(batch_size == 4) {
            // have to convert the bound arrays to vectors for the low_bin_nb_simd function
            __m256i lower_vector = _mm256_loadu_si256((__m256i*)lower_bounds);
            __m256i upper_vector = _mm256_loadu_si256((__m256i*)upper_bounds);
            __m256i left_vector = _mm256_loadu_si256((__m256i*)left);
            __m256i right_vector = _mm256_loadu_si256((__m256i*)right);

            low_bin_nb_simd(inner, inner_size, lower_vector, &left_vector);
            low_bin_nb_simd(inner, inner_size, upper_vector, &right_vector);

            // store the results back in the left and right arrays
            _mm256_storeu_si256((__m256i*)left, left_vector);
            _mm256_storeu_si256((__m256i*)right, right_vector);
        } else { 
            // no need for vectors, same as band_join
            for( int64_t j = 0; j < batch_size; j++) {
                left[j] = low_bin_nb_mask(inner, inner_size, lower_bounds[j]);
                right[j] = low_bin_nb_mask(inner, inner_size, upper_bounds[j]);
            } 
        }

        for (int64_t j=0; j < batch_size; j++) {
            for (int64_t k=left[j]; k < right[j]; k ++) {
                if(count >= result_size){
                    return count; // exit early
                }
                outer_results[count] = i + j;
                inner_results[count] = k;
                count++;
            }
        }
    } 
    return count;
}


// Main function to test low_bin_nb_arithmetic
int main() {
    int64_t data[] = {1, 3, 5, 7, 9, 11, 13};  // Sample sorted data
    int64_t size = sizeof(data) / sizeof(data[0]);
    int64_t targets[] = {0, 3, 6, 13, 15};  // Sample target values to search
    int64_t num_targets = sizeof(targets) / sizeof(targets[0]);

    printf("Testing low_bin_nb_arithmetic:\n");
    for (int64_t i = 0; i < num_targets; i++) {
        int64_t index = low_bin_nb_arithmetic(data, size, targets[i]);
        printf("Target %ld -> First index >= %ld: %ld\n", targets[i], targets[i], index);
    }

    printf("Testing low_bin_nb_mask:\n");
    for (int64_t i = 0; i < num_targets; i++) {
        int64_t index = low_bin_nb_mask(data, size, targets[i]);
        printf("Target %ld -> First index >= %ld: %ld\n", targets[i], targets[i], index);  
    }

    printf("Testing low_bin_nb_4x:\n");
    int64_t right[4] = {size, size, size, size};  // Initialize results to `size`
    int64_t targets_4x[4] = {0, 6, 13, 15};      // Select 4 targets to test `low_bin_nb_4x`

    low_bin_nb_4x(data, size, targets_4x, right);

    for (int64_t i = 0; i < 4; i++) {
        printf("Target %ld -> First index >= %ld: %ld\n", targets_4x[i], targets_4x[i], right[i]);
    }

    printf("Testing low_bin_nb_simd:\n");
    __m256i target_vector = _mm256_loadu_si256((const __m256i*)targets_4x);
    __m256i result_vector = _mm256_loadu_si256((const __m256i*)right);
    low_bin_nb_simd(data, size, target_vector, &result_vector);
    for (int64_t i = 0; i < 4; i++) {
        printf("Target %ld -> First index >= %ld: %ld\n", targets_4x[i], targets_4x[i], right[i]);
    }

    printf("Testing band_join:\n");
    int64_t inner[] = {10, 25, 36, 49, 53};
    int64_t outer[] = {20, 41, 60};
    int64_t inner_size = 5;
    int64_t outer_size = 3;
    int64_t result_size = 10;
    int64_t bound = 11;

    int64_t inner_results[10];
    int64_t outer_results[10];

    int64_t result_count = band_join(inner, inner_size, outer, outer_size, inner_results, outer_results, result_size, bound);

    printf("Results:\n");
    for (int64_t i = 0; i < result_count; i++) {
        printf("Outer index: %ld, Inner index: %ld\n", outer_results[i], inner_results[i]);
    }

    printf("Testing band_join_simd:\n");
    result_count = band_join_simd(inner, inner_size, outer, outer_size, inner_results, outer_results, result_size, bound);
    printf("Results:\n");
    for (int64_t i = 0; i < result_count; i++) {
        printf("Outer index: %ld, Inner index: %ld\n", outer_results[i], inner_results[i]);
    }
    return 0;
}
