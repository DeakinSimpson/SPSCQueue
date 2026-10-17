
#include <chrono>
#include <random>
#include <benchmark/benchmark.h>
#include "SPSC_Queue.hpp"
#include <thread>

static void BM_PUSH(benchmark::State& state)
{
    const auto numPush { static_cast<std::size_t>(state.range(0)) };


    for (auto _ : state)
    {
        TSC::Queue<int> q { numPush };

        for (auto i {0uz}; i < numPush; ++i)
        {
            q.push(1);
        }
    }

    double total_ops = static_cast<double>(state.iterations() * numPush);

    state.counters["ops_sec"] = benchmark::Counter(total_ops, benchmark::Counter::kIsRate);
}

BENCHMARK(BM_PUSH)->Arg(10000000);

static void BM_POP(benchmark::State& state)
{


    const auto numPush { static_cast<std::size_t>(state.range(0)) };


    for (auto _ : state)
    {
        state.PauseTiming();

        TSC::Queue<int> q { numPush };

        for (auto i {0uz}; i < numPush; ++i)
        {
            q.push(1);
        }

        state.ResumeTiming();

        for (auto i {0uz}; i < numPush; ++i)
        {
            int temp;
            q.pop(temp);
        }
    }


    auto total_ops { state.iterations() * numPush };

    state.counters["ops_sec"] = benchmark::Counter(total_ops, benchmark::Counter::kIsRate);
}

BENCHMARK(BM_POP)->Arg(10000000);

static void BM_PUSH_POP(benchmark::State& state)
{


    const auto numPush { static_cast<std::size_t>(state.range(0)) };


    for (auto _ : state)
    {
        TSC::Queue<int> q { numPush };

        for (auto i {0uz}; i < numPush; ++i)
        {
            q.push(1);
        }

        for (auto i {0uz}; i < numPush; ++i)
        {
            int temp;
            q.pop(temp);
        }
    }


    auto total_ops { state.iterations() * numPush * 2 };

    state.counters["ops_sec"] = benchmark::Counter(total_ops, benchmark::Counter::kIsRate);
}

BENCHMARK(BM_PUSH_POP)->Arg(10000000);

static void BM_PUSH_POP_RANDOM(benchmark::State& state)
{


    const auto numPush { static_cast<std::size_t>(state.range(0)) };

    for (auto _ : state)
    {
        TSC::Queue<int> q { numPush };

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distrib(0, 1);

        if (distrib(gen))
        {
            for (auto i {0uz}; i < numPush; ++i)
            {
                q.push(1);
            }
        } else
        {
            for (auto i {0uz}; i < numPush; ++i)
            {
                int temp;
                q.pop(temp);
            }
        }
    }


    auto total_ops { state.iterations() * numPush * 2 };

    state.counters["ops_sec"] = benchmark::Counter(total_ops, benchmark::Counter::kIsRate);
}

BENCHMARK(BM_PUSH_POP_RANDOM)->Arg(10000000);

static void BM_PUSH_POP_RANDOM_MT(benchmark::State& state)
{
    auto func = [](TSC::Queue<int>& q, std::size_t num, bool push) {
        for (auto i {0uz}; i < num; ++i)
        {
            if (push)
            {
                q.push(1);
            } else
            {
                int temp;
                q.pop(temp);
            }

        }
    };

    const auto numPush { static_cast<std::size_t>(state.range(0)) };

    for (auto _ : state)
    {
        TSC::Queue<int> q { numPush };

        std::jthread jt1(func, std::ref(q), numPush, true);
        std::jthread jt2(func, std::ref(q), numPush, false);
    }


    auto total_ops { state.iterations() * numPush * 2 };

    state.counters["ops_sec"] = benchmark::Counter(total_ops, benchmark::Counter::kIsRate);
}

BENCHMARK(BM_PUSH_POP_RANDOM_MT)->Arg(10000000);


BENCHMARK_MAIN();