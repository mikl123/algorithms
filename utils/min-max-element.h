#pragma once

#include <algorithm>
#include <limits>
#include <random>
#include <sstream>
#include <vector>

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include "internalerror.h"
#include "main.h"

extern const std::vector<long long> Ns;

std::pair<int, int> minMaxElement(const std::vector<int> &);

namespace Utils::MinMaxElement
{
struct Test
{
    Test() = delete;
    Test(std::vector<int> v)
    {
        if (v.empty())
            throw InternalError{ "Test: v array cannot be empty" };

        values   = std::move(v);
        minValue = *std::min_element(values.begin(), values.end());
        maxValue = *std::max_element(values.begin(), values.end());
    }

    Test(const Test &) = default;
    Test(Test &&)      = default;

    Test &operator=(const Test &) = default;
    Test &operator=(Test &&)      = default;

    ~Test() = default;

    std::vector<int> values;

    int minValue;
    int maxValue;
};

std::vector<Test> getTests()
{
    auto min = std::numeric_limits<int>::min();
    auto max = std::numeric_limits<int>::max();

    auto tests = std::vector<Test>{};

    auto gen = std::mt19937{ 47 };

    for (int i = 1; i <= 10; ++i)
        tests.push_back(std::vector<int>{ i });
    for (int i = 1; i <= 10; ++i)
        tests.push_back(std::vector<int>{ i, i + 1 });
    for (int i = 1; i <= 10; ++i)
        tests.push_back(std::vector<int>{ i - 1, i + 1, i });
    for (int t = 0; t < 20; ++t)
    {
        auto distr = std::uniform_int_distribution<int>(-10, 10);

        auto test = std::vector<int>{};
        for (int i = 0; i < 40; ++i)
            test.push_back(distr(gen));

        tests.push_back(std::move(test));
    }

    tests.push_back(std::vector<int>{ min + 5, min, min + 1 });
    tests.push_back(std::vector<int>{ max - 100, max, max, max });

    tests.push_back(std::vector<int>{
        max - 100, 0, 1, 10, 10, 10, 10, 10, min + 2, min + 5, max - 10 });

    return tests;
}

static void BM_minMaxElement(benchmark::State &state)
{
    auto gen = std::mt19937{ 47 };

    const auto min = std::numeric_limits<int>::min();
    const auto max = std::numeric_limits<int>::max();

    auto distr = std::uniform_int_distribution<int>{ min, max };

    const auto n = static_cast<int>(state.range(0));

    for (auto _ : state)
    {
        state.PauseTiming();

        auto v = std::vector<int>{};
        v.reserve(n);

        for (int i = 0; i < n; ++i)
            v.push_back(distr(gen));

        state.ResumeTiming();

        auto res = minMaxElement(v);
        ::benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(BM_minMaxElement)
    ->Apply(
        [](benchmark::internal::Benchmark *b)
        {
            for (const auto &n : Ns)
                b->Arg(n);

            b->Name("MinMaxElement");
        });

class MinMaxElement : public ::testing::TestWithParam<Test>
{
};

TEST_P(MinMaxElement, Correctness)
{
    const auto &test = GetParam();

    const auto actual = minMaxElement(test.values);

    ASSERT_LE(actual.first, actual.second)
        << "minMaxElement: expected first value of returned pair to contain smaller element than second";

    ASSERT_EQ(test.minValue, actual.first)
        << "minMaxElement: wrong min value, values = " << toString(test.values);
    ASSERT_EQ(test.maxValue, actual.second)
        << "minMaxElement: wrong max value, values = " << toString(test.values);
}

INSTANTIATE_TEST_SUITE_P(
    MinMaxElementTests,
    MinMaxElement,
    ::testing::ValuesIn(getTests()));

}    // namespace Utils::MinMaxElement
