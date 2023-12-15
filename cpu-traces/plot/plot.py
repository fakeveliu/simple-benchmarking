import matplotlib.pyplot as plt

OUT_FILE = 'output.txt'

OUT_M1 = 'output-m1.txt'
OUT_INTEL = 'output-intel.txt'

def plot_output():
    plt.xlabel('N')
    plt.ylabel('Time (ms)')
    
    try:
        f = open(OUT_FILE, 'r')
    except Exception as e:
        print(e)
        return

    rows = f.readlines()
    for row in rows:
        trace = row.split('/')
        name = trace[0]
        ns = [int(x) for x in trace[1].split(',')]
        times = [max(0.00001, float(x)) for x in trace[2].split(',')] # avoid log(0)
        plt.loglog(ns, times, 'o-', label=name)

    plt.legend()
    plt.show()

def plot_compare():
    plt.xlabel('N')
    plt.ylabel('Time (ms)')

    try:
        f_m1 = open(OUT_M1, 'r')
        f_intel = open(OUT_INTEL, 'r')
    except Exception as e:
        print(e)
        return
    
    rows_m1 = f_m1.readlines()
    rows_intel = f_intel.readlines()
    for i in range(len(rows_m1)):
        trace_m1, trace_intel = rows_m1[i].split('/'), rows_intel[i].split('/')
        name = trace_m1[0]
        Ns_m1 = [int(x) for x in trace_m1[1].split(',')]
        Ns_intel = [int(x) for x in trace_intel[1].split(',')]
        times_m1 = [max(0.00001, float(x)) for x in trace_m1[2].split(',')] # avoid log(0)
        times_intel = [max(0.00001, float(x)) for x in trace_intel[2].split(',')]
        plt.plot(Ns_m1, times_m1, 'o-', label='ARM')
        plt.plot(Ns_intel, times_intel, 'o-', label='x86')

        plt.xscale('log')
        plt.title(name)
        plt.legend()
        plt.show()

if __name__ == '__main__':
    # plot_output()
    plot_compare()