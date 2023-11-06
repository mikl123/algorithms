#include <stdexcept>
#include <vector>
#include "common.h"

void quickSort(std::vector<int>& v, int left, int right, Pivot_f pivotFunction) {
    if (left >= right) {
        return;
    }
    int pivotIndex = partititon(v, left, right, deterministicPivot(v.data(), right - left) + left);
    quickSort(v, left, pivotIndex - 1, pivotFunction);
    quickSort(v, pivotIndex + 1, right, pivotFunction);
}


size_t deterministicMedianPivot(int *data, size_t n){
    std::vector<int> integerVector(data, data + n);
    return medianDeterministicPivot1(integerVector);
}

size_t uniformRandomMedianPivot(int *data, size_t n)
{
    std::vector<int> integerVector(data, data + n);
    return medianUniformRandomPivot1(integerVector);
}

void quickSortSimplePivot(std::vector<int> &v, Pivot_f pivotFunction)
{
    quickSort(v, 0, v.size() - 1, pivotFunction);
}
void quickSortMedianPivot(std::vector<int> &v, Pivot_f pivotFunction)
{
    quickSort(v, 0, v.size() - 1, pivotFunction);
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
        .add(PivotPolicy::Deterministic, InputData::SortedArray,                { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::Deterministic, InputData::ReversedSortedArray,         { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::Deterministic, InputData::RandomArray,                { 100LL, 600LL, 1100LL, 1600LL, 2100LL })

        .add(PivotPolicy::UniformRandom, InputData::SortedArray,                 { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::UniformRandom, InputData::ReversedSortedArray,         { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::UniformRandom, InputData::RandomArray,                { 100LL, 600LL, 1100LL, 1600LL, 2100LL })

        .add(PivotPolicy::MedianDeterministic, InputData::SortedArray,           { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::MedianDeterministic, InputData::ReversedSortedArray,   { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::MedianDeterministic, InputData::RandomArray,           { 100LL, 600LL, 1100LL, 1600LL, 2100LL })

        .add(PivotPolicy::MedianUniformRandom, InputData::SortedArray,           { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::MedianUniformRandom, InputData::ReversedSortedArray,   { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
        .add(PivotPolicy::MedianUniformRandom, InputData::RandomArray,          { 100LL, 600LL, 1100LL, 1600LL, 2100LL })
    .build()
};
// clang-format on

// don't touch
#include "utils/quicksort.h"
