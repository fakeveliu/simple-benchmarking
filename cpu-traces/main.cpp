#include "deps/CLI11.hpp"
#include "src/trace.h"

const std::string out_path = "plot/output.txt";

void format_output(std::map<std::string, long long>& times) {
    std::cout << "------------------------" << std::endl;
    std::cout << "      Trace Result      " << std::endl;
    std::cout << "------------------------" << std::endl;
    for (auto& p : times) {
        std::cout << p.first << ": " << p.second << "ms" << std::endl;
    }
    std::cout << "------------------------" << std::endl;
}

int main(int argc, char** argv) {
    CLI::App args{"Simple Trace App"};

    std::vector<std::string> trace_names;
    bool time = false;

    args.add_option("--run-traces", trace_names, "Run specified traces");
    args.add_flag("--time", time, "Use builtin timer");

    CLI11_PARSE(args, argc, argv);

    TraceDriver td(out_path);
    td.run_traces(trace_names, time);
    // td.format_output();
}
