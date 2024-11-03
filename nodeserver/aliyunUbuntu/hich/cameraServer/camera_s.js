const http = require('http');
const https = require('https');
const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');
const url = require('url');
const ffmpeg = require('fluent-ffmpeg');

// 配置项
const PORT = 3000;
const HTTPS_PORT = 3003;
const HOST = "47.120.7.163";
const BOUNDARY = '123456789000000000000987654321';
const VIDEO_STREAM_PATH = path.join(__dirname, 'videoStream.jpg');

const options = {
    key: fs.readFileSync('/home/hich/ssl/hichchen.top.key', 'utf8'),
    cert: fs.readFileSync('/home/hich/ssl/hichchen.top.pem', 'utf8')
};

// HTTP 服务器
const httpServer = http.createServer((req, res) => {
    const { pathname } = url.parse(req.url);

    if (pathname.startsWith('/images/')) {
        serveImageFromDirectory(req, res);
    } else {
        handleOtherRequests(req, res);
    }
});

// HTTPS 服务器
const httpsServer = https.createServer(options, (req, res) => {
    const { pathname } = url.parse(req.url);

    handleOtherRequests(req, res);
});

// 创建 WebSocket 服务器
const wss = new WebSocket.Server({ server: httpServer });
let clients = [];
let senderClient = null; // 记录发送数据的客户端
let isRecord = true;

// 处理 WebSocket 连接
wss.on('connection', (ws) => {
    console.log('A new client connected to WebSocket server');
    clients.push(ws);

    sendCameraControl();
    ws.on('message', (data) => {
        senderClient = ws;

        if (clients.length >= 2 && isRecord === true) {
            isRecord = false;
            saveImagePeriodically();
        }

        fs.writeFile(VIDEO_STREAM_PATH, data, 'binary', () => {
            broadcast(data, ws);
        });
    });

    ws.on('close', () => {
        console.log('A client disconnected');
        if (clients.length <= 1 || isRecord === false) {
            isRecord = true;
            pauseSaveImage();
        }
        clients = clients.filter(client => client!== ws);
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
        if (client!== sender && client.readyState === WebSocket.OPEN) {
            client.send(data);
        }
    });
}

// 发送当前设备数量给发送数据的客户端
function sendClientCount() {
    if (senderClient && senderClient.readyState === WebSocket.OPEN) {
        const activeClients = clients.filter(client => client!== senderClient && client.readyState === WebSocket.OPEN);
        const clientCount = activeClients.length;
        console.log("Sending client count:", clientCount);
        senderClient.send(JSON.stringify({ clientCount }));
    }
}

// 控制摄像头
function sendCameraControl() {
    if (senderClient && senderClient.readyState === WebSocket.OPEN) {
        const activeClients = clients.filter(client => client!== senderClient && client.readyState === WebSocket.OPEN);
        const enableVideoSteam = activeClients.length > 0;
        console.log("Sending enableVideoSteam:", enableVideoSteam);
        senderClient.send(JSON.stringify({ enableVideoSteam }));
    }
}

// 用于保存图片的间隔定时器引用
let saveImageInterval;

// 每分钟保存一张图片并以日期命名
function saveImagePeriodically() {
    saveImageInterval = setInterval(() => {
        const now = new Date();
        const formattedDate = `${now.getFullYear()}-${now.getMonth() + 1}-${now.getDate()}-${now.getHours()}-${now.getMinutes()}`;
        const savePath = path.join(__dirname, "storagevideo", `${formattedDate}.jpg`);
        fs.readFile(VIDEO_STREAM_PATH, (err, data) => {
            if (err) {
                console.error('Error reading file for saving:', err);
                return;
            }
            fs.writeFile(savePath, data, 'binary', err => {
                if (err) {
                    console.error('Error saving file:', err);
                } else {
                    console.log(`Saved image as ${savePath}`);
                }
            });
        });
    }, 60000);
}

// 暂停保存图片功能的函数
function pauseSaveImage() {
    if (saveImageInterval) {
        clearInterval(saveImageInterval);
        saveImageInterval = null;
        console.log('保存图片功能已暂停。');
    } else {
        console.log('当前没有正在运行的保存图片任务。');
    }
}

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

// 设置 HTTP 响应头
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

// 列出指定目录下的文件
function listFiles(res) {
    const directoryPath = path.join(__dirname, 'storagevideo');
    fs.readdir(directoryPath, (err, files) => {
        if (err) {
            setHeader(res, 500, 'text/plain');
            res.end('Error reading directory');
            return;
        }
        const fileList = files.filter(file => file!== '.' && file!== '..' && (file.endsWith('.jpg') || file.endsWith('.jpeg') || file.endsWith('.png')));
        setHeader(res, 200, 'text/plain');
        res.end(fileList.join('\n'));
    });
}

// 从指定目录服务图片
function serveImageFromDirectory(req, res) {
    const imageDir = path.join(__dirname, 'storagevideo');
    const fileName = path.basename(url.parse(req.url).pathname);
    const imagePath = path.join(imageDir, fileName);
    fs.readFile(imagePath, (err, data) => {
        if (err) {
            setHeader(res, 404, 'text/plain');
            res.end('Image not found');
            return;
        }
        const extension = path.extname(fileName);
        let contentType;
        if (extension === '.jpg' || extension === '.jpeg') {
            contentType = 'image/jpeg';
        } else if (extension === '.png') {
            contentType = 'image/png';
        } else {
            contentType = 'text/plain';
        }
        setHeader(res, 200, contentType);
        res.end(data);
    });
}

// 处理其他请求
function handleOtherRequests(req, res) {
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
        case '/listfiles':
            if (req.method === 'GET') {
                listFiles(res);
            }
            break;
        default:
            handleNotFound(req, res);
    }
}

httpServer.listen(PORT, () => {
    console.log(`HTTP server is running on port ${PORT}`);
    console.log(`HTTP videoStream: http://${HOST}:${PORT}/mjpeg/1`);
    console.log(`HTTP jpg: http://${HOST}:${PORT}/jpg`);
    console.log(`List files: http://${HOST}:${PORT}/listfiles`);
    console.log(`Image access: http://${HOST}:${PORT}/images/*`);
});

httpsServer.listen(HTTPS_PORT, () => {
    console.log(`HTTPS server is running on port ${HTTPS_PORT}`);
    console.log(`HTTPS jpg: https://${HOST}:${HTTPS_PORT}/jpg`);
});