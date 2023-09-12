import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
from matplotlib.patches import Patch
x_labels = ["SCHED OTHER", "SCHED RR", "SCHED FIFO"]
l = []

with open("time.txt", "r") as f:
    a = "abc"
    while a:
        try:
            a = float(f.readline()[0:-1])
            l.append(a)
        except:
            break

colors = ['blue', 'red', 'green']
plt.bar(x_labels, l, color=colors, alpha=0.7)
plt.xlabel('Scheduling Policies')
plt.ylabel('Time')
plt.title('Scheduling Policy vs Time')
plt.yticks([i * 0.05 for i in range(int(max(l) / 0.05) + 5)])
legend_elements = [Line2D([0], [0], color='blue', lw=2, label='SCHED OTHER'),
                   Line2D([0], [0], color='red', lw=2, label='SCHED RR'),
                   Line2D([0], [0], color='green', lw=2, label='SCHED FIFO')]

plt.legend(handles=legend_elements)
plt.show()