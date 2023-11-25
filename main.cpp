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

    args.add_option("--run-traces", trace_names, "Run specified traces");

    CLI11_PARSE(args, argc, argv);

    TraceDriver td(out_path);
    td.run_traces(trace_names);
    td.format_output();
}
