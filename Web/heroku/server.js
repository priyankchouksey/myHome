const SocketServer = require('ws').Server;
var express = require('express');
const bodyParser = require('body-parser');
var connectedUsers = [];
var count = 0;

//init Express
var app = express();

//init Express Router
var router = express.Router();
var port = process.env.PORT || 3000;

// function rawBody(req, res, next) {
//     req.setEncoding('utf8');
//     req.rawBody = '';
//     req.on('data', function(chunk) {
//       req.rawBody += chunk;
//     });
//     req.on('end', function(){
//       next();
//     });
//   }

//REST route for GET /status
router.get('/status', function(req, res) {
    res.json({ status: 'App is running!' });
    wss.clients.forEach((client) => {
        client.send("status req made");
        count++;
    })
});

//connect path to router
app.use("/", router);
app.use(bodyParser.text({type:"*/*"}));

app.post('/', function(req, res) {
    var count = 0;
    var x = '' + req.body
    //var speech = req.body.result && req.body.result.parameters && req.body.result.parameters.echoText ? req.body.result.parameters.echoText : "Seems like some problem. Speak again."
    wss.clients.forEach((client) => {
        client.send(x);
        count++;
    });
    var x = res.json({
        speech: "command processed",
        displayText: "command processed to " + count + " clints",
        source: 'webhook-myHome'
    });
    return x;
})

//add middleware for static content
app.use(express.static('static'))
var server = app.listen(port, function () {
    console.log('node.js static, REST server and websockets listening on port: ' + port)
})

const wss = new SocketServer({ server });

//init Websocket ws and handle incoming connect requests
wss.on('connection', function connection(ws) {
    console.log("connection ...");

    //on connect message
    ws.on('message', function incoming(message) {
        console.log('received: %s', message);
        if ()
        connectedUsers.push(message);
    });

});