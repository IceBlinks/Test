// Based on an example:
//https://github.com/don/cordova-plugin-ble-central

	document.addEventListener("deviceready", tryAutoConnect, false);
	
	document.addEventListener('DOMContentLoaded', onDomLoad);
// ASCII only
function bytesToString(buffer) {
    return String.fromCharCode.apply(null, new Uint8Array(buffer));
}

function onDomLoad() {
	let ledBtn = document.getElementById("ledOnButton");
	ledBtn.addEventListener('click', onClickLedButton);
}

// ASCII only
function stringToBytes(string) {
    var array = new Uint8Array(string.length);
    for (var i = 0, l = string.length; i < l; i++) {
        array[i] = string.charCodeAt(i);
    }
    return array.buffer;
}

function onClickLedButton() {
	var url='https://maker.ifttt.com/trigger/Smart/with/key/lYhLnA5dYk-jXJZ-sRyik1458CZfJAedYJhG-yGG4vS';
	
	data('1');
	openBrowser(url);
}

function tryAutoConnect() {
	deviceid = window.localStorage.getItem('deviceid');
	
	if (deviceid === null) {
		refreshDeviceList();
	} else {
	ble.autoConnect(deviceid, onAutoSucess, onAutoFail);
	}
}

function onAutoSucess() {
}

function onAutoFail() {
}
// this is ble hm-10 UART service
/*var blue= {
    serviceUUID: "0000FFE0-0000-1000-8000-00805F9B34FB",
    characteristicUUID: "0000FFE1-0000-1000-8000-00805F9B34FB"
};*/

//the bluefruit UART Service
var blue ={
	serviceUUID: '6e400001-b5a3-f393-e0a9-e50e24dcca9e',
    txCharacteristic: '6e400002-b5a3-f393-e0a9-e50e24dcca9e', // transmit is from the phone's perspective
    rxCharacteristic: '6e400003-b5a3-f393-e0a9-e50e24dcca9e'  // receive is from the phone's perspective
}

var deviceid;
var deviceList =[];
 
function onLoad(){
    bleDeviceList.addEventListener('click', conn, false); // assume not scrolling
}
/*
function onDeviceReady(){
	refreshDeviceList();

}
*/
	 
function refreshDeviceList(){
	//deviceList =[];
	document.getElementById("bleDeviceList").innerHTML = ''; // empties the list
	if (cordova.platformId === 'android') { // Android filtering is broken
		ble.scan([], 5, onDiscoverDevice, onError);
	} else {
		//alert("Disconnected");
		ble.scan([blue.serviceUUID], 5, onDiscoverDevice, onError);
	}
}


function onDiscoverDevice(device){
	//Make a list in html and show devises
		var listItem = document.createElement('li'),
		html = device.name+ "," + device.id;
		listItem.innerHTML = html;
		document.getElementById("bleDeviceList").appendChild(listItem);
}


function conn(){
	var  deviceTouch= event.srcElement.innerHTML;
	document.getElementById("debugDiv").innerHTML =""; // empty debugDiv
	var deviceTouchArr = deviceTouch.split(",");
	deviceid = deviceTouchArr[1];
	window.localStorage.setItem('deviceid', deviceid);
	//document.getElementById("debugDiv").innerHTML += "<br>"+deviceTouchArr[0]+"<br>"+deviceTouchArr[1]; //for debug:
	ble.connect(deviceid, onConnect, onConnError);
 }
 
 //succes
function onConnect(){
	document.getElementById("statusDiv").innerHTML = " Status: Connected";
	document.getElementById("bleId").innerHTML = deviceid;
	ble.startNotification(deviceid, blue.serviceUUID, blue.rxCharacteristic, onData, onError);
}

//failure
function onConnError(){
	alert("Problem connecting");
	document.getElementById("statusDiv").innerHTML = " Status: Disonnected";
}

 function onData(data){ // data received from Arduino
	document.getElementById("receiveDiv").innerHTML =  "Received: " + bytesToString(data) + "<br/>";
}

function data(txt){
	GemtInput.value = txt;
	sendData();
}	

function openBrowser(url) {
   var target = '_blank';
   var options = "hidden=yes"
   var ref = cordova.InAppBrowser.open(url, target, options);
}

function sendData() { // send data to Arduino
	var data = stringToBytes(GemtInput.value)
	ble.writeWithoutResponse(deviceid, blue.serviceUUID, blue.txCharacteristic, data, onSend, onError);
}
	
function onSend(){
	document.getElementById("sendDiv").innerHTML = "Sent: " + GemtInput.value + "<br/>";
}


function disconnect() {
	ble.disconnect(deviceid, onDisconnect, onError);
}

function onDisconnect(){
	document.getElementById("statusDiv").innerHTML = "Status: Disconnected";
}
function onError(reason)  {
	alert("ERROR: " + reason); // real apps should use notification.alert
}

	
