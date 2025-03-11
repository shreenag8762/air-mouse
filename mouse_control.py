import serial
import pyautogui
import time

# Adjust COM port and baudrate
ser = serial.Serial('COM3', 115200, timeout=0.1)
pyautogui.FAILSAFE = False

while True:
    try:
        data = ser.readline().decode().strip()
        
        # Handle mouse movement
        if ',' in data:
            x, y = data.split(',')
            delta_x = int(float(x))
            delta_y = int(float(y))
            pyautogui.moveRel(delta_x, delta_y, _pause=False)
        
        # Handle button click
        elif data == "CLICK":
            pyautogui.click()
        
        # Handle scroll functionality
        elif data == "SCROLL_UP":
            pyautogui.scroll(3)  # Scroll up 3 steps
        elif data == "SCROLL_DOWN":
            pyautogui.scroll(-3) # Scroll down 3 steps
            
    except KeyboardInterrupt:
        break
    except:
        pass

ser.close()