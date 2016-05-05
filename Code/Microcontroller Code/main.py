# import the necessary packages
import argparse
import datetime
import imutils
import time
import cv2
import RPi.GPIO as GPIO
import pigpio
from PIL import Image
import numpy as np
from activation_functions import sigmoid_function, tanh_function
from cost_functions import sum_squared_error
from neuralnet import NeuralNet
from tools import Instance

servos = [24,17]
pi=pigpio.pi()

basewidth = 256
height = 192
#~ 
#~ def updateTop(angle):
	#~ dutyTop = float(angle) / 10.0 + 2.5
        #~ pwmTop.ChangeDutyCycle(dutyTop)
#~ 
#~ def updateBottom(angle):
	#~ dutyBottom = float(angle) / 10.0 + 2.5
        #~ pwmBottom.ChangeDutyCycle(dutyBottom)

def goToBin(bin):
	if bin is 1:
		pi.set_servo_pulsewidth(servos[1], 500)
	elif bin is 2:
		pi.set_servo_pulsewidth(servos[1], 1500)
	elif bin is 3:
		pi.set_servo_pulsewidth(servos[1], 2500)
	time.sleep(2)

def drop():
	pi.set_servo_pulsewidth(servos[0], 1500)
	time.sleep(2)
	pi.set_servo_pulsewidth(servos[0], 500)
	time.sleep(2)

def dropIn(bin):
	goToBin(bin)
	drop()
	
def findBin(frame):
	image=cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
	pil_im = Image.fromarray(image)
	#~ img1 = pil_im.resize((basewidth, height), Image.ANTIALIAS)
	pil_im.thumbnail((256, 256), Image.ANTIALIAS)
	img2 = pil_im.convert('1')
	#~ pixels = img2.load()
	pixels1 = np.asarray(img2.getdata(),dtype=np.bool)
	outstr = "outimg" +".bmp"
	img2.save(outstr)
	array01 = []
	count = 0
	Tot = 0
	for item in pixels1:
		Tot  += 1
		if not item:
			array01.append(1)
			count += 1
		else: 
			array01.append(0)
	testitem = []
	testitem.append(Instance(array01, [0]))
# load a stored network configuration
	network = NeuralNet.load_from_file( "plastic122.pkl" )
	arr = network.print_test(testitem)
	print('Value returned by neural network plastic: ' + str(arr[0]))
	
	network2 = NeuralNet.load_from_file( "metal122.pkl" )
	arr2 = network2.print_test(testitem)
	print('Value returned by neural network metal: ' + str(arr2[0]))
	
	network3 = NeuralNet.load_from_file( "paper122.pkl" )
	arr3 = network3.print_test(testitem)
	print('Value returned by neural network paper: ' + str(arr3[0]))
	
	pl = arr[0]
	me = arr2[0]
	pa = arr3[0]
	
	if((pl > pa and pl > me) or pl > 0.5 or (pa < 0.42 and me < 0.09) ):
		return 1 #plastic
	elif((me > pa and me > pl) or me > 0.13):
		return 3 #metal
	else:
		return 2 #paper
	#~ else:
		#~ return 3 #metal

def process(frame):
	bin=findBin(frame)
	print "found bin:"
	print bin
        dropIn(bin)        

while True:
	print "starting motion detection"
# construct the argument parser and parse the arguments
#~ ap = argparse.ArgumentParser()
#ap.add_argument("-v", "--video", help="path to the video file")
#~ ap.add_argument("-a", "--min-area", type=int, default=500, help="minimum area size")
#~ args = vars(ap.parse_args())

# if the video argument is None, then we are reading from webcam
#~ if args.get("video", None) is None:
	camera = cv2.VideoCapture(0)
	time.sleep(0.25)

# otherwise, we are reading from a video file
#~ else:
	#~ camera = cv2.VideoCapture(args["video"])

# initialize the first frame in the video stream
	firstFrame = None
	status="Unoccupied"
# loop over the frames of the video
	while True:
	# grab the current frame and initialize the occupied/unoccupied
	# text
		(grabbed, frame) = camera.read()
		text = "Unoccupied"
		origFrame=frame

	# if the frame could not be grabbed, then we have reached the end
	# of the video
		if not grabbed:
			break

	# resize the frame, convert it to grayscale, and blur it
		frame = imutils.resize(frame, width=500)
		gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
		gray = cv2.GaussianBlur(gray, (21, 21), 0)

	# if the first frame is None, initialize it
		if firstFrame is None:
			firstFrame = gray
			continue

	# compute the absolute difference between the current frame and
	# first frame
		frameDelta = cv2.absdiff(firstFrame, gray)
		thresh = cv2.threshold(frameDelta, 100, 255, cv2.THRESH_BINARY)[1]

	# dilate the thresholded image to fill in holes, then find contours
	# on thresholded image
		thresh = cv2.dilate(thresh, None, iterations=2)
		(cnts, _) = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL,
			cv2.CHAIN_APPROX_SIMPLE)

	# loop over the contours
		for c in cnts:
		# if the contour is too small, ignore it
		#~ if cv2.contourArea(c) < args["min_area"]:
			#~ continue

		# compute the bounding box for the contour, draw it on the frame,
		# and update the text
			(x, y, w, h) = cv2.boundingRect(c)
			cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
			text = "Occupied"

	# draw the text and timestamp on the frame
	#cv2.putText(frame, "Room Status: {}".format(text), (10, 20),
	#	cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
	#cv2.putText(frame, datetime.datetime.now().strftime("%A %d %B %Y %I:%M:%S%p"),
	#	(10, frame.shape[0] - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (0, 0, 255), 1)

	# show the frame and record if the user presses a key
	#cv2.imshow("Security Feed", frame)
	#cv2.imshow("Thresh", thresh)
	#cv2.imshow("Frame Delta", frameDelta)
		key = cv2.waitKey(1) & 0xFF

		if text is not status:
			print text
			status=text
			if status is "Occupied":
                        	process(origFrame)
				camera.release()
				break
              		else:
                        	print 0
	# if the `q` key is pressed, break from the lop
		if key == ord("q"):
			break


