import matplotlib.pyplot as plt
import numpy as np
with open("time.txt", "r") as f:
    data = [float(line.strip()) for line in f]
labels = ['Other-(priority=0)', 'RR-(priority=50)', 'Fifo-(priority)']
colors = ['blue', 'green', 'red']
y = np.arange(0, len(data))
bar_width = 1 
bars = plt.barh(y, data, height=bar_width, color=colors, alpha=0.7, tick_label=labels)
plt.xlabel('Values')
plt.ylabel('Datasets')
plt.title('Horizontal Bar Chart with Three Datasets')
x_ticks = np.arange(0, max(data) + 0.05, 0.05)
plt.xticks(x_ticks)
legend_labels = [plt.Line2D([0], [0], color=color, lw=4, label=label) for color, label in zip(colors, labels)]
plt.legend(handles=legend_labels)
plt.show()