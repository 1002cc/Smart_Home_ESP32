const _http_ = require('http');
const WebSocket = require('ws');
const _fs_ = require('fs');
const { Console } = require('console');

// 创建HTTP服务器
const httpServer = _http_.createServer((_req, _res) => {
    if (_req.url === '/mjpeg/1' && _req.method === 'GET') {
        handleJPGSstream(_req, _res);
    } else if (_req.url === '/jpg' && _req.method === 'GET') {
        handleJPG(_req, _res);
    } else {
        handleNotFound(_req, _res);
    }
});

function camCB() {
    return _fs_.readFileSync('videoStream.jpg');
}

// 处理单帧图片请求
function handleJPG(_req, _res) {
    const img = camCB();
    _res.writeHead(200, {
        'Content-Type': 'image/jpeg',
        'Content-Length': img.length
    });
    _res.end(img);
}

// 处理视频流请求
function handleJPGSstream(_req, _res) {
    _res.writeHead(200, {
        'Content-Type': 'multipart/x-mixed-replace; boundary=123456789000000000000987654321',
        'Access-Control-Allow-Origin': '*'
    });

    function sendFrame() {
        const img = camCB();
        _res.write(`--123456789000000000000987654321\r\n`);
        _res.write(`Content-Type: image/jpeg\r\n`);
        _res.write(`Content-Length: ${img.length}\r\n\r\n`);
        _res.write(img, 'binary');
        _res.write(`\r\n`);
        setTimeout(sendFrame, 100);
    }
    sendFrame();
}

// 处理未找到的请求
function handleNotFound(_req, _res) {
    _res.writeHead(404, { 'Content-Type': 'text/plain' });
    _res.end('Not Found');
}

// 创建WebSocket服务器
const wsServer = new WebSocket.Server({ server: httpServer });
let streamingClients = [];

// 处理WebSocket连接
wsServer.on('connection', (_ws) => {
    console.log('A new client connected to WebSocket server');

    // 将新客户端添加到连接列表中
    streamingClients.push(_ws);

    _ws.on('message', (_data) => {
        // 接收到数据后的处理逻辑（根据需求自行添加）

        // 更新视频流图片
        _fs_.writeFileSync('videoStream.jpg', _data, 'binary');

        // 向所有客户端发送数据
        streamingClients.forEach(_client => {
            if (_client.readyState === WebSocket.OPEN) {
                _client.send(_data);
            }
        });
    });

    _ws.on('close', () => {
      console.log('A client disconnected');
        // 从连接列表中移除断开的客户端
        streamingClients = streamingClients.filter(_client => _client !== _ws);
    });
});

// 监听端口
const PORT = 3000;
httpServer.listen(PORT, () => {
    console.log(`HTTP and WebSocket servers are running on port ${PORT}`);
    console.log(`HTTP videoSteam: http://localhost:${PORT}/mjpeg/1`);
    console.log(`HTTP jpg: http://localhost:${PORT}/jpg`);
    console.log(`WebSocket : ws://localhost:${PORT}`);
});