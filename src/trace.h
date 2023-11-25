#include <string>
#include <functional>
#include <map>
#include <vector>
#include <thread>

class TraceDriver;

class Trace {
public:
    Trace(const std::string& name, const std::function<void(size_t n)>& trace,
        size_t min, size_t factor, size_t iterations) : 
        name_(name), trace_(trace), min_(min), factor_(factor), iterations_(iterations) {}

protected:
    const std::string name_;
    const std::function<void(size_t n)> trace_;
    size_t min_;
    size_t factor_;
    size_t iterations_;

private:
    friend TraceDriver;
};

class TraceDriver {
public:
    TraceDriver(const std::string& out_path);
    using TraceResult = std::pair<std::string, std::vector<size_t>>;
    void run_traces(std::vector<std::string>& user_traces);
    void format_output();

private:
    std::map<std::string, Trace> all_traces;
    std::vector<TraceResult> output;
    const std::string out_path_;
    
    void register_trace(Trace& trace) {
        all_traces.emplace(trace.name_, trace);
    }
    void time_trace(Trace& trace);
};