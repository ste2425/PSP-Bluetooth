const handler = require('serve-handler'),
  fs = require('fs'),
  https = require('https'),
  path = require('path'),
  ws = require('ws'),
  debounce = require('debounce');

const binaryPath = path.join(__dirname, '../software/bin/pspBluetooth.bin'),
  sockets = [];
  
const certOptions = {
  key: fs.readFileSync('../server.key'), // replace it with your key path
  cert: fs.readFileSync('../server.crt'), // replace it with your certificate path
};

const routes = {
  '/crypto.js': (req, res) => {
    res.writeHead(200, { 'content-type': 'text/javascript' });
    fs.createReadStream(path.join(__dirname, 'node_modules/crypto-js/crypto-js.js')).pipe(res);
  },
  '/esptool.js': (req, res) => {
    res.writeHead(200, { 'content-type': 'text/javascript' });
    fs.createReadStream(path.join(__dirname, 'node_modules/esptool-js/bundle.js')).pipe(res);
  },
  '/download': (req, res) => {
    res.setHeader("content-type", "application/octet-stream");
    fs.createReadStream(binaryPath).pipe(res);
  }
};

fs.watchFile(binaryPath, debounce((curr, prev) => {
  if(fs.existsSync(binaryPath)) 
    sockets.forEach(s => s.send('binary-change'));
  
}, 200));

const server = https.createServer(certOptions, (req, res) => {
  if (req.url in routes)
    return routes[req.url](req, res);
  else
    return handler(req, res, {
      public: 'static'
    });
});

const wss = new ws.WebSocketServer({ server, path: '/ws' });

wss.on('connection', function connection(ws) {
  sockets.push(ws);

  ws.on('error', console.error);

  ws.on('close', () => {
    const index = sockets.indexOf(ws);

    if (index != -1)
      sockets.slice(index, 1);
  });
});

server.listen(3000);

server.on('listening', () => {
  console.log('Server running on port 3000');
});