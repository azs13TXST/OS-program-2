import matplotlib.pyplot as plt
import pandas as pd

#graphs the logged graphData
#plt.style.use('fivethirtyeight')
plt.figure(figsize=(5.5, 3))
data = pd.read_csv('log.txt', usecols = [0, 6, 7, 8, 9], names = ['Lambda','Turn Around Time','Throughput','Utilization','Response Time'], lineterminator=';', skipinitialspace=True);
x = data['Lambda']
y1 = data['Turn Around Time']
y2 = data['Throughput']
y3 = data['Utilization']
y4 = data['Response Time']

line1, = plt.plot(x, y1, "-o", label='Turn Around Time')
line2, = plt.plot(x, y2, "-o", label='Throughput')
line3, = plt.plot(x, y3, "-o", label='Utilization')
line4, = plt.plot(x, y4, "-o", label='Response Time')
plt.show

legend = plt.legend(loc='upper left')
plt.xticks(x[::1], rotation=90)

plt.xlabel("Lambda")  
plt.title('Most Recent Scheduling Algorithm')
plt.tight_layout()
plt.show()


