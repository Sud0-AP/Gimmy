
import time
from . import lcdconfig

class LCD_1inch83(lcdconfig.RaspberryPi):
    width = 240
    height = 280 
    
    def command(self, cmd):
        self.digital_write(self.DC_PIN, False)
        self.spi_writebyte([cmd])   
        
    def data(self, val):
        self.digital_write(self.DC_PIN, True)
        self.spi_writebyte([val])   
        
    def reset(self):
        """Reset the display"""
        self.digital_write(self.RST_PIN,True)
        time.sleep(0.01)
        self.digital_write(self.RST_PIN,False)
        time.sleep(0.01)
        self.digital_write(self.RST_PIN,True)
        time.sleep(0.01)
        
    def Init(self):
        """Initialize dispaly""" 
        self.module_init()
        self.reset()

        self.command(0x36)
        self.data(0x08)

        self.command(0xfd)
        self.data(0x06)
        self.data(0x08)

        self.command(0x61)
        self.data(0x07)
        self.data(0x04)

        self.command(0x62)
        self.data(0x00)
        self.data(0x44)
        self.data(0x45)

        self.command(0x63)
        self.data(0x41)
        self.data(0x07)
        self.data(0x12)
        self.data(0x12)

        self.command(0x64)
        self.data(0x37)
   
        self.command(0x65)
        self.data(0x09)
        self.data(0x10)
        self.data(0x21)
     
        self.command(0x66) 
        self.data(0x09) 
        self.data(0x10) 
        self.data(0x21)
      
        self.command(0x67)
        self.data(0x20)
        self.data(0x40)

       
        self.command(0x68)
        self.data(0x90)
        self.data(0x4c)
        self.data(0x7C)
        self.data(0x66)

        self.command(0xb1)
        self.data(0x0F)
        self.data(0x02)
        self.data(0x01)

        self.command(0xB4)
        self.data(0x01) 
       
        self.command(0xB5)
        self.data(0x02)
        self.data(0x02)
        self.data(0x0a)
        self.data(0x14)

        self.command(0xB6)
        self.data(0x04)
        self.data(0x01)
        self.data(0x9f)
        self.data(0x00)
        self.data(0x02)
  
        self.command(0xdf)
        self.data(0x11)

        self.command(0xE2)	
        self.data(0x13)
        self.data(0x00) 
        self.data(0x00)
        self.data(0x30)
        self.data(0x33)
        self.data(0x3f)

        self.command(0xE5)	
        self.data(0x3f)
        self.data(0x33)
        self.data(0x30)
        self.data(0x00)
        self.data(0x00)
        self.data(0x13)

        self.command(0xE1)	
        self.data(0x00)
        self.data(0x57)

        self.command(0xE4)	
        self.data(0x58)
        self.data(0x00)

        self.command(0xE0)
        self.data(0x01)
        self.data(0x03)
        self.data(0x0e)
        self.data(0x0e)
        self.data(0x0c)
        self.data(0x15)
        self.data(0x19)

        self.command(0xE3)	
        self.data(0x1a)
        self.data(0x16)
        self.data(0x0C)
        self.data(0x0f)
        self.data(0x0e)
        self.data(0x0d)
        self.data(0x02)
        self.data(0x01)
        
        self.command(0xE6)
        self.data(0x00)
        self.data(0xff)

        self.command(0xE7)
        self.data(0x01)
        self.data(0x04)
        self.data(0x03)
        self.data(0x03)
        self.data(0x00)
        self.data(0x12)

        self.command(0xE8) 
        self.data(0x00) 
        self.data(0x70) 
        self.data(0x00)
       
        self.command(0xEc)
        self.data(0x52)

        self.command(0xF1)
        self.data(0x01)
        self.data(0x01)
        self.data(0x02)


        self.command(0xF6)
        self.data(0x09)
        self.data(0x10)
        self.data(0x00)
        self.data(0x00)

        self.command(0xfd)
        self.data(0xfa)
        self.data(0xfc)

        self.command(0x3a)
        self.data(0x05)

        self.command(0x35)
        self.data(0x00)


        self.command(0x21)

        self.command(0x11)
        time.sleep(0.2)
        self.command(0x29)
        time.sleep(0.01)
  
    def SetWindows(self, Xstart, Ystart, Xend, Yend, horizontal = 0):
        if horizontal:  
            #set the X coordinates
            self.command(0x2A)
            self.data(Xstart+20>>8)         #Set the horizontal starting point to the high octet
            self.data(Xstart+20 & 0xff)     #Set the horizontal starting point to the low octet
            self.data(Xend+20-1>>8)         #Set the horizontal end to the high octet
            self.data((Xend+20-1) & 0xff)   #Set the horizontal end to the low octet 
            #set the Y coordinates
            self.command(0x2B)
            self.data(Ystart>>8)
            self.data((Ystart & 0xff))
            self.data(Yend-1>>8)
            self.data((Yend-1) & 0xff)
            self.command(0x2C)
        else:
            #set the X coordinates
            self.command(0x2A)
            self.data(Xstart>>8)        #Set the horizontal starting point to the high octet
            self.data(Xstart & 0xff)    #Set the horizontal starting point to the low octet
            self.data(Xend-1>>8)        #Set the horizontal end to the high octet
            self.data((Xend-1) & 0xff)  #Set the horizontal end to the low octet 
            #set the Y coordinates
            self.command(0x2B)
            self.data(Ystart+20>>8)
            self.data((Ystart+20 & 0xff))
            self.data(Yend+20-1>>8)
            self.data((Yend+20-1) & 0xff)
            self.command(0x2C)    


    def ShowImage(self, Image):
        """Set buffer to value of Python Imaging Library image."""
        """Write display buffer to physical display"""
        imwidth, imheight = Image.size
        if imwidth == self.height and imheight ==  self.width:
            print("Landscape screen")
            img = self.np.asarray(Image)
            pix = self.np.zeros((self.width, self.height,2), dtype = self.np.uint8)
            #RGB888 >> RGB565
            pix[...,[0]] = self.np.add(self.np.bitwise_and(img[...,[0]],0xF8),self.np.right_shift(img[...,[1]],5))
            pix[...,[1]] = self.np.add(self.np.bitwise_and(self.np.left_shift(img[...,[1]],3),0xE0), self.np.right_shift(img[...,[2]],3))
            pix = pix.flatten().tolist()
            
            self.command(0x36)
            self.data(0x78)
            self.SetWindows(0, 0, self.height,self.width, 1)
            self.digital_write(self.DC_PIN,True)
            for i in range(0,len(pix),4096):
                self.spi_writebyte(pix[i:i+4096])
        else :
            print("Portrait screen")
            img = self.np.asarray(Image)
            pix = self.np.zeros((imheight,imwidth , 2), dtype = self.np.uint8)
            
            pix[...,[0]] = self.np.add(self.np.bitwise_and(img[...,[0]],0xF8),self.np.right_shift(img[...,[1]],5))
            pix[...,[1]] = self.np.add(self.np.bitwise_and(self.np.left_shift(img[...,[1]],3),0xE0), self.np.right_shift(img[...,[2]],3))
            pix = pix.flatten().tolist()
            
            self.command(0x36)
            self.data(0x08)
            self.SetWindows(0, 0, self.width, self.height, 0)
            self.digital_write(self.DC_PIN,True)
        for i in range(0, len(pix), 4096):
            self.spi_writebyte(pix[i: i+4096])
        

    def clear(self):
        """Clear contents of image buffer"""
        _buffer = [0xff] * (self.width*self.height*2)
        self.SetWindows(0, 0, self.width, self.height)
        self.digital_write(self.DC_PIN,True)
        for i in range(0, len(_buffer), 4096):
            self.spi_writebyte(_buffer[i: i+4096])
        
