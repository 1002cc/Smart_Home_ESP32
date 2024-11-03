const http = require('http');
const https = require('https');
const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');
const url = require('url');

// 配置项
const PORT = 3000;
const HOST = "47.120.7.163";
const BOUNDARY = '123456789000000000000987654321';
const VIDEO_STREAM_PATH = path.join(__dirname, 'videoStream.jpg');

const options = {
    key: fs.readFileSync('/home/hich/ssl/hichchen.top.key', 'utf8'),
    cert: fs.readFileSync('/home/hich/ssl/hichchen.top.pem','utf8')
};

const httpsServer = https.createServer(options, (req, res) => {
    const { pathname } = url.parse(req.url);

    switch (pathname) {
        case '/mjpeg':
            if (req.method === 'GET') {
                handleMJPEGStream(req, res);
            }
            break;
        case '/jpg':
            if (req.method === 'GET') {
                handleJPEG(req, res);
            }
            break;
        default:
            handleNotFound(req, res);
    }  
});
  

// 创建HTTP服务器
const server = http.createServer((req, res) => {
    const { pathname } = url.parse(req.url);

    switch (pathname) {
        case '/mjpeg/1':
            if (req.method === 'GET') {
                handleMJPEGStream(req, res);
            }
            break;
        case '/jpg':
            if (req.method === 'GET') {
                handleJPEG(req, res);
            }
            break;
        default:
            handleNotFound(req, res);
    }
});

// 处理单帧图片请求
function handleJPEG(req, res) {
    readAndSendFile(res, VIDEO_STREAM_PATH);
}

// 处理视频流请求
function handleMJPEGStream(req, res) {
    setHeader(res, 200, 'multipart/x-mixed-replace; boundary=' + BOUNDARY);
    setInterval(() => {
        readAndSendFile(res, VIDEO_STREAM_PATH, BOUNDARY);
    }, 100);
}


// 处理未找到的请求
function handleNotFound(req, res) {
    setHeader(res, 404, 'text/plain');
    res.end('Not Found');
}

// 设置HTTP响应头
function setHeader(res, statusCode, contentType) {
    res.writeHead(statusCode, { 'Content-Type': contentType, 'Access-Control-Allow-Origin': '*' });
}

// 读取文件并发送
function readAndSendFile(res, filePath, boundary) {
    fs.readFile(filePath, (err, data) => {
        if (err) {
            console.error('Error reading file:', err);
            res.end();
            return;
        }
        if (boundary) {
            res.write(`--${boundary}\r\n`);
            res.write(`Content-Type: image/jpeg\r\n`);
            res.write(`Content-Length: ${data.length}\r\n\r\n`);
        }
        res.write(data, 'binary');
        if (boundary) {
            res.write(`\r\n`);
        }
    });
}

// 创建WebSocket服务器
const wss = new WebSocket.Server({ server });
let clients = [];
let senderClient = null; // 记录发送数据的客户端

// 处理WebSocket连接
wss.on('connection', (ws) => {
    console.log('A new client connected to WebSocket server');
    clients.push(ws);

    sendCameraControl();

    ws.on('message', (data, isBinary) => {
            if(isBinary) {
                senderClient = ws; 
                fs.writeFile(VIDEO_STREAM_PATH, data, 'binary', () => {
                    broadcast(data, ws);
             
                });
            } else {
                
            }
          
    });

    ws.on('close', () => {
        console.log('A client disconnected');
		
        clients = clients.filter(client => client !== ws);
        console.log("Clients count:", clients.length);
        if (ws === senderClient) {
            senderClient = null;
        }
     
        sendCameraControl();
    });
});

// 广播数据给所有客户端，排除发送数据的客户端
function broadcast(data, sender) {
    clients.forEach(client => {
        if (client !== sender && client.readyState === WebSocket.OPEN) {
            client.send(data);
        }
    });
}

// 发送当前设备数量给发送数据的客户端
function sendClientCount() {
    if (senderClient && senderClient.readyState === WebSocket.OPEN) {
        const activeClients = clients.filter(client => client !== senderClient && client.readyState === WebSocket.OPEN);
        const clientCount = activeClients.length;
        console.log("Sending client count:", clientCount);
        senderClient.send(JSON.stringify({ clientCount }));
    }
}

// 控制摄像头
function sendCameraControl() {
    if (senderClient && senderClient.readyState === WebSocket.OPEN) {
        const activeClients = clients.filter(client => client !== senderClient && client.readyState === WebSocket.OPEN);
        const enableVideoSteam = activeClients.length > 0;
        console.log("Sending enableVideoSteam:", enableVideoSteam);
        senderClient.send(JSON.stringify({ enableVideoSteam }));
    }
}


server.listen(PORT, () => {
    console.log(`HTTP and WebSocket servers are running on port ${PORT}`);
    console.log(`HTTP videoSteam: http://${HOST}:${PORT}/mjpeg/1`);
    console.log(`HTTP jpg: http://${HOST}:${PORT}/jpg`);
    console.log(`WebSocket : ws://${HOST}:${PORT}`);
});

httpsServer.listen(3003, () => {
    console.log(`HTTPS server is running on port 3003`);
    console.log(`HTTPS jpg: https://${HOST}:3003/jpg`);
});