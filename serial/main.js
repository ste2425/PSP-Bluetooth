const ws = require('ws'),
  handler = require('serve-handler'),
  { SerialPort } = require('serialport'),
  fs = require('fs'),
  https = require('https'),
  { spawn } = require('child_process'),
  path = require('path');

let port,
  _ws;

function startSocat(ready) {
  console.log('Starting SOCAT...');

  const matches = [];
  //
  const ls = spawn('socat', ['-d', '-d', 'pty,raw,echo=0', 'pty,raw,echo=0']);

  const handler = (data) => {
    const dataStr = data.toString();

    if (dataStr.includes('/dev/pts'))
      matches.push(...dataStr.match(new RegExp("/dev/pts/(\\d+)", 'g')));

    if (dataStr.includes('starting data transfer loop'))
      ready(matches);
  }

  ls.stdout.on('data', handler);

  ls.stderr.on('data', handler);

  ls.on('close', (code) => {
    console.log(`SOCAT closed: ${code}`)
  });
}

function connectSerial(path) {
  console.log('Connecting to serial', path);

  port = new SerialPort({
    path,
    baudRate: 115200,
  });

  port.on('close', () => {
    console.log('Serial Port closed');
    port = undefined;
  });

  port.on('data', function (data) {
    console.log('got port data', data.toString());
    if (_ws)
      _ws.send(data);
  });
}

const routes = {
  '/esptool.js': (req, res) => {
    res.writeHead(200, { 'content-type': 'text/javascript' });
    fs.createReadStream(path.join(__dirname, 'node_modules/esptool-js/bundle.js')).pipe(res);
  }
};

const options = {
  key: fs.readFileSync('../server.key'), // replace it with your key path
  cert: fs.readFileSync('../server.crt'), // replace it with your certificate path
};

const server = https.createServer(options, (req, res) => {
  if (req.url in routes)
    return routes[req.url](req, res);
  else
    return handler(req, res, {
      public: 'static'
    });
});

server.listen(3000);

server.on('listening', () => {
  console.log('Server running on port 3000');
return;
  startSocat((paths) => {
    console.log('Socat ready with paths:', paths);

    connectSerial(paths[0]);
  });
});

const wss = new ws.Server({ server, path: '/ws' });

wss.on('connection', function connection(ws) {
  _ws = ws;

  ws.on('error', (e) => {
    console.error(e);
  });

  ws.on('message', function message(data) {
    port.write(data, function (err) {
      if (err) {
        return console.log('Error writing to Serial Port', err.message)
      }
    });
  });
});

wss.on('close', () => {
  _ws = undefined;
});