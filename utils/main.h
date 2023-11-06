#pragma once

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include "internalerror.h"

namespace Utils
{
std::string toString(const std::vector<int> &v)
{
    auto strm = std::stringstream{};

    strm << "[";

    if (!v.empty())
        strm << v.front();

    for (int i = 1; i < v.size(); ++i)
        strm << ", " << v[i];

    strm << "]";

    return strm.str();
}

std::filesystem::path getBenchmarkPath(
    const std::filesystem::path &benchmarksFolder,
    const std::string           &executableName)
{
    if (!std::filesystem::is_directory(benchmarksFolder))
    {
        throw std::runtime_error{
            "Cannot find benchmarks folder (please create it): " +
            benchmarksFolder.string()
        };
    }

    if (auto path = benchmarksFolder / (executableName + ".json");
        !std::filesystem::exists(path))
    {
        return path;
    }

    for (int i = 1; i < 100; ++i)
    {
        auto path =
            benchmarksFolder / (executableName + "-" + std::to_string(i) + ".json");

        if (!std::filesystem::exists(path))
            return path;
    }

    throw std::runtime_error{ "Too many benchmark files for " + executableName +
                              " exists" };
}

/**
 * @brief adds additional cmd args during runtime
 */
class CmdArgs
{
public:
    CmdArgs(int argc, char **argv)
    {
        if (argv == nullptr)
            throw InternalError{ "argv cannot be nullptr" };

        _argc = argc;

        for (int i = 0; i < argc; ++i)
            _argv.push_back(argv[i]);
    }

    CmdArgs(const CmdArgs &) = delete;
    CmdArgs(CmdArgs &&)      = delete;

    CmdArgs &operator=(const CmdArgs &) = delete;
    CmdArgs &operator=(CmdArgs &&)      = delete;

    ~CmdArgs() = default;

    void add(std::string arg)
    {
        ++_argc;
        _additionalArgsHolder.push_back(std::move(arg));
        _argv.push_back(_additionalArgsHolder.back().data());
    }

    int   *argc() { return &_argc; }
    char **argv() { return _argv.data(); }

private:
    int                 _argc;
    std::vector<char *> _argv;

    std::vector<std::string> _additionalArgsHolder;
};

int main(int argc, char **argv)
{
    try
    {
        const std::string benchmarksFolder{ HW2_BENCHMARKS_FOLDER };
        const std::string executableName{ HW2_EXECUTABLE_NAME };

        const auto benchmarkPath =
            getBenchmarkPath(benchmarksFolder, executableName);

        auto args = CmdArgs{ argc, argv };

        args.add("--benchmark_out_format=json");
        args.add("--benchmark_out=" + benchmarkPath.string());

        args.add("--gtest_catch_exceptions=0");
        args.add("--gtest_break_on_failure");

        ::testing::InitGoogleTest(args.argc(), args.argv());
        const auto testsOk = RUN_ALL_TESTS();
        if (testsOk != 0)
        {
            std::cerr << "Failed to passed some tests, exiting..." << std::endl;
            return 1;
        }

        std::cout << "-----------------------" << std::endl;
        std::cout << "-----------------------" << std::endl;
        std::cout << "-----------------------" << std::endl;
        std::cout << "Tests passed, launching benchmarks" << std::endl;
        std::cout << "Writing benchmark to " << benchmarkPath.string() << std::endl;
        std::cout << "-----------------------" << std::endl;
        std::cout << "-----------------------" << std::endl;
        std::cout << "-----------------------" << std::endl;

        ::benchmark::Initialize(args.argc(), args.argv());
        ::benchmark::RunSpecifiedBenchmarks();
        ::benchmark::Shutdown();

        return 0;
    }
    catch (const InternalError &e)
    {
        std::cerr << "Internal error occured (should not happen): " << e.what()
                  << std::endl;
        throw;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occured: " << e.what() << std::endl;
        throw;
    }
}

}    // namespace Utils

int main(int argc, char **argv)
{
    return Utils::main(argc, argv);
}
