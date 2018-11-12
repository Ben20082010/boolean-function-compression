# Reduce Truth Table to Compressed Binary Decision Tree

This was one of my coursework at Imperial College London in my first year (spring 2017).

In this coursework, I discover that the table can be converted to a POS (Product of Sum) expression.\
Hence, I was able to implement a highly efficient Quine–McCluskey algorithm to reduce the binary decision tree.\

# Requirement

Consider a truth table such as below. The programme will initially given an array of strings, each of the string is the value from X1 to Xn, which will be evaluated as one. The input corresponds to table below is ["00","11"].

| X1 | X2 | X1 AND X2 |
|----|----|-----------|
| 0  | 0  | 1         |
| 0  | 1  | 0         |
| 1  | 0  | 0         |
| 1  | 1  | 1         |

- [ ] Write an function called buildcompactbdt() to generate an compact binary decision tree.
- [ ] Write an function called evalcompactbdt() to evaulate an string based on the binary decision tree.


The detailed description of the task is available [here](https://github.com/BenShen98/truth-table-compression/blob/master/requirement.pdf).

# High Level Implementation

Please refer to [this section](https://github.com/BenShen98/truth-table-compression/blob/master/report/high%20level%20implementation.pdf) of my report for detailed description.


To accommodate input with arbitrary, I created a new data structure called 'term', which is a vector of 'uint64_t'.

I also overloaded bitwise operators for the type 'term', as the overloading makes the code more readable and will not confuse the reader. I had read [this StackOverflow question](https://stackoverflow.com/questions/4421706/what-are-the-basic-rules-and-idioms-for-operator-overloading) before I consider overloading and couldn't agree more.


## Build the compact binary decision tree

The input for this function was a vector of strings, with each string representing X1 to Xn, where n is the length of the string.

Below is what this buildcompactbdt() perform:

1. Convert the vector of strings to vector of terms (vector of uint64), to save space and more importantly make the bitwise operation fast.

2. Using Quine–McCluskey's algorithm to find essential prime implicants.\
    a. classify each term to groups by number of 1 it contains.\
    b. compare two terms from neighbouring group repeatedly.\
    group these two terms if they only contain difference by 1 bit, mark that bit as don't care and call it the next generation.\
    c. repeat the process a to b for every generation until there is nothing to be simplified.
    
3.Using a heuristic algorithm to generating binary decision tree, with the aim to make duplicated node appear as deeper as possible.

## Evaluating string based on binary decision tree

Below is what this evalcompactbdt() perform:

1. Look at the root of the tree, reading its index, find whether it is 1 or 0 in the string at that index, choose the right or left child as root.

2. Repeat step 1, until reach the leaf node, the value of this node is the result of this evaluation (1 or 0).

# Implementation in C++

Please refer to [this section](https://github.com/BenShen98/truth-table-compression/blob/master/report/Implementation%20in%20C%2B%2B.pdf) of my report for detailed description of Implementation.

While I wrote this coursework, I aimed and achieved the following:

1. efficiency: As the number of comparison in Quine–McCluskey's algorithm grows exponentially with the size of the input, it was important to make sure it was fast. I achieved by:\
    a. Make sure there is no unnecessary copy, using pointers and pass by reference.\
    b. Make full use of bitwise operation, compare 64 bit at the same time.\
    c. Make use of binary tricks to use minimum numbers of instruction.\
    d. Consider GCC build in function, make use of CPU shortcut, such as '__builtin_popcount', but it was not allowed for this coursework due to the requirement.
    
2. readability and maintainability:\
    a. Only optimised for speed on functions that will be heavily used.\
    b. Use comment when I wrote binary tricks that may be difficult to understand.\
    b. Warp the optimised part as member functions.
    
# Verification and Testing Performance

It was vital that my function produces the correct result, that is why I had written an automated test function called testCorrectness(), which will iterate all the possible combination and check to bdt produce the correct response.

I had also used 'valgrind' to check there is no memory leak.

Please refer to [this section](https://github.com/BenShen98/truth-table-compression/blob/master/report/Verification%20and%20Testing%20Performance.pdf) of my report for detailed description.

### Test on compression 

To test for compression, I generated 5 set of 10,000 discrete number ranging from 1 to 0x1FFF, the maximum possible to generate at random.org.

On average, my code had a 74.3% of compression compare to standard bdt without simplification.

### Test on efficiency 
By providing input of 0b00000, 0b00001 up to 0b11111, a total of 32 terms, I was able to test maximum workload possible for a vector of 5-bit string.

My code only takes 104.1 ms to construct the tree, with 101kB on HEAP at maximum, with 99.3% reduction on time and 99.7% reduction on HEAP.
