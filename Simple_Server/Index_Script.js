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
        //Waiting for 4 bytes of data from endpoint #5, store that data in result
        let result = await device.transferIn(5,4);
        return (result.data.getFloat32()); //Convert raw bytes into float
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
        //console.log('Sending Data...');
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
        if(device.opened)
            console.log('Device did not close');
        else
            console.log('Device closed');
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
    let receive = document.getElementById('request-device');
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
    var expTrace = 1;
    var measuredTrace = 0;
    var runExpTrace = 1;
    var runMeasuredTrace = 0;
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
                disableButtons(false);
            } catch(err){
                console.log(err);
                $(select).attr('disabled', false);
            }
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
            var period = $('#per').val()*1;
            var dutyCycle = $('#dutyCycle').val()*1;
            var elementID = document.getElementById('user-graph'); //DOM element where the graph will be inserted
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
                    await sendData(device,'0');
                    await sendData(device,bPeriod); 
                    await sendData(device,bDutyCycle);
                    //Initialize the measured and expected graph
                    initGraph(elementID,gTitle, 'Your Results');
                    initGraph(elementID,gTitle, 'Expected Results');
                    //Plot PWM wave in graph
                    await plotOscilloscope(device,offList,onList,expOffTime,expOnTime,elementID,measuredTrace, expTrace);

                    measuredTrace += 2;
                    expTrace += 2;
                    //Receive period and duty cycle
                    var perResult = await receiveData(device);
                    var dutyResult = await receiveData(device);
                    var elementID = 'user-graph';
                    //Check for time out error
                    if(perResult*1 < 0){
                            $('#' + elementID).after('<div>There was a TIMEOUT ERROR while processing ' +
                                'test case ' + index + '. Please reset the tester board.</div>');
                    }
                    //Otherwise display results
                    else{
                        var per = (parseFloat(perResult) * 1000).toFixed(SIG_FIGS);; //Convert from seconds to ms
                        var dCycle = (parseFloat(dutyResult) * 100).toFixed(SIG_FIGS);
                        var periodRemainder = getTimeUnits(period,perResult,TIME_UNIT).toFixed(SIG_FIGS);
                        $('#' + elementID).after('<div>Manual Test Case'  + '</div>' +
                                '<div>Period received: ' + per + 'ms</div>' +
                                '<div>Duty Cycle received: ' + dCycle + '%</div>' +
                                '<div>Number of time units off: ' + periodRemainder + '</div>' +
                                '<br>'); 
                    }

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
    formatted to be displayed on the browser. The time stamps are displaeyd dynamically.
    The measured wave along with the expected wave (what the wave should look like) will
    be displayed.*/
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
            var totalTime = 0; //The total time elapsed
            var exptotalTime = 0; //The total time expected to have elapsed

            var index = 1;  //Keeps track of what test case we are on
            var finalResult = ''; //What will be saved to text file on server side
            var lastName = 'last'; //Used to store last name of student
            var firstName = 'first'; //Used to store first name of student
            disableButtons(true);
            try{
                await connectDev(device);
                for(var i=0; i<NUM_CASES; i+=1){
                    var elementID = 'plotly-test' + index.toString(); //Get which test case this is
                    var onList = []; //Stores measured on time for each rise
                    var offList = []; //Stores measured off time for each fall

                    //Initialize the measured and expected graph
                    initGraph(elementID, 'Test Case: ' + index, 'Your Results');
                    initGraph(elementID, 'Test Case: ' + index, 'Expected Results');


                    //Send period followed by duty cycle to test board
                    await sendData(device,'0'); //Tell device this is assignment 0
                    await sendData(device, perList[i]); 
                    await sendData(device, dutyList[i]);

                    //Loop until the testboard is finished sending data
                    await plotOscilloscope(device,offList,onList,expOffList[i],expOnList[i],elementID, runMeasuredTrace, runExpTrace);

                    //Reset total times for next test case
                    calculateTotalError(onList,offList,expOnList[i],expOffList[i], index);
                    console.log('ONTIMES: for test case ' + index + ': ' + onList);
                    console.log('OFFTIMES: for test case ' + index + ': ' + offList);

                    //Receive calculated period and duty cycle from device last
                    var period = await receiveData(device);
                    console.log('Period: ' + period);
                    var dCycle = await receiveData(device);
                    console.log('Duty Cycle: ' + dCycle);

                    //Check for timeOut error
                    if(period*1 < 0){
                        $('#' + elementID).after('<div>There was a TIMEOUT ERROR while processing ' +
                            'test case ' + index + '.</div>');
                    }
                    else{
                        //finalResult is what will be saved on server side for teach access
                        period *= 1000;
                        dCycle *= 100;
                        var expectedPer = (parseInt(perList[i], 2) + 1)*10;  //Get expected period for this test case
                        var expectedDuty = parseInt(dutyList[i],2); //Get expected duty cycle for this test case
                        var periodRemainder = getTimeUnits(expectedPer,period,TIME_UNIT).toFixed(SIG_FIGS);
                        var grade = gradeData(periodRemainder, expectedDuty, dCycle);
                        finalResult += 'Test Case: ' + index + '\n' +
                            'Period received: ' + period + 'ms\n' +
                            'Duty Cycle received: ' + dCycle + '%\n' +
                            'Number of time units off: ' + periodRemainder + '\n' +
                            'Grade: ' + grade + '%\n' + '\n' + '\n' ;
                        //Append results to browser
                        $('#' + elementID).after('<div>Test Case: ' + index + '</div>' +
                            '<div>Period received: ' + period + 'ms</div>' +
                            '<div>Duty Cycle received: ' + dCycle + '%</div>' +
                            '<div>Number of time units off: ' + periodRemainder + '</div>' +
                            '<div>Grade: ' + grade + '%</div><br>'); 
                    }
                    index++;
                }
                /* //Run recordGrades.php to save grades
                $.ajax({
                    type: 'POST', //POST to send data to php file
                    url: 'serverFiles/recordGrades.php', //what file to run
                    data: { fGrade: finalResult, l_name:lastName, f_name:firstName}, //what data to send
                    success: function(response) {     //Run this function if successful
                        console.log('Saved Results');
                    }
                });*/

                runExpTrace += 2;
                runMeasuredTrace +=2;
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
    /*This function is used to receive time stamps of PWM waves and plot them on a graph.
    *@param {device} device - The device that is sending data
    *@param {array} offList - An array used to stored all off times of falls
    *@param {array} onList - An array used to stored all on times of rises
    *@param {float} expOffTime - Expected off time of all falls
    *@param {float} expOnTime - Expected on time of all rises
    */
    async function plotOscilloscope(device,offList, onList, expOffTime, expOnTime, elementID, eTrace, mTrace){
        var totalTime = 0;
        var exptotalTime = 0;
        var count = 0;
        while(true){
            //Receive timeOff first
            var timeOff = await receiveData(device);
            if(timeOff*1 < 0){ //check that the device is still sending data
                break; //Break from the loop if there is no more data
            }
            var ftimeOff = timeOff*1; //Convert timeOff to float
            offList.push(ftimeOff*1000); //Store measured offTime to offList
            totalTime+= (ftimeOff*1000); //Updated totalTime (s to ms)
            exptotalTime += (expOffTime); //Update expected time (ms)
            appendGraph(totalTime, exptotalTime, 0,1,elementID,mTrace,eTrace); //Append both graphs with new data

            //Receive timeOn next
            var timeOn = await receiveData(device);
            if(timeOn*1 < 0){ //check that the device is still sending data
                break; //Break from the loop if there is no more data
            }
            var ftimeOn = timeOn*1; //Convert timeOn to float
            onList.push(ftimeOn*1000); //Store measured onTime to onList
            totalTime+= (ftimeOn*1000); //Updated totalTime (s to ms)
            exptotalTime += (expOnTime); //Update expected time (ms)
            appendGraph(totalTime, exptotalTime, 1,0,elementID, mTrace, eTrace); //Append both graphs with new data

            //Check that the x-axis does not exceed 500 units, if so adjust the graph
            if(count > 500){
                var newLayout = {
                    'xaxis.range': [count - 500, count]
                }
                Plotly.relayout(elementID,newLayout);
            }
            count = totalTime;
        }
    }

    //-------------------------------------------------------------------
    /*This functio is used to validate period and duty cycle*/
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
    /*This function is used to convert period from a decimal number to binary string*/
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
    /*This funciton is used to convert duty cycle from decimal number to binary string*/
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
    /*This function initializes a graph with a single point at (0,0)
    *@param {DOMelement} elementID - The HTML element where the graph will be created
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
    *@param {int} x1 - The measured time of the current edge
    *@param {int} x2 - The expected time of the current edge
    *@param {int} y1 - 0 for fall, 1 for rise
    *@param {int} y2 - 1 for fall, 0 for rise
    *@param {DOMelement} elementID - The HTML element where the graph to be appended is
    *@param {int} expTrace - Trace where expected PWM is stored is stored
    *@param {int} measuredTrace - Trace where measured PWM wave is stored
    */
    function appendGraph(x1,x2,y1,y2,elementID, measuredTrace, expTrace)
    {
        //Plotly.extendTraces(element, updated_data, traces)
        //y:[[y-cooridinates to push to trace ], [y-coordinates to push to trace 1]]
        //[trace 0, trace 1]
        Plotly.extendTraces(elementID, {y:[[y1,y2],[y1,y2]], x:[[x1,x1],[x2,x2]]}, [measuredTrace,expTrace])
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
        console.log(onList[0]);
        console.log(offList[0]);
        console.log(expectedOn);
        console.log(expectedOff);
        for(var i=0;i<onList.length;++i)
        {
            onSum += Math.abs((onList[i] - expectedOn));
        }
        for(var i=0;i<offList.length;++i)
        {
            offSum += Math.abs((offList[i] - expectedOff));
        }
        console.log('TOTAL ERROR FOR ON TIMES FOR TEST CASE ' + caseNum + ': ' + onSum);
        console.log('TOTAL ERROR FOR OFF TIMES FOR TEST CASE ' + caseNum + ': ' + offSum);
    }






})