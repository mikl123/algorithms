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

int quickSelect(std::vector<int> &, int, Pivot_f);

extern const BenchmarkData benchmarksData;

namespace Utils::KthOrderStatistics
{
Pivot_f getPivotF(PivotPolicy pivotPolicy)
{
    Pivot_f res = nullptr;

    switch (pivotPolicy)
    {
    case PivotPolicy::Deterministic:
        res = &::deterministicPivot;
        break;
    case PivotPolicy::UniformRandom:
        res = &::uniformRandomPivot;
        break;
    }
    if (res == nullptr)
    {
        throw InternalError{
            "kth-order-statistics.h: pivot function cannot be nullptr"
        };
    }

    return res;
}

struct Test
{
    Test() = delete;
    Test(std::vector<int> v, int k)
    {
        if (v.empty())
            throw InternalError{ "Test: v array cannot be empty" };
        if (k < 1 || k > v.size())
            throw InternalError{ "Test: wrong value for k" };

        values  = v;
        this->k = k;

        auto m = v.begin() + k - 1;
        std::nth_element(v.begin(), m, v.end());
        kthValue = *m;
    }

    Test(const Test &) = default;
    Test(Test &&)      = default;

    Test &operator=(const Test &) = default;
    Test &operator=(Test &&)      = default;

    ~Test() = default;

    std::vector<int> values;
    int              k;

    int kthValue;
};

std::vector<Test> getTests()
{
    auto min = std::numeric_limits<int>::min();
    auto max = std::numeric_limits<int>::max();

    auto tests = std::vector<Test>{};

    tests.emplace_back(std::vector<int>{ 1 }, 1);
    tests.emplace_back(std::vector<int>{ 2 }, 1);
    tests.emplace_back(std::vector<int>{ 5 }, 1);
    tests.emplace_back(std::vector<int>{ 4, 2 }, 1);
    tests.emplace_back(std::vector<int>{ 4, 2 }, 2);
    tests.emplace_back(std::vector<int>{ 4, 2, 7 }, 1);
    tests.emplace_back(std::vector<int>{ 4, 2, 7 }, 2);
    tests.emplace_back(std::vector<int>{ 4, 2, 7 }, 3);
    tests.emplace_back(std::vector<int>{ 1, 1, 2 }, 1);
    tests.emplace_back(std::vector<int>{ 1, 1, 2 }, 2);
    tests.emplace_back(std::vector<int>{ 1, 1, 2 }, 3);

    for (int t = 1; t <= 10; ++t)
    {
        auto values = std::vector<int>{};
        values.resize(2 * t, t);

        for (int k = 1; k <= t / 2; ++k)
            tests.emplace_back(values, k);
    }

    auto gen = std::mt19937{ 47 };

    for (int t = 1; t <= 10; ++t)
    {
        int n = t * 10;

        auto valuesDistr = std::uniform_int_distribution<int>{ -10, 10 };
        auto kDistr      = std::uniform_int_distribution<int>{ 1, n };

        auto values = std::vector<int>{};

        for (int i = 0; i < n; ++i)
            values.push_back(valuesDistr(gen));

        for (int i = 0; i < 10; ++i)
            tests.emplace_back(values, kDistr(gen));
    }

    return tests;
}

static void BM_kthOrderStatistics(
    benchmark::State &state,
    PivotPolicy       pivotPolicy,
    InputData         inputData)
{
    auto gen = std::mt19937{ 47 };

    const auto n = static_cast<int>(state.range(0));
    if (n <= 0)
        throw InternalError{ "BM_impl: n should be positive" };

    auto valuesDistr = std::uniform_int_distribution<int>{ -1000, 1000 };
    auto kDistr      = std::uniform_int_distribution<int>{ 1, n };

    Pivot_f pivot = getPivotF(pivotPolicy);

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

        const auto k = kDistr(gen);

        state.ResumeTiming();

        auto res = ::quickSelect(values, k, pivot);
        ::benchmark::DoNotOptimize(res);
    }
}

void registerBenchmarks()
{
    const auto &data = ::benchmarksData.getData();

    for (const auto pivotPolicy :
         { PivotPolicy::Deterministic, PivotPolicy::UniformRandom })
    {
        for (const auto inputData : { InputData::SortedArray,
                                      InputData::ReversedSortedArray,
                                      InputData::RandomArray })
        {
            const auto name =
                (std::stringstream{} << "kthOrderStatistics/" << pivotPolicy
                                     << "Pivot/" << inputData)
                    .str();

            const auto key = std::make_pair(
                static_cast<::PivotPolicy>(pivotPolicy),
                static_cast<::InputData>(inputData));

            const auto it = data.find(key);
            if (it == data.end())
                continue;

            auto b = benchmark::RegisterBenchmark(
                name, BM_kthOrderStatistics, pivotPolicy, inputData);

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

class KthOrderStatistics
    : public ::testing::TestWithParam<std::tuple<PivotPolicy, Test>>
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

TEST_P(KthOrderStatistics, Correctness)
{
    const auto &[pivotPolicy, test] = GetParam();

    Pivot_f pivot  = getPivotF(pivotPolicy);
    auto    values = test.values;
    auto    k      = test.k;

    const auto actual = ::quickSelect(values, k, pivot);

    ASSERT_EQ(actual, test.kthValue)
        << "Wrong kth value: PivotPolicy=" << pivotPolicy << ", "
        << "k = " << test.k << ", v = " << toString(test.values);
}

INSTANTIATE_TEST_SUITE_P(
    DeterministicPivot,
    KthOrderStatistics,
    ::testing::Combine(
        ::testing::ValuesIn({ PivotPolicy::Deterministic }),
        ::testing::ValuesIn(getTests())));
INSTANTIATE_TEST_SUITE_P(
    UniformRandomPivot,
    KthOrderStatistics,
    ::testing::Combine(
        ::testing::ValuesIn({ PivotPolicy::UniformRandom }),
        ::testing::ValuesIn(getTests())));

}    // namespace Utils::KthOrderStatistics
