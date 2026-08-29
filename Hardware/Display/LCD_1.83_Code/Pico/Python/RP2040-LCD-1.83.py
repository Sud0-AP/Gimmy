from machine import Pin,SPI,PWM

import framebuf
import time

DC = 8
CS = 9
SCK = 10
MOSI = 11
RST = 12
BL = 13

class LCD_1inch83(framebuf.FrameBuffer):
    def __init__(self):
        self.width = 240
        self.height = 300
        
        self.cs = Pin(CS,Pin.OUT)
        self.rst = Pin(RST,Pin.OUT)
        
        self.cs(1)
        self.spi = SPI(1,40_000_000,polarity=0, phase=0,sck=Pin(SCK),mosi=Pin(MOSI),miso=None)
        self.dc = Pin(DC,Pin.OUT)
        self.dc(1)
        self.buffer = bytearray(self.width *  self.height * 2)
        super().__init__(self.buffer, self.width, self.height, framebuf.RGB565)
        self.init_display()
        
        self.red   =   0x07E0
        self.green =   0x001f
        self.blue  =   0xf800
        self.white =   0xffff
        
        self.fill(self.white)
        self.show()

        self.pwm = PWM(Pin(BL))
        self.pwm.freq(5000)
        
    def write_cmd(self, cmd):
        self.cs(1)
        self.dc(0)
        self.cs(0)
        self.spi.write(bytearray([cmd]))
        self.cs(1)

    def write_data(self, buf):
        self.cs(1)
        self.dc(1)
        self.cs(0)
        self.spi.write(bytearray([buf]))
        self.cs(1)
    def set_bl_pwm(self,duty):
        self.pwm.duty_u16(duty)#max 65535
    def init_display(self):
        """Initialize dispaly"""  
        self.rst(1)
        time.sleep(0.01)
        self.rst(0)
        time.sleep(0.01)
        self.rst(1)
        time.sleep(0.05)
        
        self.write_cmd(0x36)
        self.write_data(0x08)

        self.write_cmd(0xfd)
        self.write_data(0x06)
        self.write_data(0x08)

        self.write_cmd(0x61)
        self.write_data(0x07)
        self.write_data(0x04)

        self.write_cmd(0x62)
        self.write_data(0x00)
        self.write_data(0x44)
        self.write_data(0x45)

        self.write_cmd(0x63)
        self.write_data(0x41)
        self.write_data(0x07)
        self.write_data(0x12)
        self.write_data(0x12)

        self.write_cmd(0x64)
        self.write_data(0x37)
   
        self.write_cmd(0x65)
        self.write_data(0x09)
        self.write_data(0x10)
        self.write_data(0x21)
     
        self.write_cmd(0x66) 
        self.write_data(0x09) 
        self.write_data(0x10) 
        self.write_data(0x21)
      
        self.write_cmd(0x67)
        self.write_data(0x20)
        self.write_data(0x40)

       
        self.write_cmd(0x68)
        self.write_data(0x90)
        self.write_data(0x4c)
        self.write_data(0x7C)
        self.write_data(0x66)

        self.write_cmd(0xb1)
        self.write_data(0x0F)
        self.write_data(0x02)
        self.write_data(0x01)

        self.write_cmd(0xB4)
        self.write_data(0x01) 
       
        self.write_cmd(0xB5)
        self.write_data(0x02)
        self.write_data(0x02)
        self.write_data(0x0a)
        self.write_data(0x14)

        self.write_cmd(0xB6)
        self.write_data(0x04)
        self.write_data(0x01)
        self.write_data(0x9f)
        self.write_data(0x00)
        self.write_data(0x02)
  
        self.write_cmd(0xdf)
        self.write_data(0x11)

        self.write_cmd(0xE2)	
        self.write_data(0x13)
        self.write_data(0x00) 
        self.write_data(0x00)
        self.write_data(0x30)
        self.write_data(0x33)
        self.write_data(0x3f)

        self.write_cmd(0xE5)	
        self.write_data(0x3f)
        self.write_data(0x33)
        self.write_data(0x30)
        self.write_data(0x00)
        self.write_data(0x00)
        self.write_data(0x13)

        self.write_cmd(0xE1)	
        self.write_data(0x00)
        self.write_data(0x57)

        self.write_cmd(0xE4)	
        self.write_data(0x58)
        self.write_data(0x00)

        self.write_cmd(0xE0)
        self.write_data(0x01)
        self.write_data(0x03)
        self.write_data(0x0e)
        self.write_data(0x0e)
        self.write_data(0x0c)
        self.write_data(0x15)
        self.write_data(0x19)

        self.write_cmd(0xE3)	
        self.write_data(0x1a)
        self.write_data(0x16)
        self.write_data(0x0C)
        self.write_data(0x0f)
        self.write_data(0x0e)
        self.write_data(0x0d)
        self.write_data(0x02)
        self.write_data(0x01)
        
        self.write_cmd(0xE6)
        self.write_data(0x00)
        self.write_data(0xff)

        self.write_cmd(0xE7)
        self.write_data(0x01)
        self.write_data(0x04)
        self.write_data(0x03)
        self.write_data(0x03)
        self.write_data(0x00)
        self.write_data(0x12)

        self.write_cmd(0xE8) 
        self.write_data(0x00) 
        self.write_data(0x70) 
        self.write_data(0x00)
       
        self.write_cmd(0xEc)
        self.write_data(0x52)

        self.write_cmd(0xF1)
        self.write_data(0x01)
        self.write_data(0x01)
        self.write_data(0x02)


        self.write_cmd(0xF6)
        self.write_data(0x09)
        self.write_data(0x10)
        self.write_data(0x00)
        self.write_data(0x00)

        self.write_cmd(0xfd)
        self.write_data(0xfa)
        self.write_data(0xfc)

        self.write_cmd(0x3a)
        self.write_data(0x05)

        self.write_cmd(0x35)
        self.write_data(0x00)


        self.write_cmd(0x21)

        self.write_cmd(0x11)
        time.sleep(0.2)
        self.write_cmd(0x29)
        time.sleep(0.01)




    def show(self):
        self.write_cmd(0x2A)
        self.write_data(0x00)
        self.write_data(0x00)
        self.write_data(0x00)
        self.write_data(0xef)
        
        self.write_cmd(0x2B)
        self.write_data(0x00)
        self.write_data(0x00)
        self.write_data(0x01)
        self.write_data(0x2b)
        
        self.write_cmd(0x2C)
        
        self.cs(1)
        self.dc(1)
        self.cs(0)
        self.spi.write(self.buffer)
        self.cs(1)


if __name__=='__main__':
  
    LCD = LCD_1inch83()
    LCD.set_bl_pwm(65535)
    
    LCD.fill(LCD.white)
    LCD.text("RP2040-LCD-1.83",60,125,LCD.green)
    LCD.fill_rect(0,40,240,40,LCD.blue)
    LCD.text("Waveshare",80,57,LCD.white)
    
    
    LCD.show()



