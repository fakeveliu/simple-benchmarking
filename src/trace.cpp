#include "trace.h"
#include <iostream>
#include <fstream>

TraceDriver::TraceDriver(const std::string& out_path) : out_path_(out_path) {
    Trace trace_loop("loop", [](size_t n){
        for (size_t i = 0; i < n; ++i) {

        } 
    }, 10000, 10, 6);
    register_trace(trace_loop);

    Trace trace_threads("threads", [](size_t n) {
        std::vector<std::thread> threads;
        for (int i = 0; i < n; ++i) {
            threads.push_back(std::thread([](){}));
        }
        for (int i = 0; i < n; ++i) {
            threads[i].join();
        }
    }, 10, 10, 5);
    register_trace(trace_threads);

    Trace trace_memory_copy("memory_copy", [](size_t n) {
        // big array copy: memory bandwidth and throughput
        std::vector<int> src(n, 1);
        std::vector<int> dest(n, 0); 

        std::copy(src.begin(), src.end(), dest.begin());
    }, 1000000, 2, 5);
    register_trace(trace_memory_copy);

    Trace trace_memory_sort("memory_sort", [](size_t n) {
        // sort: computational capabilities and cache behavior
        std::vector<int> data(n);
        std::generate(data.begin(), data.end(), std::rand);
        std::sort(data.begin(), data.end());
    }, 10000, 2, 5);
    register_trace(trace_memory_sort);

    Trace trace_memory_search("memory_search", [](size_t n) {
        // search: various memory access patterns
        std::vector<int> data(n);
        std::generate(data.begin(), data.end(), std::rand);
        std::sort(data.begin(), data.end());

        for (size_t i = 0; i < n; ++i) {
            int target = std::rand() % n;
            (void)std::binary_search(data.begin(), data.end(), target);
        }
    }, 10000, 2, 5);
    register_trace(trace_memory_search);
}

void TraceDriver::time_trace(Trace& trace) {
    size_t n = trace.min_;
    size_t i = 0;
    std::vector<int> data;
    while (i < trace.iterations_) {
        auto t0 = std::chrono::high_resolution_clock::now();
        trace.trace_(n);
        auto t1 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        data.push_back(n);
        data.push_back(duration);
        n *= trace.factor_;
        ++i;
    }
    output.push_back(std::make_pair(trace.name_, data));
}

void TraceDriver::run_traces(std::vector<std::string>& user_traces) {
    if (user_traces.empty()) {
        for (auto& it : all_traces) {
            time_trace(it.second);
        }
    } else {
        for (auto& trace_name : user_traces) {
            auto it = all_traces.find(trace_name);
            if (it != all_traces.end()) {
                time_trace(it->second);
            }
        }
    }
}

void TraceDriver::format_output() {
    std::ofstream file(out_path_);
    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return;
    }

    for (const TraceResult& res : output) {
        file << res.first;
        
        std::string indices, values;
        for (size_t i = 0; i < res.second.size(); i++) {
            if (i % 2 == 0) {
                if (!indices.empty()) indices += ",";
                indices += std::to_string(res.second[i]);
            } else {
                if (!values.empty()) values += ",";
                values += std::to_string(res.second[i]);
            }
        }

        file << "/" << indices << "/" << values << std::endl;
    }

    file.close();
}