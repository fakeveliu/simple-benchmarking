import matplotlib.pyplot as plt

OUT_FILE = 'output.txt'

OUT_M1 = 'output-m1.txt'
OUT_INTEL = 'output-intel.txt'
OUT_ADD_ARRAY = 'output-add-array.txt'

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
        times_m1 = [int(x) for x in trace_m1[2].split(',')] # avoid log(0)
        times_intel = [int(0.00001, float(x)) for x in trace_intel[2].split(',')]
        plt.plot(Ns_m1, times_m1, 'o-', label='ARM')
        plt.plot(Ns_intel, times_intel, 'o-', label='x86')

        plt.xscale('log')
        plt.title(name)
        plt.legend()
        plt.show()

def plot_add_array():
    plt.xlabel('N')
    plt.ylabel('Time (s)')

    try:
        f = open(OUT_ADD_ARRAY, 'r')
    except Exception as e:
        print(e)
        return
    
    rows = f.readlines()
    gpu_m1, cpu_m1, gpu_intel, cpu_intel = \
        rows[0].split('/'), rows[1].split('/'), rows[2].split('/'), rows[3].split('/')
    
    Ns = [int(x) for x in gpu_m1[1].split(',')]
    times_gpu_m1 = [float(x) for x in gpu_m1[2].split(',')]
    times_cpu_m1 = [float(x) for x in cpu_m1[2].split(',')]
    times_gpu_intel = [float(x) for x in gpu_intel[2].split(',')]
    times_cpu_intel = [float(x) for x in cpu_intel[2].split(',')]

    plt.plot(Ns, times_gpu_m1, 'o-', color='green', label='ARM, GPU')
    plt.plot(Ns, times_cpu_m1, 'x-', color='green', label='ARM, CPU')
    plt.plot(Ns, times_gpu_intel, 'o-', color='blue', label='x86, GPU')
    plt.plot(Ns, times_cpu_intel, 'x-', color='blue', label='x86, CPU')

    plt.xscale('log', base=2)
    plt.title('addArray')
    plt.legend()
    plt.show()


if __name__ == '__main__':
    # plot_output()
    # plot_compare()
    plot_add_array()