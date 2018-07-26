//Wait until page is fully loaded
$(document).ready(function(){
    /*This function is used to open the USB device, select the configuration, and 
    to select the interface. Await is followed with a promise. This means that the
    promise will carry out and the function will wait until that promised is fulfilled.
    You can store the value of this promise in a variable if desired.*/
    async function connectDev(device){
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

    //------------------------------------------------------------------
    /*This function is used to receive data from device, function will return 
    the data received*/
    async function receiveData(device){
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
        return decoder.decode(result.data);
    }

    //-------------------------------------------------------------------
    /*This function is used to send data to the device, u_input is data to be sent*/
    async function sendData(device, u_input){
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
        let encoder = new TextEncoder();
        buffer = encoder.encode(u_input);
        await device.transferOut(5,buffer);
        console.log('Data Successfully Sent');
    }

    //-------------------------------------------------------------------
    /*This function is used to close the device*/
    async function closeDev(device){
        console.log('Closing...')
        await device.close();
        if(device.opened)
            console.log('Device did not close');
        else
            console.log('Device closed');
    }

    //-------------------------------------------------------------------
    /*This function disables/enables all buttons*/
    function disableButtons(flag){
        $(':button').prop('disabled',flag); //Selects all buttons, sets diasabled to flag
    }

    //-------------------------------------------------------------------
    /*Used to check if there are devices alreay paired from before*/
    document.addEventListener('DOMContentLoaded', async () => {
        let devices = await navigator.usb.getDevices();
        devices.forEach(device => {
        console.log('Device already connected');// Add |device| to the UI.
        });
    });

    //-------------------------------------------------------------------
    /*Used to detect when a USB device is connected. NOTE: Connected means that
    a new USB device is connected to the PC, not that a device is opened by WebUSB.*/
    navigator.usb.addEventListener('connect', event => {
        console.log('New device connected');// Add |event.device| to the UI.
    });

    //-------------------------------------------------------------------
    /*Used to detect when a USB device is disconnected, look at note above.*/
    navigator.usb.addEventListener('disconnect', event => {
        console.log('Device disconencted');// Remove |event.device| from the UI.
    });

    //-------------------------------------------------------------------    
    /*Define all buttons/variables beforehand*/
    let select = document.getElementById('select');
    let receive = document.getElementById('request-device');
    let send = document.getElementById('send');
    let runAll = document.getElementById('test-cases')
    let liveGraph = document.getElementById('liveGraph');
    let blinky1 = document.getElementById('blinky1');
    let blinky2 = document.getElementById('blinky2');
    let blinky3 = document.getElementById('blinky3');
    var SIG_FIGS = 5;
    var TIME_UNIT = 0.2; //ms
    var NUM_CYCLES = 10; //Used for plotly function
    let device;

    //-------------------------------------------------------------------
    /*Defining what happens when the blinky1 button is clicked. This is meant to
    simulate picking an assignment. It will tell the device what assignment is to
    be graded.*/
    if(blinky1){
        $(blinky1).click(async()=>{
            try{
                disableButtons(true);
                await connectDev(device);
                await sendData(device, '1'); //Indicate this is blinky1 assignment with 1
                await closeDev(device);
            } catch(err){
                console.log(err);
            }
            disableButtons(false);
        })
    }

    //-------------------------------------------------------------------
    /*Defining what happens when the blinky2 button is clicked. This is meant to
    simulate picking an assignment. It will tell the device what assignment is to
    be graded.*/
    if(blinky2){
        $(blinky2).click(async()=>{
            try{
                disableButtons(true);
                await connectDev(device);
                await sendData(device, '2'); //Indicate this is blinky2 assignment with 2
                await closeDev(device);
            } catch(err){
                console.log(err);
            }
            disableButtons(false);
        })
    }

    //-------------------------------------------------------------------
    /*Defining what happens when the blinky3 button is clicked. This is meant to
    simulate picking an assignment. It will tell the device what assignment is to
    be graded.*/
    if(blinky3){
        $(blinky3).click(async()=>{
            try{
                disableButtons(true);
                await connectDev(device);
                await sendData(device, '3'); //Indicate this is blinky3 assignment with 3
                await closeDev(device);
            } catch(err){
                console.log(err);
            }
            disableButtons(false);
        })
    }
    //-------------------------------------------------------------------
    /*Defining what happens when the select button is clicked. This is meant
    to select which device will be communicating with the browser*/
    if(select){
        $(select).click(async()=>{
            try{
                disableButtons(true);
                console.log("Device being selected");
                device = await navigator.usb.requestDevice({filters: [{vendorId:0x1F00}]});
                console.log('Device selected');
            } catch(err){
                console.log(err);
            }
            disableButtons(false);
        })
    }


    //-------------------------------------------------------------------
    /*Defining what happens when the receive button is clicked. This is meant
    to receive data from the test board and print to console*/
    //Check that the button exists
    if(receive){
        //What happens when the button is clicked
        $(receive).click(async() => {
            try {
                disableButtons(true);
                await connectDev(device);//Connect the device
                await receiveData(device);//Receive data from device
                await closeDev(device);//Close the device
            } catch (err){
                console.log(err);//Error occured
            }
            disableButtons(false);
        })
    }

    //-------------------------------------------------------------------
    /*Defines what happens when the send button is clicked. This is used to 
    send period and duty cycle (user input) to the test board and then receive the time 
    stamps measured by the test baord for the period and duty cycle.*/
    if(send){
        $(send).click(async() => {
            //Get values period and duty cycle entered by user
            var period = $('#per').val();
            var dutyCycle = $('#dutyCycle').val();            
            try{
                disableButtons(true);
                await connectDev(device);
                //Send period and duty cycle to device
                await sendData(device,period); //Currently, sendData also receives data to test it
                await sendData(device,dutyCycle);
                //Receive period and duty cycle measured from test board
                var perResult = await receiveData(device);
                var dutyResult = await receiveData(device);
                if(perResult.charAt(0) == 'T'){
                        $('#' + elementID).after('<div>There was a TIMEOUT ERROR while processing ' +
                            'test case ' + index + '. Please reset the tester board.</div>');
                }
                else{
                    var per = (parseFloat(perResult) * 1000).toFixed(SIG_FIGS);; //Convert from seconds to ms
                    var dCycle = (parseFloat(dutyResult) * 100).toFixed(SIG_FIGS);
                    $('#' + elementID).after(       '<div>Period received: ' + per + 'ms</div>' +
                            '<div>Duty Cycle received: ' + dCycle + '%</div>');
                }

                await closeDev(device);
            } catch(err){
                console.log(err);
            }
            disableButtons(false);
        })
    }

    //-------------------------------------------------------------------
    /*This is used to several test cases to the test board. The test cases are sent
    and then the time stamps are received by the test board. These time stamps are 
    formatted to be displayed on the browser.*/
    if (runAll){
        $(runAll).click(async () => {
            //let device;
            //Define all periods and duty cycles in binary
            var NUM_CASES = 5;
            var per1 = '01111'; //160ms
            var duty1 = '0110010'; //50%
            var per2 = '01011'; //120ms
            var duty2 = //'1000110'; //70%
            var per3 = '10110'; //230ms
            var duty3 = '1011010'; //90%
            var per4 = '11110'; //310ms
            var duty4 = '0001010'; //10%
            var per5 = '00010'; //30ms
            var duty5 = '0010001'; //17%
            var perList = [per1, per2, per3, per4, per5];
            var dutyList = [duty1,duty2,duty3,duty4,duty5]
            var perResults = [];
            var dutyResults = []
            var index = 1;  //Keeps track of what test case we are on
            var finalResult = '';
            var lastName = 'last'; //Used to store last name of student
            var firstName = 'first'; //Used to store first name of student
            disableButtons(true);
            try{
                //device = await navigator.usb.requestDevice({filters: [{vendorId:0x1F00}]});
                await connectDev(device);
                for(var i=0; i<NUM_CASES; i+=1){
                    var elementID = 'plotly-test' + index.toString(); //Get which test case this is
                    //Send period followed by duty cycle to test board
                    await sendData(device,'4'); //Tell device this is assignment 0
                    await sendData(device, perList[i]);
                    await sendData(device, dutyList[i]);
                    //Store time stamp of period then duty cycle to results
                    perResults[i] = await receiveData(device);
                    console.log('Received ' + perResults[i]);
                    dutyResults[i] = await receiveData(device);
                    console.log("Made it here on case " + index);
                    console.log(perResults[i]);
                    if(perResults[i].charAt(0) == 'T'){
                        $('#' + elementID).after('<div>There was a TIMEOUT ERROR while processing ' +
                            'test case ' + index + '.</div>');
                    }
                    else{
                        //Convert results into floats
                        var per = (parseFloat(perResults[i]) * 1000).toFixed(SIG_FIGS);; //Convert from seconds to ms
                        var dCycle = (parseFloat(dutyResults[i]) * 100).toFixed(SIG_FIGS); //Convert from decimal to percentage
                        var expectedPer = (parseInt(perList[i], 2) + 1)*10;
                        var expectedDuty = parseInt(dutyList[i],2);
                        var periodRemainder = getTimeUnits(expectedPer,per,TIME_UNIT).toFixed(SIG_FIGS);
                        var grade = gradeData(periodRemainder, expectedDuty, dCycle)
                        //finalResult is what will be saved on server side for teach access
                        finalResult += 'Test Case: ' + index + '\n' +
                            'Period received: ' + per + 'ms\n' +
                            'Duty Cycle received: ' + dCycle + '%\n' +
                            'Number of time units off: ' + periodRemainder + '\n' +
                            'Grade: ' + grade + '%\n' + '\n' + '\n' ;
                        //Append results to browser
                        $('#' + elementID).after('<div>Test Case: ' + index + '</div>' +
                            '<div>Period received: ' + per + 'ms</div>' +
                            '<div>Duty Cycle received: ' + dCycle + '%</div>' +
                            '<div>Number of time units off: ' + periodRemainder + '</div>' +
                            '<div>Grade: ' + grade + '%</div><br>'); 
                        graphPlotly(per, dCycle/100, elementID, index);
                    }
                    index++;
                }
                //Run recordGrades.php to save grades
                $.ajax({
                    type: 'POST', //POST to send data to php file
                    url: 'serverFiles/recordGrades.php', //what file to run
                    data: { fGrade: finalResult, l_name:lastName, f_name:firstName}, //what data to send
                    success: function(response) {     //Run this function if successful
                        console.log('Saved Results');
                    }
                });
                await closeDev(device);
            }
            catch(err){
                console.log(err);
            }

            index = 1; //Reset index for next time
            disableButtons(false);
        })
    }

    //-------------------------------------------------------------------
    /*This is used to several test cases to the test board. The test cases are sent
    and then the time stamps are received by the test board. These time stamps are 
    formatted to be displayed on the browser.*/
    if (liveGraph){
        $(liveGraph).click(async () => {
            //let device;
            //Define all periods and duty cycles in binary
            var NUM_CASES = 5;
            var per1 = '01111'; //160ms
            var duty1 = '0110010'; //50%
            var per2 = '01011'; //120ms
            var duty2 = '1000110'; //70%
            var per3 = '10110'; //230ms
            var duty3 = '1011010'; //90%
            var per4 = '11110'; //310ms
            var duty4 = '0001010'; //10%
            var per5 = '00010'; //30ms
            var duty5 = '0010001'; //17%
            var perList = [per1, per2, per3, per4, per5];
            var dutyList = [duty1,duty2,duty3,duty4,duty5];
            var onResults = [];
            var offResults = [];

            var index = 1;  //Keeps track of what test case we are on
            var finalResult = '';
            var lastName = 'last'; //Used to store last name of student
            var firstName = 'first'; //Used to store first name of student
            disableButtons(true);
            try{
                //device = await navigator.usb.requestDevice({filters: [{vendorId:0x1F00}]});
                await connectDev(device);
                for(var i=0; i<NUM_CASES; i+=1){
                    var onList = [];
                    var offList = [];
                    var elementID = 'plotly-test' + index.toString(); //Get which test case this is
                    //Send period followed by duty cycle to test board
                    await sendData(device,'0'); //Tell device this is assignment 0
                    await sendData(device, perList[i]);
                    await sendData(device, dutyList[i]);

                    while(true){
                        var timeOff = await receiveData(device);
                        if(timeOff.charAt(0) == 'S'){
                            break;
                        }
                        offList.push(timeOff);
                        var timeOn = await receiveData(device);
                        if(timeOn.charAt(0) == 'S'){
                            break;
                        }
                        onList.push(timeOn);
                    }
                    onResults.push(onList);
                    offResults.push(offList);
                    console.log('ONTIMES: ' + onResults[i]);
                    console.log('OFFTIMES: ' + offResults[i]);
                    var period = await receiveData(device);
                    console.log('Period: ' + period);
                    var dutyCylce = await receiveData(device);
                    console.log('Duty Cycle: ' + dutyCycle);
                    if(period.charAt(0) == 'T'){
                        $('#' + elementID).after('<div>There was a TIMEOUT ERROR while processing ' +
                            'test case ' + index + '.</div>');
                    }
                    
                    index++;
                }
                await closeDev(device);
            }
            catch(err){
                console.log(err);
            }

            index = 1; //Reset index for next time
            disableButtons(false);
        })
    }

    //-------------------------------------------------------------------
    /*This function will create a plotly graph.
    *@param {float} period - The period measured from the board
    *@param {float} dutyCycle - The duty cycle measured from the board
    *@param {string} id - The ID tag of the <div> element that will hold the graph
    *@param {int} index - The number of the test case, eg. Test case 1, Test case 2, etc
    */
    function graphPlotly(period, dutyCycle, id, index){
        let test = document.getElementById(id);
        var xAxis = [];
        var yAxis = [];
        //Creating the x values of the points
        for(var i=0; i<NUM_CYCLES;++i){
            xAxis.push(period*(i));
            xAxis.push(period*(i) + period*dutyCycle);
            xAxis.push(period*(i) + period*dutyCycle);
            xAxis.push(period*(i+1));
        }
        //Creating the y values of the points
        for(var i=0;i<NUM_CYCLES;++i)
        {
            yAxis.push(1);
            yAxis.push(1);
            yAxis.push(0);
            yAxis.push(0);
        }
        var layout = {
            title: 'Test Case ' + index.toString(),
            xaxis:{
                title: 'Time (ms)'
            }            
        };
        Plotly.plot(test, [{
            x: xAxis,
            y: yAxis 
            }],layout
            //{margin: {t:0}}    
        );
    }

    //-------------------------------------------------------------------
    /*This function will create a plotly graph.
    *@param {float} period - The period measured from the board
    *@param {float} dutyCycle - The duty cycle measured from the board
    *@param {string} id - The ID tag of the <div> element that will hold the graph
    *@param {int} index - The number of the test case, eg. Test case 1, Test case 2, etc
    */
    function tgraphPlotly(id, index){
        let test = document.getElementById(id);
        var xAxis = [];
        var yAxis = [];
        //Creating the x values of the points
        xAxis.push(0);
        xAxis.push(1);
        yAxis.push(0);
        yAxis.push(1);
        /*for(var i=0; i<NUM_CYCLES;++i){
            xAxis.push(period*(i));
            xAxis.push(period*(i) + period*dutyCycle);
            xAxis.push(period*(i) + period*dutyCycle);
            xAxis.push(period*(i+1));
        }
        //Creating the y values of the points
        for(var i=0;i<NUM_CYCLES;++i)
        {
            yAxis.push(1);
            yAxis.push(1);
            yAxis.push(0);
            yAxis.push(0);
        }*/
        var layout = {
            title: 'Test Case ' + index.toString(),
            xaxis:{
                title: 'Time (ms)'
            }            
        };
        Plotly.plot(test, [{
            x: xAxis,
            y: yAxis 
            }],layout
            //{margin: {t:0}}    
        );
    }

    //-------------------------------------------------------------------
    /*This function grades a set of received data
    *@param {Float} periodRemainder - The number of time units the students answer was off by
    *@param {Float} expectedDuty - The decimal value of the binary number passed to the board for duty cycle
    *@param {Float} receivedDuty - The measured duty cycle
    */
    function gradeData(periodRemainder, expectedDuty, receivedDuty)
    {
        if(periodRemainder < 1) //Checking if error was within one time unit
        {
            return 100;
        }
        var penalty = 3*(periodRemainder - 1)*(periodRemainder - 1); //squaring it
        if(penalty >= 100) //Checking if error exceeds 100%
        {
            return 0;
        }
        else
        {
            return (100-penalty);
        }
    }

    //-------------------------------------------------------------------
    /*This function finds the number of time units that the students answer was off by
    *@param {Float} expectedPer - The decimal value of the binary number passed to the board for period
    *@param {Float} receivedPer - The measured value of the period
    *@param {Float} minTimeUnit - The length of the minimum time unit
    ALL UNITS MUST BE CONSISTENT. WE HAVE DESIGNED IT TO BE IN MILLISECONDS
    */
    function getTimeUnits(expectedPer,receivedPer,minTimeUnit)
    {
        return Math.abs((expectedPer - receivedPer)/minTimeUnit);
    }
//Entire program needs to be in bracket below
})