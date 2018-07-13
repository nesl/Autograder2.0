function myFunction() {
	document.getElementById("demo").innerHTML = "Paragraph changed.";
    var device;
    navigator.usb.requestDevice({ filters: [{vendorId:0x1F00}] })
    .then(selectedDevice => {
    	device = selectedDevice;
        return device.open();
    })
    .then(() => device.selectConfiguration(1))
    .then(() => device.claimInterface(2))
    .then(() => device.controlTransferOut({
    	requestType: 'class' ,
        recipient: 'interface' ,
        request: 0x22,
        value: 0x01,
        index: 0x02}))
/* 	.then(() => {
    	let decoder = new TextDecoder();
        let result = device.transferIn(2, 6);
        console.log('Received: ' + decoder.decode(result.data));
   	})  */
    .catch(error => {console.log(error); });
    navigator.usb.getDevices().then(devices => {
    	devices.map(device => {
        	console.log(device.productName);
            console.log(device.manufacturerName);
            });
        })
}