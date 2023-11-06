#pragma once

#include <algorithm>
#include <limits>
#include <ostream>
#include <random>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include "benchmarkdata.h"
#include "common.h"
#include "internalerror.h"
#include "main.h"

size_t deterministicMedianPivot(int *data, size_t n);
size_t uniformRandomMedianPivot(int *data, size_t n);

void quickSortSimplePivot(std::vector<int> &v, Pivot_f pivotFunction);
void quickSortMedianPivot(std::vector<int> &v, Pivot_f pivotFunction);

extern const BenchmarkData benchmarksData;

namespace Utils::QuickSort
{
using QuickSort_f = void (*)(std::vector<int> &, Pivot_f);

QuickSort_f getQuickSortF(PivotPolicy pivotPolicy)
{
    switch (pivotPolicy)
    {
    case PivotPolicy::Deterministic:
    case PivotPolicy::UniformRandom:
        return &::quickSortSimplePivot;
    case PivotPolicy::MedianDeterministic:
    case PivotPolicy::MedianUniformRandom:
        return &::quickSortMedianPivot;
    }

    throw InternalError{ "quicksort.h: quick sort function cannot be nullptr" };
}

Pivot_f getPivotF(PivotPolicy pivotPolicy)
{
    switch (pivotPolicy)
    {
    case PivotPolicy::Deterministic:
        return &::deterministicPivot;
    case PivotPolicy::UniformRandom:
        return &::uniformRandomPivot;
    case PivotPolicy::MedianDeterministic:
        return &::deterministicMedianPivot;
    case PivotPolicy::MedianUniformRandom:
        return &::uniformRandomMedianPivot;
    }

    throw InternalError{ "quickSort.h: pivot function cannot be nullptr" };
}

struct Test
{
    Test() = delete;
    Test(std::vector<int> v)
    {
        values = v;

        sortedValues = std::move(v);
        std::sort(sortedValues.begin(), sortedValues.end());
    }

    Test(const Test &) = default;
    Test(Test &&)      = default;

    Test &operator=(const Test &) = default;
    Test &operator=(Test &&)      = default;

    ~Test() = default;

