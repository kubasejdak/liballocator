/////////////////////////////////////////////////////////////////////////////////////
///
/// @file
/// @author Kuba Sejdak
/// @copyright BSD 2-Clause License
///
/// Copyright (c) 2017-2023, Kuba Sejdak <kuba.sejdak@gmail.com>
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// 1. Redistributions of source code must retain the above copyright notice, this
///    list of conditions and the following disclaimer.
///
/// 2. Redistributions in binary form must reproduce the above copyright notice,
///    this list of conditions and the following disclaimer in the documentation
///    and/or other materials provided with the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
/// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
/// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
/// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
/// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
/// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
/// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
/// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/////////////////////////////////////////////////////////////////////////////////////

#include "platform/init.hpp"

#include <allocator/allocator.hpp>

#include <fmt/printf.h>

#include <cstddef>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

// Defined in linker script.
extern const char heapMin; // NOLINT
extern const char heapMax; // NOLINT

void* operator new(std::size_t size)
{
    return memory::allocator::allocate(size);
}

void operator delete(void* ptr) noexcept
{
    memory::allocator::release(ptr);
}

void operator delete(void* ptr, [[maybe_unused]] std::size_t sz) noexcept
{
    operator delete(ptr);
}

static std::size_t freeMemory()
{
    return memory::allocator::getStats().freeMemorySize;
}

static std::size_t initialFreeMemory;

static void showStats()
{
    auto stats = memory::allocator::getStats();
    fmt::print("Total memory size     : {} B\n", stats.totalMemorySize);
    fmt::print("Reserved memory size  : {} B\n", stats.reservedMemorySize);
    fmt::print("User memory size      : {} B\n", stats.userMemorySize);
    fmt::print("Allocated memory size : {} B\n", stats.allocatedMemorySize);
    fmt::print("Free memory size      : {} B\n", stats.freeMemorySize);
    fmt::print("\n");
}

static void testStart(const char* description)
{
    fmt::print("------------------------------------------------\n");
    fmt::print("{}\n\n", description);
    showStats();
}

static bool testEnd()
{
    showStats();

    return (initialFreeMemory == freeMemory());
}

static bool testSmartPointers()
{
    testStart("Testing allocator with smart pointers");

    {
        fmt::print("Allocate 113 B (std::make_unique<char[]>)...\n");
        constexpr int cAllocSize = 113;
        auto ptr = std::make_unique<char[]>(cAllocSize); // NOLINT
        fmt::print("ptr = {}\n", static_cast<void*>(ptr.get()));
        showStats();

        fmt::print("Release memory (ptr.reset())...\n");
    }

    return testEnd();
}

static bool testVector()
{
    testStart("Testing allocator with std::vector<int>");

    {
        std::vector<int> vec;
        constexpr int cItemsCount = 16;

        for (int i = 0; i < cItemsCount; ++i)
            vec.push_back(i);

        for (std::size_t i = 0; i < vec.size(); ++i)
            fmt::print("vec[{}] = {}\n", i, vec[i]);
    }

    return testEnd();
}

static bool testMap()
{
    testStart("Testing allocator with std::map<int, std::string>");

    {
        std::map<int, std::string> map;
        map[0] = "Great resources on modern C++:";
        map[1] = "C++ Weekly - YouTube channel hosted by Jason Turner";
        map[2] = "\"Effective\" books by Scott Meyers";
        map[3] = "C++ Core Guidelines - set of guidelines, rules, and best practices about coding in C++";
        map[4] = "CppCast - the first podcast for C++ developers by C++ developers";

        for (const auto& [key, value] : map)
            fmt::print("map[{}] = {}\n", key, value);
    }

    return testEnd();
}

// NOLINTNEXTLINE
int appMain([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    if (!platform::init())
        return EXIT_FAILURE;

    constexpr std::size_t cPageSize = 512;
    if (!memory::allocator::init(std::uintptr_t(&heapMin), std::uintptr_t(&heapMax), cPageSize)) {
        fmt::print("error: Failed to initialize liballocator.\n");
        fmt::print("FAILED\n");
        return EXIT_FAILURE;
    }

    fmt::print("Initialized liballocator v{}.\n\n", memory::allocator::version());
    initialFreeMemory = freeMemory();

    if (!testSmartPointers()) {
        fmt::print("FAILED\n");
        return EXIT_FAILURE;
    }

    if (!testVector()) {
        fmt::print("FAILED\n");
        return EXIT_FAILURE;
    }

    if (!testMap()) {
        fmt::print("FAILED\n");
        return EXIT_FAILURE;
    }

    fmt::print("PASSED\n");
    return EXIT_SUCCESS;
}
