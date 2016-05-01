import numpy as np
from PIL import Image

basewidth = 512
hsize = 512
img = Image.open('image.jpg')
img1 = img.resize((basewidth, hsize), Image.ANTIALIAS)
img2 = img1.convert('1')
pixels = img2.load()
pixels1 = np.asarray(img2.getdata(),dtype=np.bool)

print(pixels1)
count = 0
Tot = 0
for item in pixels1:
	Tot  += 1
	if not item:
		count += 1

print("Total Count %d, black %d"%(Tot,count))
img2.save('3.jpg')

#once we have image in the vector pixels use it for training the neural network


