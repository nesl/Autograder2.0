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
        if(!device.opened) {
            console.log('Could Not Open');
        }
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
    /*This function is used to setup the browser to receive data from the device. Only needs to be 
    called once before you start receiving data. Seperated from receiveData function to save time*/
    async function setControlTransferOut(device){
        await device.controlTransferOut({
            requestType: 'class',
            recipient: 'interface',
            request: 0x22,
            value: 0x01,
            index: 0x02
        })
    }

    //------------------------------------------------------------------
    /*This function is used to receive data from device, function will return the data received.*/
    async function receiveData(device){
        //Waiting for 4 bytes of data from endpoint #5, store that data in result
        let result = await device.transferIn(5,4);
        //Convert raw bytes into int (microseconds)
        var theNum = result.data.getUint32();
        
        //This value is what an unsigned integer returns when it is assigned to be -1. This indicates either a stop message or an error
        if(theNum == 4294967295){ 
            return -1; 
        }
        return (theNum/1000); // divide 1000 to go to ms
    }

    //------------------------------------------------------------------
    /*This function is used to setup the browser to send data to the device. Only needs to be called once before
    you start sending data. Seperated from sendData function to save time*/
    async function setControlTransferIn(device){
        await device.controlTransferIn({
                requestType: 'class',
                recipient: 'interface',
                request: 0x22,
                value: 0x01,
                index: 0x02
            }, 8);
    }

    //-------------------------------------------------------------------
    /*This function is used to send data to the device, u_input is data to be sent*/
    async function sendData(device, u_input){
        ////console.log('Sending Data...');
        //Waiting for 64bytes of data from endpoint #5, store that data in result
        var buffer = new ArrayBuffer(8);
        let encoder = new TextEncoder();
        buffer = encoder.encode(u_input);
        await device.transferOut(5,buffer);
        //console.log('Data Successfully Sent');
    }

    //-------------------------------------------------------------------
    /*This function is used to close the device*/
    async function closeDev(device){
        console.log('Closing...')
        await device.close();
        if(device.opened){
            console.log('Device did not close');
        }
        else{
            console.log('Device closed');
        }
    }

    //-------------------------------------------------------------------
    /*This function is used to simulate wait function. ms represents milliseconds to wait. To work, make
    sure to use await before this function.*/
    function sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    //-------------------------------------------------------------------
    /*This function disables/enables all buttons*/
    function disableButtons(flag){
        $(':button').prop('disabled',flag); //Selects all buttons, sets disabled to flag
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
        disableButtons(true);
        $(select).attr('disabled',false);
        console.log('Device disconencted');// Remove |event.device| from the UI.
    });

    //-------------------------------------------------------------------    
    /*Define all buttons/variables beforehand*/
    let select = document.getElementById('select');
    let send = document.getElementById('send');
    let liveGraph = document.getElementById('liveGraph');
    let blinky1 = document.getElementById('blinky1');
    let blinky2 = document.getElementById('blinky2');
    let blinky3 = document.getElementById('blinky3');
    var SIG_FIGS = 5;
    var TIME_UNIT = 0.2; //ms
    var NUM_CYCLES = 10; //Used for plotly function
    var MIN_PERIOD = 10; //Minimum period required (ms)
    var MAX_PERIOD = 320; //Maximum period (ms)
    var MIN_DUTY_CYCLE = 2; //Minimum Duty Cycle (percentage)
    var MAX_DUTY_CYCLE = 98; //Maximum Duty Cycle (percentage)
    let device;
    //Start with all buttons disabled until device is selected
    disableButtons(true);
    $(select).attr('disabled',false);

    //-------------------------------------------------------------------
    /*Defining what happens when the blinky1 button is clicked. This is meant to
    simulate picking an assignment. It will tell the device what assignment is to
    be graded.*/
    if(blinky1){
        $(blinky1).click(async()=>{
            blinkLight('1');
        })
    }

    //-------------------------------------------------------------------
    /*Defining what happens when the blinky2 button is clicked. This is meant to
    simulate picking an assignment. It will tell the device what assignment is to
    be graded.*/
    if(blinky2){
        $(blinky2).click(async()=>{
            blinkLight('2');
        })
    }

    //-------------------------------------------------------------------
    /*Defining what happens when the blinky3 button is clicked. This is meant to
    simulate picking an assignment. It will tell the device what assignment is to
    be graded.*/
    if(blinky3){
        $(blinky3).click(async()=>{
            blinkLight('3');
        })
    }
    
    //-------------------------------------------------------------------
    /*Defining what happens when the select button is clicked. This is meant
    to select which device will be communicating with the browser*/
    if(select){
        $(select).click(async()=>{
            try{
                disableButtons(true);
                //console.log("Device being selected");
                device = await navigator.usb.requestDevice({filters: [{vendorId:0x1F00}]});
                //console.log('Device selected');
                disableButtons(false);
            } catch(err){
                //console.log(err);
                $(select).attr('disabled', false);
            }
        })
    }

    //-------------------------------------------------------------------
    /*Defines what happens when the send button is clicked. This is used to 
    send period and duty cycle (user input) to the test board and then receive the time 
    stamps measured by the test baord for the period and duty cycle.*/
    if(send){
        $(send).click(async() => {
            //Get values period and duty cycle entered by user
            var period = $('#per').val()*1;
            var dutyCycle = $('#dutyCycle').val()*1;
            var graphLocation = 'user-graph';
            var gTitle = 'Manual Input'; //Graph Title
            var offList = []; //Used to store off times of each fall
            var onList = []; //Used to store on times of each rie
            var expOnTime = period * (dutyCycle/100); //Determine expected on time for each rise
            var expOffTime = period - expOnTime; //Determine expected off time for each fall
            if(validate(period,dutyCycle)){
                try{
                    //Convert from decimal to binary
                    var bPeriod = perDecimaltoBinary(period);
                    var bDutyCycle = dCycleDecimaltoBinary(dutyCycle);
                    disableButtons(true);
                    await connectDev(device);
                    //Send period and duty cycle to device
                    await sendTestCase(bPeriod, bDutyCycle, gTitle, expOnTime, expOffTime, graphLocation);
                    await closeDev(device);
                } catch(err){
                    console.log(err);
                }
                disableButtons(false);
            }
        })
    }

    //-------------------------------------------------------------------
    /*This is used to several test cases to the test board. The test cases are sent
    and then the time stamps are received by the test board. These time stamps are 
    formatted to be displayed on the browser. The measured wave along with the expected 
    wave (what the wave should look like) will be displayed.*/
    if (liveGraph){
        $(liveGraph).click(async () => {
            //let device;
            //Define all periods and duty cycles in binary
            var NUM_CASES = 5;
            var per1 = '01111'; //160ms
            var duty1 = '0001111'//'0110010'; //50%
            var per2 = '01011'; //120ms
            var duty2 = '1000110'; //70%
            var per3 = '10110'; //230ms
            var duty3 = '1011010'; //90%    
            var per4 = '11110'; //310ms
            var duty4 = '0001010'; //10%
            var per5 = '00010'; //30ms
            var duty5 = '0010001'; //17%
            var perList = [per1, per2, per3, per4, per5]; //Store all periods
            var dutyList = [duty1,duty2,duty3,duty4,duty5] //Store all duty cycles
            var expOnList = []; //Stores expected on time for one cycle (ms)
            var expOffList = []; //Stores expected off time for one cycle (ms)
            for(var i=0;i<NUM_CASES;++i){ //Find expected values of period/duty Cycle
                var tempPeriod = (parseInt(perList[i],2) + 1)*10; //Convert binary to period using formula from assignment
                var tempDutyCycle = (parseInt(dutyList[i],2))/100; //Convert binary to percentage
                expOnList[i] = tempPeriod * tempDutyCycle;
                expOffList[i] = tempPeriod - expOnList[i];
            }
            var index = 1;  //Keeps track of what test case we are on
            disableButtons(true);
            try{
                await connectDev(device);
                for(var i=0; i<NUM_CASES; i+=1){
                    var graphElement = 'plotly-test' + index.toString(); //Get which test case this is
                    await sendTestCase(perList[i], dutyList[i], index, expOnList[i], expOffList[i], graphElement);
                    index++;
                }
                /* //Run recordGrades.php to save grades
                $.ajax({
                    type: 'POST', //POST to send data to php file
                    url: 'serverFiles/recordGrades.php', //what file to run
                    data: { fGrade: finalResult, l_name:lastName, f_name:firstName}, //what data to send
                    success: function(response) {     //Run this function if successful
                        //console.log('Saved Results');
                    }
                });*/
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
    /*This function will push a test case to the board and perform various operations on the data
    *@param {String} periodToSend - A binary string representing the period
    *@param {String} dutyToSend - A binary string representing the duty cycle
    *@param {String} index - A string representing which test case it is, represents either a number or phrase
    *@param {Float} expectedOnTime - A number that represents how long each rise should be for this test case
    *@param {Float} expectedOffTime - A number that represents how long each fall should be for this test case
    *@param {String} graphID - A string that determines which div element the graph will go on
    */
    async function sendTestCase(periodToSend, dutyToSend, index, expectedOnTime, expectedOffTime, graphID)
    {
        //Set browser to send and receive data
        await setControlTransferOut(device);
        await setControlTransferIn(device);

        let onList = []; //Stores measured on time for each rise
        let offList = []; //Stores measured off time for each fall

        //Initialize the measured
        Plotly.purge(graphID);
        initGraph(graphID, 'Test Case: ' + index, 'Your Results');

        //Send period followed by duty cycle to test board
        await sendData(device,'0'); //Tell device this is assignment 0
        await sendData(device, periodToSend); 
        await sendData(device, dutyToSend);
        //Loop until the testboard is finished sending data
        await plotOscilloscope(device,offList,onList,graphID);
        //Receive calculated period and duty cycle from device last
        //Check for timeOut error
        var checkStatus = await receiveData(device);
        if(checkStatus < 0){
            $('#' + graphID).after('<div>There was a TIMEOUT ERROR while processing ' +
                'test case ' + index + '.</div>');
        }
        else{
            var finalResult = ''; //What will be saved to text file on server side
            var lastName = 'last'; //Used to store last name of student
            var firstName = 'first'; //Used to store first name of student
            calculateTotalError(onList,offList,expectedOnTime,expectedOffTime, index);
            ////console.log('ONTIMES: for test case ' + index + ': ' + onList);
            ////console.log('OFFTIMES: for test case ' + index + ': ' + offList);
            //This function is used for the reduce function to sum the arrays
            const add = (a, b) =>
                (a + b)
            var aveOnTimes = onList.reduce(add)/onList.length; //Sums array and divide by array length to find average
            var aveOffTimes = offList.reduce(add)/offList.length; 
            var period = (aveOnTimes + aveOffTimes); //Determines average period
            var dCycle = (aveOnTimes / (aveOnTimes + aveOffTimes));
            var expectedPer = expectedOnTime + expectedOffTime;  //Get expected period for this test case
            var expectedDuty = expectedOnTime / expectedPer; //Get expected duty cycle for this test case
            //console.log('Expected Period: ' + expectedPer + '   Expected Duty ' + expectedDuty);
            var periodRemainder = getTimeUnits(expectedPer,period,TIME_UNIT).toFixed(SIG_FIGS);
            var grade = gradeData(periodRemainder, expectedDuty, dCycle);
            //finalResult is what will be saved on server side for teach access
            finalResult += 'Test Case: ' + index + '\n' +
                'Period received: ' + period + 'ms\n' +
                'Duty Cycle received: ' + dCycle*100 + '%\n' +
                'Number of time units off: ' + periodRemainder + '\n' +
                'Grade: ' + grade + '%\n' + '\n' + '\n' ;
            //Append results to browser
            $('#' + graphID).after('<div>Test Case: ' + index + '</div>' +
                '<div>Period received: ' + period + 'ms</div>' +
                '<div>Duty Cycle received: ' + dCycle*100 + '%</div>' +
                '<div>Number of time units off: ' + periodRemainder + '</div>' +
                '<div>Grade: ' + grade + '%</div><br>'); 
        }
    }

    //-------------------------------------------------------------------
    /*This function is used to receive time stamps of PWM waves and plot them on a graph.
    *@param {device} device - The device that is sending data
    *@param {array} offList - An array used to stored all off times of falls
    *@param {array} onList - An array used to stored all on times of rises
    *@param {string} elementID - The string id of DOM element to place graph in
    */
    var currCycle = 0; //Keeps track of the current cycle being plotted 
    var onCounter = 0; //Keeps track of what rise is being plotted
    var offCounter = 0; //Keeps track of what fall is being plotted
    var flag = true; //Used to set flag on whether to keep plotting 
    async function plotOscilloscope(device,offList, onList, elementID){
        var totalTime = 0; //Used to keep track of the current time stamp
        var count = 0; //Count used to adjust x-axis on graph as needed
        var timeOffPromise, timeOnPromise; //Used to hold promises of timeOff/timeOn
        var fOff = 0; //Used to get value from timeOffPromise
        var fOn = 0; //Used to get value from timeOnPromise
        var waitPromise; //Used as a temp variable to wait for timeOn/timeOff Promise
        var record = true; //Flag used to determine whether to keep receiving data 
        //Set flag to true for plotting function
        flag = true; 
        var offSet = 0;

        try{
            //Wait for first timestamp before beginnings async functions
            totalTime = await receiveData(device);
            if(totalTime < 0){
                record = false;
            }
            if(record){
                fOff = (await receiveData(device)*1);
                console.log('aOff: ' + fOff);
            
                //Check for potential timeOut errors
                if(fOff < 0){
                    record = false;
                }
            }
            //Each iteration of while loop records timeStamps of one cycle
            while(record){
                //Begin the plotting function only at the first cycle
                if(currCycle == 0){
                    aPlot(offList,onList,elementID);
                }
                //Start receiving data for timeOn
                timeOn =  (receiveData(device));
                /*This will ignore the first 2 cycles as indicated by the assignment, continue operations
                with fOff while waiting for timeOn*/
                if(currCycle >=0){
                    offList.push(fOff - totalTime);
                    offCounter++;
                    totalTime = fOff;
                }
                //Must wait for timeOn before proceeding, waitPromise will contain an array of promises
                waitPromise = await Promise.all([timeOn]);
                //Get value of timeOn stored in waitPromise array
                fOn = waitPromise[0];
                console.log('On: ' + fOn);
                //Check for potential timeOut errors
                if(fOn < 0){
                    break;
                }
                //Start receiving data for timeOn
                timeOff =  (receiveData(device));
                /*This will ignore the first 2 cycles as indicated by the assignment, continue operations
                with fOff while waiting for timeOn*/
                if(currCycle >=0){
                    onList.push(fOn-totalTime);
                    onCounter++;
                    totalTime = fOn;
                }
                currCycle++; //Increase the currCycle counter
                //Must wait for timeOff before proceeding, waitPromise will contain an array of promises
                waitPromise = await Promise.all([timeOff]);
                //Get value of timeOn stored in waitPromise array
                fOff = waitPromise[0];
                console.log('Off: ' + fOff);
                //Check for potential timeOut errors
                if(fOff < 0){
                    break;
                }
        }}catch(err){
            console.log(err);
        }
        flag = false;
        //Wait for plot function to finish plotting all remaining points
        await Promise.all([aPlot]);
        //Reset all counters for next loop
        onCounter = 0;
        offCounter = 0;
        currCycle = 0;
        console.log(offList);
        console.log(onList);
    }

    //-------------------------------------------------------------------
    /*This function is used to determine what points to plot asynchronously. 
    *@param {array} offList - An array used to stored all off times of falls
    *@param {array} onList - An array used to stored all on times of rises
    *@param {string} elementID - The string id of DOM element to place graph in
    */
    async function aPlot(offList,onList,elementID)
    {
        var totalTime = 0; //Used to keep track of current timeStamp
        var onTime = 0;  //Used to keep track of timeStamp when rise is about to fall
        var offTime = 0; //Used to keep track of timeStamp when fall is about to rise
        var i = 0; //Used to iterate through onList and offList
        var count = 0; //Used to realign x-axis of graph as necessary
        var appendPromise; //Used to store promise made by appendGraph function
        //Used to set up the x-axis and y-axis of graph
        var newLayout = {'xaxis.range': [0,500], 'yaxis.range': [0,1]};
        Plotly.relayout(elementID,newLayout);
        //While there is data to plot
        while(flag){
            await sleep(1); //Wait 1 ms, the plotting doesn't work without this for some reason
            //Get a copy of both counters to prevent potential issues in sharing offCounter and onCounter
            var off_counter_copy = offCounter; 
            var on_counter_copy = onCounter;
            //Iterate through each element of offList and onList
            for(; i < on_counter_copy; i++){
                //Check that we aren't acessing an undefined element of the list
                if(typeof(offList[i]) != 'undefined' && typeof(onList[i]) != 'undefined'){
                    //Determine offTime to plot
                    totalTime += offList[i];
                    offTime = totalTime;
                    //Determine onTime to plot
                    totalTime += onList[i];
                    onTime = totalTime;
                    //Beginning plotting via async
                    appendPromise = appendGraph(offTime,onTime,elementID);
                    //Adjust x-axis as needed
                    if(count > 500){
                        var newLayout = {
                            'xaxis.range': [count - 500, count]
                        }
                        Plotly.relayout(elementID,newLayout);
                    }
                    count = totalTime;
                }
                else{
                    //console.log('error');
                }
            }
        }
        //Wait until all appending is done
        await Promise.all([appendPromise]);
    }

    //-------------------------------------------------------------------
    /*This function initializes a graph with a single point at (0,0)
    *@param {string} elementID - The HTML element where the graph will be created
    *@param {string} gTitle - The title of the graph
    *@parm {string} traceName - Name of the trace
    */
    function initGraph(elementID, gTitle, traceName)
    {
        //Plotly.plot(element, data, layout)
        Plotly.plot(elementID, [{y: [0],x: [0], name: traceName}], {title: gTitle,
            xaxis:{title: 'Time (ms)'}});
    }

    //-------------------------------------------------------------------
    /*This function appends a rise/fall of PWM wave
    *@param {int} off - offTimeStamp to be plotted
    *@param {int} on - onTimeStamp to be plotted
    *@param {string} elementID - The HTML element where the graph to be appended is
    */
    async function appendGraph(off,on,elementID)
    {
        //Plotly.extendTraces(element, updated_data, traces)
        //y:[[y-cooridinates to push to trace ], [y-coordinates to push to trace 1]]
        //[trace 0, trace 1]
        Plotly.extendTraces(elementID, {y:[[0,1,1,0]], x:[[off,off,on,on]]}, [0])
    }

    //-------------------------------------------------------------------
    /*This function is used to validate period and duty cycle
    *@param {int} period - Period to be checked
    *@param {int} dutyCycle - Duty Cycle to be checked
    */
    function validate(period,dutyCycle){
        if(period < MIN_PERIOD || period > MAX_PERIOD || (period % 10 != 0)){
                alert('Error: Period is outside range, please enter a period between ' + MIN_PERIOD +
                    ' and ' + MAX_PERIOD + ' that is divisible by 10.');
                return false;
            }else if(dutyCycle < MIN_DUTY_CYCLE || dutyCycle > MAX_DUTY_CYCLE){
                alert('Error: Duty Cycle is outside range, please enter a duty cycle between ' + MIN_DUTY_CYCLE +
                    ' and ' + MAX_DUTY_CYCLE);
                return false;
            } else{
                return true;
            }
    }

    //-------------------------------------------------------------------
    /*This function is used to convert period from a decimal number to binary string
    *@param {int} period - Period to be converted to binary
    */
    function perDecimaltoBinary(period){
        var bPeriod = parseInt((period/10)-1, 10).toString(2);
        if(bPeriod.length != 5){
            var fillIn = '';
            for(var i = (5 - bPeriod.length); i != 0; i--){
                fillIn += '0';
            }
            bPeriod = fillIn + bPeriod;
        }
        return bPeriod;
    }

    //-------------------------------------------------------------------
    /*This funciton is used to convert duty cycle from decimal number to binary string
    *@param {int} dutyCycle - Duty Cycle to be converted 
    */
    function dCycleDecimaltoBinary(dutyCycle){
        var bDutyCycle = parseInt(dutyCycle, 10).toString(2);
                if(bDutyCycle.length != 7){
                    var fillIn = '';
                    for(var i = (7 - bDutyCycle.length); i != 0; i--){
                        fillIn += '0';
                    }
                    bDutyCycle = fillIn + bDutyCycle;
                }
                return bDutyCycle;
    }

    //-------------------------------------------------------------------
    /*This function grades a set of received data
    *@param {Float} periodRemainder - The number of time units the students answer was off by
    *@param {Float} expectedDuty - The decimal value of the binary number passed to the board for duty cycle
    *@param {Float} receivedDuty - The measured duty cycle
    *FUNCTIONALITY FOR GRADING DUTY CYCLE MUST BE ADDED BE ADDED
    */

    function gradeData(periodRemainder, expectedDuty, receivedDuty)
    {
        if(periodRemainder < 1){ //Checking if error was within one time unit
            return 100;
        }
        var penalty = 3*(periodRemainder - 1)*(periodRemainder - 1); //squaring it
        if(penalty >= 100){ //Checking if error exceeds 100%
            return 0;
        }
        else{
            return (100-penalty);
        }
    }

    //-------------------------------------------------------------------
    /*This function finds the number of time units that the students answer was off by
    *@param {Float} expectedPer - The decimal value of the binary number passed to the board for period
    *@param {Float} receivedPer - The measured value of the period
    *@param {Float} minTimeUnit - The length of the minimum time unit
    *ALL UNITS MUST BE CONSISTENT. WE HAVE DESIGNED IT TO BE IN MILLISECONDS
    */
    function getTimeUnits(expectedPer,receivedPer,minTimeUnit)
    {
        return Math.abs((expectedPer - receivedPer)/minTimeUnit);
    }
    
    //-------------------------------------------------------------------
    /*This function is to test how accurate the timing algorithm is. It is for debugging purposes only
    *@param {Float []} onList - A list of timestamps representing the length of rises for that wave
    *@param {Float []} offList - A list of timestamps representing the length of falls for that wave
    *@param {Float} expectedOn - The theoretical length of the rise
    *@param {Float} expectedOff - The theoretical length of the fall
    *@param {Int} caseNum - The case number for cascaded grading, eg. case 1, case 2, etc.
    */
    function calculateTotalError(onList,offList,expectedOn,expectedOff, caseNum)
    {
        var onSum = 0;
        var offSum = 0;
        //console.log(onList[0]);
        //console.log(offList[0]);
        //console.log(expectedOn);
        //console.log(expectedOff);
        for(var i=0;i<onList.length;++i)
        {
            onSum += Math.abs((onList[i] - expectedOn));
        }
        for(var i=0;i<offList.length;++i)
        {
            offSum += Math.abs((offList[i] - expectedOff));
        }
        //console.log('TOTAL ERROR FOR ON TIMES FOR TEST CASE ' + caseNum + ': ' + onSum);
        //console.log('TOTAL ERROR FOR OFF TIMES FOR TEST CASE ' + caseNum + ': ' + offSum);
        //console.log('AVERAGE ON TIME ERROR ' + onSum/onList.length);
        //console.log('AVERAGE OFF TIME ERROR' + offSum/offList.length);
    }

    //-------------------------------------------------------------------
    /*This function turns a light on or off
    *@param {String} lightNumber - Determines which light will blink on or off
    */
    async function blinkLight(lightNumber)
    {
        try{
                disableButtons(true);
                await connectDev(device);
                await sendData(device, lightNumber); //Indicate this is blinky3 assignment with 3

                await closeDev(device);
            } catch(err){
                //console.log(err);
            }
            disableButtons(false);
    }





})