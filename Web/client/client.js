const WebSocket = require('ws');
const http = require('http');

let response= { device: {name: 'entry table led'},
                responseType: 'config',
                responseText: '' };

//var connection = new WebSocket('wss://otv-myhome.herokuapp.com');
var connection = new WebSocket('wss://otv-myhome.herokuapp.com');

connection.onopen = function () {
  respondWS();
};

// Log errors
connection.onerror = function (error) {
  console.error('WebSocket Error ' + error);
};

// Log messages from the server
connection.onmessage = function (e) {
    console.log(e.data);
    var data = JSON.parse(e.data);
    processRequest(data);
};

function processRequest(data){
    response.responseText = '';
    var options = getoption(data.result.parameters);
    if (options == ''){
        responseType = "response";
        response.responseText = 'I cannot understand that...';
        respondWS();
    }
    else{
        http.get(options, (resp) => {
              let data = '';
            
              // A chunk of data has been recieved.
              resp.on('data', (chunk) => {
                data += chunk;
              });
            
              // The whole response has been received. Print out the result.
              resp.on('end', () => {
                responseType = "response";
                response.responseText = 'Table LED turned ' + data.result.parameters.state;
                respondWS();                
                console.log(data);
              });
            
            }).on("error", (err) => {
              console.log("Error: " + err.message);
        });
    }
}
function respondWS(){
    connection.send(response);
};
function getoption(params){
    var options = '';
    if (params.location == "entry" && params.deviceholder === "table" && params.device === "led"){
        options = 'http://10.0.0.173/';
    }
    if (options != ''){
        options += getStateCode(params.state);
    }
    return options;
}
function getStateCode(state){
    var statusCode = '/';
    switch (state) {
        case 'rainbow':
            statusCode = 'RBO';
            break;
        case 'on':
            statusCode = 'RBO';            
            break;
        case 'off':
            statusCode = 'WPO';            
            break;
        case 'green':
            statusCode = 'WPG';            
            break;
        case 'red':
            statusCode = 'WPR';            
            break;
        case 'blue':
            statusCode = 'WPB';            
            break;
        case 'glitter':
            statusCode = 'RBG';            
            break;
        case 'confetti':
            statusCode = 'CNF';            
            break;
        case 'sinelon':
            statusCode = 'SLN';            
            break;
        case 'juggle':
            statusCode = 'JGL';            
            break;
        case 'BPM':
            statusCode = 'BPM';            
            break;
        default:
            statusCode = 'WPO';
            break;
    }
    return statusCode;
}