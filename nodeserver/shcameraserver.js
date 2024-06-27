const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8888 });

wss.on('connection', function connection(ws) {
  console.log('A new client Connected!');
  ws.send('Welcome New Client!');

  ws.on('message', function incoming(data) {
    //console.log('received: %s', data);
    console.log('rec len: %d', data.length);
    wss.clients.forEach(function each(client) {
      if (client !== ws && client.readyState === WebSocket.OPEN) {
        client.send(data);
      }
    });
  });

  ws.on('close', function() {
    console.log('Client has disconnected');
  });
});

console.log('WebSocket server is running on ws://localhost:8888/');