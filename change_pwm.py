import serial as s
import time

smallBoard = s.Serial('/dev/tty.usbmodem432', 9600, timeout = 15)
bigBoard = s.Serial('/dev/tty.usbmodem643', 9600)
print(smallBoard.name + " is available.")
print(bigBoard.name + " is available.")
while(1):
	print("Enter your required duty cycle: ")
	duty = input()
	print("Enter the period: ")
	period = input()
	bigBoard.write(duty.encode('utf-8'))
	bigBoard.write(period.encode('utf-8'))
	print("Board is processing data...")
	newDuty = smallBoard.read()
	print("Here")
	#print(newDuty)
	newDuty = newDuty.decode('utf-8')
	print ("The received duty cycle is " + newDuty)
	newDuty = ord(newDuty)
	print(newDuty)
	newPeriod = smallBoard.read()
	newPeriod = newPeriod.decode('utf-8')
	newPeriod = ord(newPeriod)
	print ("The received period is %d.", newPeriod)
	print("The duty cycle was %d percent and the period was %d seconds.",newDuty,newPeriod)
	time.sleep(2)





