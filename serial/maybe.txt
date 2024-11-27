// http://ejohn.org/blog/ecmascript-5-strict-mode-json-and-more/
"use strict";
process.title = 'node-serial-ws';

// Websocket
var webSocketsServerPort = 1337;
var webSocketServer = require('websocket').server;
var http = require('http');
var server = http.createServer(function(request, response) {
    // Not important for us. We're writing WebSocket server, not HTTP server
});
var clients = [];

server.listen(webSocketsServerPort, function() {
    console.log((new Date()) + " Server is listening on port " + webSocketsServerPort);
});

var wsServer = new webSocketServer({
    // WebSocket server is tied to a HTTP server. WebSocket request is just
    // an enhanced HTTP request. For more info http://tools.ietf.org/html/rfc6455#page-6
    httpServer: server
});

wsServer.on('request', function(request) {
    console.log((new Date()) + ' Connection from origin ' + request.origin + '.');
    var connection = request.accept(null, request.origin); 
    console.log((new Date()) + ' Connection accepted.');

    var index = clients.push(connection) - 1;

    // user sent some message
    connection.on('message', function(message) {
      onReceive(message);
    });

    // user disconnected
    connection.on('close', function(connection) {
        if (userName !== false && userColor !== false) {
            console.log((new Date()) + " Peer "
                + connection.remoteAddress + " disconnected.");
            // remove user from the list of connected clients
            clients.splice(index, 1);
        }
    });

});

function onReceive(msg)
{
  console.log("ws msg:" + msg);
  serialPort.write(msg);
}

function onSerial(msg)
{
  console.log("uart msg:" + msg);
  for (var i=0; i < clients.length; i++) 
    clients[i].sendUTF(msg);
}

// Serial port
var SerialPort = require("serialport").SerialPort
var portName = 'COM60'; 
var buffer = "";

var serialPort = new SerialPort(portName, {
    baudrate: 9600,
    // defaults for Arduino serial communication
     dataBits: 8,
     parity: 'none',
     stopBits: 1,
     flowControl: false
});

serialPort.on("open", function () {
  console.log('open serial communication');
        // Listens to incoming data
    serialPort.on('data', function(data) {

      buffer += new String(data);
      var lines = buffer.split("\n");
      while ( lines.length > 1 )
        onSerial( lines.shift() );
      buffer = lines.join("\n");

  });  
});  