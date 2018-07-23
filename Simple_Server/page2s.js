//Wait until page is fully loaded
$(document).ready(function(){


if (typeof(Storage) !== "undefined") {
    let blah = document.getElementById('blah');
    if (blah){
    	$(blah).click(async()=>{
    		localStorage.setItem('period','LOGO');
    	})
    }
    let foo = document.getElementById('foo');
    if(foo){
    	$(foo).click(async()=>{
    		console.log(localStorage.getItem('period'));
    	})
    }
} else {
	console.log('No');
    // Sorry! No Web Storage support..
}
})