    std::vector<int> values;
    std::vector<int> sortedValues;
};

std::vector<Test> getTests()
{
    auto min = std::numeric_limits<int>::min();
    auto max = std::numeric_limits<int>::max();

    auto tests = std::vector<Test>{};

    tests.emplace_back(std::vector<int>{});
    tests.emplace_back(std::vector<int>{ 1 });
    tests.emplace_back(std::vector<int>{ 2 });
    tests.emplace_back(std::vector<int>{ 5 });
    tests.emplace_back(std::vector<int>{ 4, 2 });
    tests.emplace_back(std::vector<int>{ 4, 2, 7 });
    tests.emplace_back(std::vector<int>{ 1, 2, 1 });

    for (int t = 1; t <= 10; ++t)
    {
        auto values = std::vector<int>{};
        values.resize(2 * t, t);
    }

    auto gen = std::mt19937{ 47 };

    for (int t = 1; t <= 50; ++t)
    {
        int n = t * 10;

        auto valuesDistr = std::uniform_int_distribution<int>{ -10, 10 };

        auto values = std::vector<int>{};

        for (int i = 0; i < n; ++i)
            values.push_back(valuesDistr(gen));

        tests.emplace_back(values);
    }

    return tests;
}

static void BM_quickSort(
    benchmark::State &state,
    PivotPolicy       pivotPolicy,
    InputData         inputData)
{
    auto gen = std::mt19937{ 47 };

    const auto n = static_cast<int>(state.range(0));
    if (n < 0)
        throw InternalError{ "BM_impl: n cannot be negative" };

    auto valuesDistr = std::uniform_int_distribution<int>{ -1000, 1000 };

    QuickSort_f quickSort = getQuickSortF(pivotPolicy);
    Pivot_f     pivot     = getPivotF(pivotPolicy);

    for (auto _ : state)
    {
        state.PauseTiming();

        auto values = std::vector<int>{};
        values.reserve(n);

        for (int i = 0; i < n; ++i)
            values.push_back(valuesDistr(gen));

        switch (inputData)
        {
        case InputData::RandomArray:
            break;
        case InputData::SortedArray:
            std::sort(values.begin(), values.end());
            break;
        case InputData::ReversedSortedArray:
            std::sort(values.begin(), values.end());
            std::reverse(values.begin(), values.end());
            break;
        }

        state.ResumeTiming();

        ::benchmark::DoNotOptimize([&] { quickSort(values, pivot); });
    }
}

void registerBenchmarks()
{
    const auto &data = ::benchmarksData.getData();

    for (const auto pivotPolicy : { PivotPolicy::Deterministic,
                                    PivotPolicy::UniformRandom,
                                    PivotPolicy::MedianDeterministic,
                                    PivotPolicy::MedianUniformRandom })
    {
        for (const auto inputData : { InputData::SortedArray,
                                      InputData::ReversedSortedArray,
                                      InputData::RandomArray })
        {
            const auto name = (std::stringstream{} << "quickSort/" << pivotPolicy
                                                   << "Pivot/" << inputData)
                                  .str();

            const auto key = std::make_pair(
                static_cast<::PivotPolicy>(pivotPolicy),
                static_cast<::InputData>(inputData));

            const auto it = data.find(key);
            if (it == data.end())
                continue;

            auto b = benchmark::RegisterBenchmark(
                name, BM_quickSort, pivotPolicy, inputData);

            for (const auto &n : it->second)
            {
                b->Arg(n);
            }
        }
    }
}

const int kTmp{ []() -> int
                {
                    registerBenchmarks();
                    return 0;
                }() };

class QuickSort : public ::testing::TestWithParam<std::tuple<PivotPolicy, Test>>
{
protected:
    void SetUp() override
    {
        const auto &[testPivotPolicy, test] = GetParam();

        auto shouldBeSkipped = true;

        for (const auto &[key, val] : ::benchmarksData.getData())
        {
            const auto &[pivotPolicy, inputData] = key;

            if (pivotPolicy == testPivotPolicy)
                shouldBeSkipped = false;
        }

        if (shouldBeSkipped)
        {
            GTEST_SKIP();
        }
    }
};

TEST_P(QuickSort, Correctness)
{
    const auto &[pivotPolicy, test] = GetParam();

    QuickSort_f quickSort = getQuickSortF(pivotPolicy);
    Pivot_f     pivot     = getPivotF(pivotPolicy);

    auto values = test.values;
    quickSort(values, pivot);

    ASSERT_EQ(values.size(), test.values.size())
        << "Array changed its size after calling quickSort, PivotPolicy = "
        << pivotPolicy << ", input values = " << toString(test.values)
        << ", values after sorting: " << toString(values);

    const int n = values.size();

    for (int i = 0; i < n; ++i)
    {
        ASSERT_EQ(values.at(i), test.sortedValues.at(i))
            << "Array is poorly sorted, PivotPolicy = " << pivotPolicy
            << ", input values = " << toString(test.values)
            << ", values after sorting: " << toString(values)
            << ", sorted values: " << toString(test.sortedValues);
    }
}

INSTANTIATE_TEST_SUITE_P(
    DeterministicPivot,
    QuickSort,
    ::testing::Combine(
        ::testing::ValuesIn({ PivotPolicy::Deterministic }),
        ::testing::ValuesIn(getTests())));
INSTANTIATE_TEST_SUITE_P(
    UniformRandomPivot,
    QuickSort,
    ::testing::Combine(
        ::testing::ValuesIn({ PivotPolicy::UniformRandom }),
        ::testing::ValuesIn(getTests())));
INSTANTIATE_TEST_SUITE_P(
    DeterministicMedianPivot,
    QuickSort,
    ::testing::Combine(
        ::testing::ValuesIn({ PivotPolicy::MedianDeterministic }),
        ::testing::ValuesIn(getTests())));
INSTANTIATE_TEST_SUITE_P(
    UniformRandomMedianPivot,
    QuickSort,
    ::testing::Combine(
        ::testing::ValuesIn({ PivotPolicy::MedianUniformRandom }),
        ::testing::ValuesIn(getTests())));

}    // namespace Utils::QuickSort
