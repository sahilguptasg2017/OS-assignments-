import matplotlib.pyplot as plt
import numpy as np
with open("time.txt", "r") as f:
    for x in f :
        y=x.split()
data=[]
for i in y:
    data.append(float(i))
labels = ['Other', 'RR', 'Fifo']
colors = ['blue', 'green', 'red']
y = np.arange(0, len(data))
bar_width = 1  
bars = plt.barh(y, data, height=bar_width, color=colors, alpha=0.7, tick_label=labels)
plt.xlabel('Time Taken')
plt.ylabel('Scheduling Policy')
plt.title('Histogram for scheduling policies')
x_ticks = np.arange(0, max(data) + 0.5, 0.5)
plt.xticks(x_ticks)
legend_labels = [plt.Line2D([0], [0], color=color, lw=4, label=label) for color, label in zip(colors, labels)]
plt.legend(handles=legend_labels)
plt.show()