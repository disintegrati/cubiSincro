const express = require('express');
const ws = require('ws');
const http = require('http');
const path = require('path');


const app = express();

//initialize a simple http server
const server = http.createServer(app);

const wss = new ws.Server({ server, path: "/ws" });

wss.broadcast = function broadcast(data) {
  wss.clients.forEach(function each(client) {
    if (client.readyState === ws.OPEN) {
      client.send(data);
    }
  });
};

const PORT = process.env.PORT || 3000;
const INDEX = path.join(__dirname, 'public', 'index.html');

app.get('/', (req, res) => {
  res.sendFile(INDEX);
  wss.broadcast('request');
});

wss.on('connection', (ws) => {
  console.log('Client connected!');

  ws.on('close', () => console.log('Client disconnected'));
});

server.listen(PORT, () => console.log(`Listening on ${PORT}`));
