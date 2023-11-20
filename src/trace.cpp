#include "trace.h"
#include <iostream>
#include <fstream>

TraceDriver::TraceDriver(const std::string& out_path) : out_path_(out_path) {
    Trace trace_loop("loop", [](size_t n){
        for (size_t i = 0; i < n; ++i) {

        } 
    }, 10000, 10, 5);
    register_trace(trace_loop);

    Trace trace_threads("threads", [](size_t n) {
        std::vector<std::thread> threads;
        for (int i = 0; i < n; ++i) {
            threads.push_back(std::thread([](){}));
        }
        for (int i = 0; i < n; ++i) {
            threads[i].join();
        }
    }, 10, 10, 3);
    register_trace(trace_threads);
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