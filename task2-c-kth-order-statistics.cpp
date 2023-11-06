#include <stdexcept>
#include <vector>
#include "common.h"
/**
 * @brief finds kth order statistics in the input vector @p v
 *
 * @param v
 * @param k
 * @param pivotFunction - function pointer that specifies the algorithm to select
 * pivot element. quickSelect will be tested/benchmarked with deterministicPivot
 * and uniformRandomPivot implementations from common.h
 *
 * Constraints:
 *      1. v.size() > 0
 *      2. 1 <= k <= v.size()
 *      3. pivotPolicy != nullptr (e.g. it's safe to call pivotPolicy())
 *
 * @return int - kth order statistics of the input array @p v . For example:
 *      v = [3, 2, 5, 4],     k = 2 ---> 3
 *      v = [5, 7, 6, 5],     k = 2 ---> 5
 *      v = [4, 5, 6, 0, 2],  k = 1 ---> 0
 */

int quickSelect(std::vector<int>& v, int k, Pivot_f pivotFunction)
{
    return quickSelect1(v, k, pivotFunction);
}

// --------------------
// --------------------
// --------------------

// clang-format off
#include "utils/benchmarkdata.h"

// lengths of arrays to benchmark (for different combinations of pivot policy and input data)
// feel free to change the numbers or add more if necessary
const BenchmarkData benchmarksData {
    BenchmarkData::Builder{}
        .add(PivotPolicy::Deterministic, InputData::SortedArray,             { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::Deterministic, InputData::ReversedSortedArray,     { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::Deterministic, InputData::RandomArray,             { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        
        .add(PivotPolicy::UniformRandom, InputData::SortedArray,             { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::UniformRandom, InputData::ReversedSortedArray,     { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::UniformRandom, InputData::RandomArray,             { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
    .build()
};
// clang-format on

// don't touch
#include "utils/kth-order-statistics.h"





