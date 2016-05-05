from neuralnet import NeuralNet
from tools import Instance
import numpy as np
paper = 3
plastic = 3+3
metal = 10
lst = []

one = [1]
zero = [0]

for i in range(10):
	filename = "file" + str(i) + ".txt"
	fo = open(filename, "r")
	data = fo.read().replace('\n', '').replace("[", "").replace("]", "")
	data = np.fromstring(data, dtype=int, sep=',')
	#~ if i == 0:
		#~ print data
	if i < paper :
		lst.append(Instance(data, one))
	elif i < plastic:
		lst.append(Instance(data, zero))
	else :
		lst.append(Instance(data, zero))
		
print(lst)
