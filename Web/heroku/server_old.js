'use strict';

const express = require('express');
const PORT = process.env.PORT || 3000;
const bodyParser = require('body-parser');
const SocketServer = require('ws').Server;

const restService = express();

restService.use(bodyParser.urlencoded({
    extended: true
}));

restService.use(bodyParser.json());

restService.post('/', function(req, res) {
    var count = 0;
    var speech = req.body.result && req.body.result.parameters && req.body.result.parameters.echoText ? req.body.result.parameters.echoText : "Seems like some problem. Speak again."
    wss.clients.forEach((client) => {
        client.send(req.body.result.parameters.echoText);
        count++;
    });
    var x = res.json({
        speech: "command processed",
        displayText: "command processed to " + count + " clints",
        source: 'webhook-myHome'
    });
    return x;
});

const listner = restService.listen((PORT), function() {
    console.log("Server up and listening");
});

//console.log(listner);
const wss = new SocketServer({ server: listner });
wss.on('message',(ws) =>{
    console.log(ws);
})
wss.on('connection', (ws) => {
  console.log('Client connected');
  ws.on('close', () => console.log('Client disconnected'));
});