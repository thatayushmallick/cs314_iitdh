# OPERATING-SYSTEM-LAB
## Assignment-1
#### Step-1
+ added padding as when a kernel is passed through a "HxW" matrix it shorts it to "H-2 X W-2", so a padding can change matrix to "H+2 X W+2".
#### Step-2
+ smoothening can be done when a (3x3) matrix of (1/9s) are passed over the padded image.
#### Step-3 
+ details can be extracted when smoothened image matrix is substracted from original matrix, keeping in mind that it should be between 0-255.
#### Step-4 
+ final sharpened image can be aquired by adding detailed matrix to original image.