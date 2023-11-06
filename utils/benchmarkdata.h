#pragma once

#include <map>
#include <utility>
#include <vector>

#include "common.h"

class BenchmarkData
{
    using data_t =
        std::map<std::pair<PivotPolicy, InputData>, std::vector<long long>>;

public:
    class Builder
    {
    public:
        BenchmarkData build() { return BenchmarkData{ std::move(_data) }; }

        Builder &
            add(PivotPolicy            pivotPolicy,
                InputData              inputData,
                std::vector<long long> ns)
        {
            const auto key = std::make_pair(pivotPolicy, inputData);
            _data[key]     = std::move(ns);

            return *this;
        }

    private:
        data_t _data;
    };

    const std::map<std::pair<PivotPolicy, InputData>, std::vector<long long>> &
        getData() const
    {
        return _data;
    }

private:
    BenchmarkData(data_t data) : _data{ std::move(data) } {}

    std::map<std::pair<PivotPolicy, InputData>, std::vector<long long>> _data;
};
