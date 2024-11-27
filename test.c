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

// The function to test
inline int64_t low_bin_nb_arithmetic(int64_t* data, int64_t size, int64_t target) {
    int64_t left = 0;
    int64_t right = size-1;

    while (left < right) {
        int64_t mid = (left + right) / 2;

        // Calculate whether the target is less than or equal to data[mid]
        int64_t is_ge = (data[mid] >= target);

        // Update right or left based on is_ge without branching

        right = mid * is_ge + right * (1 - is_ge);  // If is_ge: right = mid
        left = left * is_ge + (mid + 1) * (1 - is_ge);
    }
	int64_t is_greater = (data[right] >= target);
    return right * (is_greater) + (-1) * (1-is_greater);
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


  return right;
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

    return 0;
}
