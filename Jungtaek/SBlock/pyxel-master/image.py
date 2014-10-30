from PIL import Image  # need to install PIL
import sys

# User input image file name
# ex) python image.py true.png 1   : it will make trueimg.h
# ex) python image.py false.png 2  : it will make falseimg.h
# ex) python image.py wait.png 0  : it will make waitimg.h

if len(sys.argv)!=3:
    print("Image filename and true/false option(1/2) needed")
    sys.exit()

i = Image.open(sys.argv[1])

# fixed size since LCD size is fixed (320 * 240)
width=320
height=240

# .h file would be created (the header file only include rgb565 format array)
filename=sys.argv[1].split('.',1)[0];
if(int(sys.argv[2])==1):
	file = open("trueimg.h","w")
	file.write("const u16 trueimg["+str(width*height)+"] = {");
elif(int(sys.argv[2])==2):
	file = open("falseimg.h","w")
	file.write("const u16 falseimg["+str(width*height)+"] = {");
else:
	file = open("waitimg.h","w")
	file.write("const u16 waitimg["+str(width*height)+"] = {");

im=i.resize((width,height), Image.NEAREST) # https://www.daniweb.com/software-development/python/code/216637/resize-an-image-python
pixels = im.load()

# write pixel info as rgb565 format to header file
for x in range(width):
    for y in range(height):
        cpixel = pixels[width-1-x, y]
        hexrgb=hex((((cpixel[0]&0xf8)>>3)<<11)|(((cpixel[1]&0xfc)>>2)<<5)|(((cpixel[2]&0xf8)>>3)))
        file.write(hexrgb)
        if(x*height+y!=width*height-1):
            file.write(',')

# close
file.write("};")
if(int(sys.argv[2])==1):
	print("trueimg.h is successfully created!")
elif(int(sys.argv[2])==2):
	print("falseimg.h is successfully created!")
else:
	print("waitimg.h is successfully created!")
file.close()

