#include "common.h"

#include <ostream>

std::ostream &operator<<(std::ostream &strm, PivotPolicy rhs)
{
    switch (rhs)
    {
    case PivotPolicy::Deterministic:
        return strm << "Deterministic";
    case PivotPolicy::UniformRandom:
        return strm << "UniformRandom";
    case PivotPolicy::MedianDeterministic:
        return strm << "MedianDeterministic";
    case PivotPolicy::MedianUniformRandom:
        return strm << "MedianUniformRandom";
    }

    return strm << "Unknown";
}

std::ostream &operator<<(std::ostream &strm, InputData rhs)
{
    switch (rhs)
    {
    case InputData::SortedArray:
        return strm << "SortedArray";
    case InputData::ReversedSortedArray:
        return strm << "ReversedSortedArray";
    case InputData::RandomArray:
        return strm << "RandomArray";
    }

    return strm << "Unknown";
}
