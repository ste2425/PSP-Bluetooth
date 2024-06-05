const readline = require('node:readline');
const ws = require('ws');

console.log('GO BTTO');

const { SerialPort } = require('serialport');

// Create a port
const port = new SerialPort({
  path: '/dev/pts/6',
  baudRate: 57600,
});

let _ws;

port.on('close', () => console.log('CLOSE')); 
port.on('data', function (data) {
  console.log('data', data.toString());
 //ws.send(data.toString());
 if (_ws)
  _ws.send(data);
});

const fs = require('fs')
const options = {
  key: fs.readFileSync('../server.key'), // replace it with your key path
  cert: fs.readFileSync('../server.crt'), // replace it with your certificate path
};

const http = require('https')

const server = http.createServer(options, (req, res) => {
  res.writeHead(200, { 'content-type': 'text/html' })
  fs.createReadStream('index.html').pipe(res)
});
console.log('Running on 3000');
server.listen(3000);

const wss = new ws.Server({ server, path: '/ws' });

wss.on('connection', function connection(ws) {
  _ws = ws;
    ws.on('error', console.error);

    ws.on('message', function message(data) {
        console.log('received: ', data);
        port.write(data, function(err) {
            if (err) {
              return console.log('Error on write: ', err.message)
            }
          });
    });

});