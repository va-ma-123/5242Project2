#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

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
    int tracker = 0b0000;
    while( tracker != 0b1111){
        for(int i =0; i<4; i++){
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

    return 0;
}
