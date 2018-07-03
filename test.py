import serial as s
lpc = s.Serial("/dev/tty.usbmodemFA132",9600, timeout = 5)
bigBoard = s.Serial('/dev/tty.usbmodemFD143', 9600, timeout = 20)
while(1):
    duty = input("Enter the duty cycle (1 for 10%, 2 for 20, etc): ")
    duty = int(duty,2)
    duty = chr(duty)
    period = input("Enter the period in milliseconds: ")
    period = int(period,2)
    period = chr(period)
    bigBoard.write(duty.encode('utf-8'))
    bigBoard.write(period.encode('utf-8'))
    print("Board is processing data...")
    period = ""
    duty = ""
    counter = 0
    while(counter < 8): #reads each digit of the float individually
        duty += lpc.read().decode('utf-8')
        counter += 1
    counter = 0
    while(counter < 8): #reads each digit of the float individually
        period += lpc.read().decode('utf-8')
        counter += 1
    print("Duty cycle is: ")
    print(str(duty))
    print("Period is: ")
    print(str(period) + " seconds")
