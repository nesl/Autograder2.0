
    /*This function is used to open the USB device, select the configuration, and 
    to select the interface. Await is followed with a promise. This means that the
    promise will carry out and the function will wait until that promised is fulfilled.
    You can store the value of this promise in a variable if desired.*/
    async function connectDev(device)
    {
        console.log('Opening...');
        //This function will wait until the device is open
        await device.open();
        //Checks if the device is opened 
        if(!device.opened) 
            console.log('Could Not Open');
        else{
            console.log('Opened Successfully');
            //Check if the device is configured
            if (device.configuration == null){ 
                console.log('Selecting Configuration...');
                //The function waits until the configuration is set
                await device.selectConfiguration(1);
                console.log('Configuration Selected');
            }
            console.log('Claiming Intereface...');
            //The function waits until the interface is claimed
            await device.claimInterface(2);
            console.log('Interface Claimed');
        }

    }
    //CHANGED
    /*This function is used to receive data from the device.*/
    async function receiveData(device)
    {
        //Make the device ready to receive data
        await device.controlTransferOut({
            requestType: 'class',
            recipient: 'interface',
            request: 0x22,
            value: 0x01,
            index: 0x02
        });
        console.log('Receiving Data...');
        //Waiting for 64bytes of data from endpoint #5, store that data in result
        let result = await device.transferIn(5,64);
        console.log('Data Received');
        //Decode and print the message
        let decoder = new TextDecoder();
        console.log('Received: ' + decoder.decode(result.data));
    }
    /*This function is used to send data to the device*/
    async function sendData(device, u_input)
    {
        //Make the device ready to send data
        await device.controlTransferIn({
            requestType: 'class',
            recipient: 'interface',
            request: 0x22,
            value: 0x01,
            index: 0x02
        }, 8);
        console.log('Sending Data...');
        //Waiting for 64bytes of data from endpoint #5, store that data in result
        var buffer = new ArrayBuffer(8);
        /*buffer[0] = 'L';
        buffer[1] = 'O';
        buffer[2] = 'G';
        buffer[3] = 'O';
        buffer[4] = 'N';
        buffer[5] = 'T';
        buffer[6] = 'S';
        buffer[7] = 'T';
        May not need, as textencoder should handle this
        */

        console.log('Logontst was sent as a message.');
        let encoder = new TextEncoder();
        buffer = encoder.encode(u_input);
        await device.transferOut(5,buffer);
        console.log('Data Successfully Sent');
        //Decode and print the message
        /*let decoder = new TextDecoder();
        console.log('Sent: ' + decoder.decode(result.data));*/
        //await receiveData(device); //ONLY UNCOMMENT TO LINK SENDING AND RECEIVING DATA
    }

    /*This function is used to close the device*/
    async function closeDev(device)
    {
        console.log('Closing...')
        await device.close();
        if(device.opened)
            console.log('Device did not close');
        else
            console.log('Device closed');
    }


    /*Used to check if there are devices alreay paired from before*/
    document.addEventListener('DOMContentLoaded', async () => {
        let devices = await navigator.usb.getDevices();
        devices.forEach(device => {
        console.log('Get Devices pass');// Add |device| to the UI.
        });
    });

    /*Used to detect when a USB device is connected. NOTE: Connected means that
    a new USB device is connected to the PC, not that a device is opened by WebUSB.*/
    navigator.usb.addEventListener('connect', event => {
        console.log('Device connected');// Add |event.device| to the UI.
    });

    /*Used to detect when a USB device is disconnected, look at note above.*/
    navigator.usb.addEventListener('disconnect', event => {
        console.log('Device disconencted');// Remove |event.device| from the UI.
    });

    /*Defining what happens when the button is clicked*/
    let receive = document.getElementById('request-device');
    //Check that the button exists
    if(receive){
        //What happens when the button is clicked
        receive.addEventListener('click', async() => {
            document.getElementById('demo').innerHTML = "Paragraph changed.";
            let device;
            try {
                //Have user select device
                device = await navigator.usb.requestDevice({filters: [{vendorId:0x1F00}]})
                //Connect the device
                await connectDev(device);
                //Receive data from device
                await receiveData(device);
                //await receiveData(device);
                //await receiveData(device);
                //await receiveData(device);
                //await receiveData(device);
                //await closeDev(device); 
            } catch (err){
                console.log('No Device was selected in receive');//No device was selected.
            }
        })
    }

    let send = document.getElementById('test');
    if(send){
        send.addEventListener('click', async() => {
            document.getElementById('demo').innerHTML = "Entered Here";
            let device;
            var period = document.getElementById('per').value;
            var dutyCycle = document.getElementById('dutyCycle').value;            try{
                device = await navigator.usb.requestDevice({filters: [{vendorId:0x1F00}]})
                await connectDev(device);
                await sendData(device,period); //Currently, sendData also receives data to test it
                await sendData(device,dutyCycle);
                await receiveData(device);
                await receiveData(device);
                await closeDev(device);
            } catch(err){
                console.log('No Device was selected' + err);
            }
        })
    }
