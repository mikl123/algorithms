#pragma once

#include <algorithm>
#include <cmath>
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

double medianDeterministicPivot(std::vector<int> &v);
double medianUniformRandomPivot(std::vector<int> &v);

extern const BenchmarkData benchmarksData;

namespace Utils::Median
{
using median_fn = double (*)(std::vector<int> &);

median_fn getMedianFn(PivotPolicy pivotPolicy)
{
    median_fn res = nullptr;

    switch (pivotPolicy)
    {
    case PivotPolicy::Deterministic:
        res = &::medianDeterministicPivot;
        break;
    case PivotPolicy::UniformRandom:
        res = &::medianUniformRandomPivot;
        break;
    }

    if (res == nullptr)
    {
        throw InternalError{ "median.h: median function cannot be nullptr" };
    }

    return res;
}

struct Test
{
    Test() = delete;
    Test(std::vector<int> v)
    {
        if (v.empty())
            throw InternalError{ "Test: v array cannot be empty" };

        values = v;

        std::sort(v.begin(), v.end());
        if (v.size() % 2 == 1)
            median = v[v.size() / 2];
        else
            median = (v[v.size() / 2] + v[v.size() / 2 - 1]) / 2.0;
    }

    Test(const Test &) = default;
    Test(Test &&)      = default;

    Test &operator=(const Test &) = default;
    Test &operator=(Test &&)      = default;

    ~Test() = default;

    std::vector<int> values;
    double           median;
};

std::vector<Test> getTests()
{
    auto min = std::numeric_limits<int>::min();
    auto max = std::numeric_limits<int>::max();

    auto tests = std::vector<Test>{};

    tests.emplace_back(std::vector<int>{ 1 });
    tests.emplace_back(std::vector<int>{ 2 });
    tests.emplace_back(std::vector<int>{ 5 });
    tests.emplace_back(std::vector<int>{ 4, 2 });
    tests.emplace_back(std::vector<int>{ 4, 2 });
    tests.emplace_back(std::vector<int>{ 4, 2, 7 });
    tests.emplace_back(std::vector<int>{ 4, 2, 7 });
    tests.emplace_back(std::vector<int>{ 4, 2, 7 });
    tests.emplace_back(std::vector<int>{ 1, 1, 2 });
    tests.emplace_back(std::vector<int>{ 1, 1, 2 });
    tests.emplace_back(std::vector<int>{ 1, 1, 2 });

    for (int t = 1; t <= 20; ++t)
    {
        auto values = std::vector<int>{};
        values.resize(2 * t, t);

        tests.emplace_back(values);
    }

    auto gen = std::mt19937{ 47 };

    for (int t = 1; t <= 20; ++t)
    {
        int n = t * 10;

        auto valuesDistr = std::uniform_int_distribution<int>{ -10, 10 };
        auto values      = std::vector<int>{};

        for (int i = 0; i < n; ++i)
            values.push_back(valuesDistr(gen));

        tests.emplace_back(values);
    }

    return tests;
}

static void
    BM_median(benchmark::State &state, PivotPolicy pivotPolicy, InputData inputData)
{
    auto gen = std::mt19937{ 47 };

    const auto n = static_cast<int>(state.range(0));
    if (n <= 0)
        throw InternalError{ "BM_impl: n should be positive" };

    auto valuesDistr = std::uniform_int_distribution<int>{ -1000, 1000 };

    median_fn medianFn = getMedianFn(pivotPolicy);

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

        auto res = medianFn(values);
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
            const auto name = (std::stringstream{} << "median/" << pivotPolicy
                                                   << "Pivot/" << inputData)
                                  .str();

            const auto key = std::make_pair(
                static_cast<::PivotPolicy>(pivotPolicy),
                static_cast<::InputData>(inputData));

            const auto it = data.find(key);
            if (it == data.end())
                continue;

            auto b = benchmark::RegisterBenchmark(
                name, BM_median, pivotPolicy, inputData);

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

class Median : public ::testing::TestWithParam<std::tuple<PivotPolicy, Test>>
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

TEST_P(Median, Correctness)
{
    const auto &[pivotPolicy, test] = GetParam();

    auto medianFn = getMedianFn(pivotPolicy);

    auto       values = test.values;
    const auto actual = medianFn(values);

    ASSERT_LE(std::abs(actual - test.median), 1e-8)
        << "Wrong median: PivotPolicy=" << pivotPolicy
        << "v = " << toString(test.values);
}

INSTANTIATE_TEST_SUITE_P(
    DeterministicPivot,
    Median,
    ::testing::Combine(
        ::testing::ValuesIn({ PivotPolicy::Deterministic }),
        ::testing::ValuesIn(getTests())));
INSTANTIATE_TEST_SUITE_P(
    UniformRandomPivot,
    Median,
    ::testing::Combine(
        ::testing::ValuesIn({ PivotPolicy::UniformRandom }),
        ::testing::ValuesIn(getTests())));

}    // namespace Utils::Median
