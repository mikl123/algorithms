#pragma once

#include <cstddef>
#include <stdexcept>
#include<vector>

using Pivot_f = size_t (*)(int *, size_t);

size_t deterministicPivot(int *data, size_t n)
{
    return n;
}

size_t uniformRandomPivot(int *data, size_t n)
{
    return (rand() % (n));
}



void change(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}


size_t partititon(std::vector<int>& v, int l, int r, size_t pivot) {

    int index = l;
    change(&v[pivot], &v[r]);
    int pivot_value = v[r];

    for (int i = l; i < r;i++) {
        if (v[i] < pivot_value) {
            change(&v[i], &v[index]);
            index++;
        }
    }
    change(&v[index], &v[r]);
    return index;
}
int quickSelect1(std::vector<int>& v, int k, Pivot_f pivotFunction)
{
    k = k - 1;
    int right = v.size() - 1;
    int left = 0;
    size_t pivotIndex = 0;
    while (right != left) {
        pivotIndex = partititon(v, left, right, pivotFunction(v.data(), right - left) + left);
        if (k == pivotIndex) {
            return v[k];
        }
        else if (k < pivotIndex) {
            right = pivotIndex - 1;
        }
        else {
            left = pivotIndex + 1;
        }
    }
    return v[left];
}



double medianUniformRandomPivot1(std::vector<int>& v)
{
    if (v.size() % 2 == 0) {
        return (quickSelect1(v, (v.size() / 2), uniformRandomPivot) + quickSelect1(v, (v.size() / 2) + 1, uniformRandomPivot)) / 2.0;
    }
    else {
        return quickSelect1(v, (v.size() + 1) / 2, uniformRandomPivot);
    }
    return 0;
}
double medianDeterministicPivot1(std::vector<int>& v)
{
    if (v.size() % 2 == 0) {
        return (quickSelect1(v, (v.size() / 2), deterministicPivot) + quickSelect1(v, (v.size() / 2) + 1, deterministicPivot)) / 2.0;
    }
    else {
        return quickSelect1(v, (v.size() + 1) / 2, deterministicPivot);
    }
    return 0;
}



// --------------------
// --------------------
// Utility enums (don't touch)
// --------------------
// --------------------

enum class PivotPolicy
{
    Deterministic,
    UniformRandom,
    MedianDeterministic,
    MedianUniformRandom
};

enum class InputData
{
    SortedArray,
    ReversedSortedArray,
    RandomArray
};

#include <ostream>
std::ostream &operator<<(std::ostream &, PivotPolicy);
std::ostream &operator<<(std::ostream &, InputData);
#include "utils/common_impl.h"
