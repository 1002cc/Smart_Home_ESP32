const WebSocket = require('ws');

// /smartHome/esp32_cam_pub
/*
{
  "code": "200",
  "datas":{
    "startVideo" : 1
  }
}
*/

const wss = new WebSocket.Server({ port: 8888 });
var count;

wss.on('connection', function connection(ws) {
  console.log('A new client Connected!');
  ws.send('Welcome New Client!');
  ++count;

  ws.on('message', function incoming(data) {
    //console.log('received: %s', data);
    //console.log('rec len: %d', data.length);
    wss.clients.forEach(function each(client) {
      if (client !== ws && client.readyState === WebSocket.OPEN) {
        client.send(data);
      }
    });
  });

  ws.on('close', function() {
    count -= 1;
    console.log('%d clients left', count);
    console.log('Client has disconnected');
  });
});

console.log('WebSocket server is running on ws://localhost:8888/');