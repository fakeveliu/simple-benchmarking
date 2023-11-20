import matplotlib.pyplot as plt

OUT_FILE = 'output.txt'

def plot_output():
    plt.xlabel('n')
    plt.ylabel('time')
    
    try:
        f = open(OUT_FILE, 'r')
    except Exception as e:
        print(e)
        return

    rows = f.readlines()
    for row in rows:
        trace = row.split('/')
        name = trace[0]
        ns = trace[1].split(',')
        times = trace[2].split(',')
        plt.plot(ns, times, label=name)

    plt.legend()
    plt.show()

if __name__ == '__main__':
    plot_output()