import serial as s
b1768, F746ZG = '/dev/tty.usbmodem432', '/dev/tty.usbmodem643'
measureBoard = s.Serial(b1768,9600, timeout = 5)
pwmGenerator = s.Serial(F746ZG, 9600, timeout = 20)
while(1):
    duty = input("Enter the duty cycle in binary as a percentage: ")
    duty = chr(int(duty,2)) #convert from binary, then to character
    period = input("Enter the period in binary: ")
    period = chr(int(period,2)) 
    pwmGenerator.write(duty.encode('utf-8'))
    pwmGenerator.write(period.encode('utf-8'))
    print("Board is processing data...")
    period = ""
    duty = ""
    counter = 0
    while(counter < 8): #reads each digit of the float individually
        duty += measureBoard.read().decode('utf-8')
        counter += 1
    counter = 0
    while(counter < 8): #reads each digit of the float individually
        period += measureBoard.read().decode('utf-8')
        counter += 1
    print("Duty cycle is: ")
    print(str(duty))
    print("Period is: ")
    print(str(period) + " seconds")
    
