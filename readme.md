2016 - CS308  Group : Project BinC
================================================ 
 
Group Info: 
------------ 
+   Jaseem Umar M (120050081) 
+   Aman Gour (120050030)
+  Pintu Lal Meena (120050018)
+  Ravi Kumar Verma (120050027)

Project Description 
------------------- 
 
BINC is an electromechanical machine that helps in smart waste segregation by classifying waste into one of the three different categories: paper, plastic and metal.

 
Technologies Used 
------------------- 
+   Raspberry Pi
+   Python 
+   Python Image Library
+   PIGPIO
+   Tiva Launchpad
+   USB Webcam
+   Servo
+   OpenCV 
    
Installation Instructions 
=========================

Code in Microcontroller Code folder should be copied to Raspberry Pi.

Instructions to install required modules on Raspberry Pi:

OpenCV:

    sudo apt-get install python-opencv
    

Installing PIGPIO

    wget abyz.co.uk/rpi/pigpio/pigpio.zip
    unzip pigpio.zip
    cd PIGPIO
    make
    make install

Starting PIGPIO daemon:

    sudo pigpiod

Installing Imutils:

    sudo pip install imutils

Run the program:

    python main.py
    
Demonstration Video 
=========================  
https://www.youtube.com/watch?v=LT5wMNyYqyg


References 
=========== 

+   Neural Network in python (https://github.com/jorgenkg/python-neural-network)
+   Motion Detection in Raspberry Pi (http://www.pyimagesearch.com/2015/05/25/basic-motion-detection-and-tracking-with-python-and-opencv/)
+   PIGPIO for servo control (https://raspberrypiwonderland.wordpress.com/2014/02/19/servo-test/) 

