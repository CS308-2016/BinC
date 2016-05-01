from activation_functions import sigmoid_function, tanh_function
#~ linear_function,\
                                 #~ LReLU_function, ReLU_function, elliot_function, symmetric_elliot_function, softmax_function
from cost_functions import sum_squared_error
#~ , cross_entropy_cost, exponential_cost, hellinger_distance, softmax_cross_entropy_cost
#~ from learning_algorithms import backpropagation, scaled_conjugate_gradient, scipyoptimize, resilient_backpropagation
from neuralnet import NeuralNet
from tools import Instance
import numpy as np

# Training sets

paper = 3
plastic = 3+3
metal = 10
lst = []

one = [1]
zero = [0]

for i in range(12):
	filename = "file" + str(i) + ".txt"
	fo = open(filename, "r")
	data = fo.read().replace('\n', '').replace("[", "").replace("]", "")
	data = np.fromstring(data, dtype=int, sep=',')
	#~ if i == 0:
		#~ print data
	if i < paper :
		lst.append(Instance(data, zero))
	elif i < plastic:
		lst.append(Instance(data, zero))
	else :
		lst.append(Instance(data, one))
training_one = []
for i in range(0,10):
	if i != 8:
		training_one.append(lst[i])

tst = []
tst.append(lst[10])
tst.append(lst[11])
#~ fo = open("outputtrainingone.txt", "w")
#~ fo.write(str(training_one))
#~ print(str(training_one[0]))

#~ settings = {
    #~ # Required settings
    #~ "cost_function"         : sum_squared_error,
    #~ "n_inputs"              : 65536,       # Number of network input signals
    #~ "layers"                : [ (20, tanh_function), (1, sigmoid_function) ],
                                        #~ # [ (number_of_neurons, activation_function) ]
                                        #~ # The last pair in you list describes the number of output signals
    #~ 
    #~ # Optional settings
    #~ "weights_low"           : -0.1,     # Lower bound on initial weight range
    #~ "weights_high"          : 0.1,      # Upper bound on initial weight range
    #~ "save_trained_network"  : True,    # Whether to write the trained weights to disk
    #~ 
    #~ "input_layer_dropout"   : 0.0,      # dropout fraction of the input layer
    #~ "hidden_layer_dropout"  : 0.0,      # dropout fraction in all hidden layers
#~ }

# initialize the neural network
#~ network = NeuralNet( settings )

# load a stored network configuration
network = NeuralNet.load_from_file( "network5.pkl" )

# Train the network using backpropagation
#~ backpropagation(
        #~ network,
        #~ training_one,          # specify the training set
        #~ ERROR_LIMIT     = 0.001, # define an acceptable error limit 
        #~ #max_iterations  = 100, # continues until the error limit is reach if this argument is skipped
                    #~ 
        #~ # optional parameters
        #~ learning_rate   = 0.03, # learning rate
        #~ momentum_factor = 0.4, # momentum
         #~ )
#~ 
#~ # Train the network using SciPy
#~ scipyoptimize(
        #~ network,
        #~ training_one, 
        #~ method = "Newton-CG",
        #~ ERROR_LIMIT = 1e-4
    #~ )
#~ 
#~ # Train the network using Scaled Conjugate Gradient
#~ scaled_conjugate_gradient(
        #~ network,
        #~ training_one, 
        #~ ERROR_LIMIT = 1e-4
    #~ )
#~ 
#~ # Train the network using resilient backpropagation
#~ resilient_backpropagation(
        #~ network,
        #~ training_one,          # specify the training set
        #~ ERROR_LIMIT     = 1e-3, # define an acceptable error limit
        #~ #max_iterations = (),   # continues until the error limit is reach if this argument is skipped
        #~ 
        #~ # optional parameters
        #~ weight_step_max = 50., 
        #~ weight_step_min = 0., 
        #~ start_step = 0.5, 
        #~ learn_max = 1.2, 
        #~ learn_min = 0.5
    #~ )


network.print_test( tst)
