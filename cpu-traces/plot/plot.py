import matplotlib.pyplot as plt

OUT_FILE = 'output.txt'

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

if __name__ == '__main__':
    plot_output()