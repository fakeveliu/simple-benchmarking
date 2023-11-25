#include "trace.h"
#include <iostream>
#include <fstream>

TraceDriver::TraceDriver(const std::string& out_path) : out_path_(out_path) {
    Trace trace_loop("loop", [](size_t n){
        for (size_t i = 0; i < n; ++i) {
            // dummy
        } 
    }, 10000000000, 1, 1);
    register_trace(trace_loop);

    Trace trace_threads("threads", [](size_t n) {
        std::vector<std::thread> threads;
        for (int i = 0; i < n; ++i) {
            threads.push_back(std::thread([](){})); // dummy thread
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
    }, 1000000, 2, 5);
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
    }, 1000000, 2, 5);
    register_trace(trace_memory_search);

    Trace trace_file_io("file_io", [](size_t n) {
        std::string filename = "temp_io_test.txt";
        std::ofstream outFile(filename);

        // write n lines to a file
        for (size_t i = 0; i < n; ++i) {
            outFile << "This is line " << i << " of the test file.\n";
        }

        outFile.close();

        // read the file back
        std::ifstream inFile(filename);
        std::string line;
        while (std::getline(inFile, line)) {
            // dummy
        }

        inFile.close();

        // clean up
        std::remove(filename.c_str());
    }, 1000000, 2, 5);
    register_trace(trace_file_io);
}

void TraceDriver::time_trace(Trace& trace) {
    size_t n = trace.min_;
    size_t i = 0;
    std::vector<size_t> data;
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

void TraceDriver::run_trace(Trace& trace) {
    // no timing overhead
    size_t n = trace.min_;
    size_t i = 0;
    while (i < trace.iterations_) {
        trace.trace_(n);
        n *= trace.factor_;
        ++i;
    }
}

void TraceDriver::run_traces(std::vector<std::string>& user_traces, bool time) {
    if (user_traces.empty()) {
        for (auto& it : all_traces) {
            time ? time_trace(it.second) : run_trace(it.second);
        }
    } else {
        for (auto& trace_name : user_traces) {
            auto it = all_traces.find(trace_name);
            if (it != all_traces.end()) {
                time ? time_trace(it->second) : run_trace(it->second);
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
        
        std::string n_iterations, durations;
        for (size_t i = 0; i < res.second.size(); i++) {
            if (i % 2 == 0) {
                if (!n_iterations.empty()) n_iterations += ",";
                n_iterations += std::to_string(res.second[i]);
            } else {
                if (!durations.empty()) durations += ",";
                durations += std::to_string(res.second[i]);
            }
        }

        file << "/" << n_iterations << "/" << durations << std::endl;
    }

    file.close();
}