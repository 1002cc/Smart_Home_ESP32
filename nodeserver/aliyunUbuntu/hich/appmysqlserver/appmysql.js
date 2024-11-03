const express = require('express');
const bodyParser = require('body-parser');
const mysql = require('mysql');
const http = require('http');
const https = require('https');
const fs = require('fs');

// 创建 Express 应用
const app = express();
app.use(bodyParser.json());

const options = {
  key: fs.readFileSync('/home/hich/ssl/hichchen.top.key', 'utf8'),
  cert: fs.readFileSync('/home/hich/ssl/hichchen.top.pem','utf8')
};

app.get('/', (req, res) => res.send('Hello World!smarthome!'));

app.post('/userlogin', (req, res) => {
  const { username, password } = req.body;
  console.log(req.body);
  if (req.body.username === undefined || req.body.password === undefined) {
    res.status(400).send('请求体中缺少用户名或密码');
    return;
  }
  // 创建 MySQL 连接
  const connection = mysql.createConnection({
    host: 'localhost',
    user: 'root',
    password: '1002chEN*',
    database:'smarthome_flow'
  });

  // 连接到 MySQL 数据库
  connection.connect((err) => {
    if (err) {
      console.error('数据库连接失败:', err);
      return;
    }
    console.log('成功连接到数据库');
  });

  const query = `SELECT * FROM users WHERE username = '${username}' AND password = '${password}';`;
  console.log(query);
  connection.query(query, (err, results) => {
    if (err) {
      console.error('查询数据库时出错:', err);
      res.status(500).send('服务器内部错误');
      return;
    }
    console.log(results);

    if (results.length > 0) {
      const responseData = {
        loginSuccess: true
      };
      console.log(responseData);
      res.send(responseData);
    } else {
      const responseData = {
        loginSuccess: false
      };
      console.log(responseData);
      res.send(responseData);
    }
    console.log("关闭数据库");
    connection.end();
  });
});


app.post('/login', (req, res) => {
  const { username, password } = req.body;
  console.log(req.body);
  if (req.body.username === undefined || req.body.password === undefined) {
    res.status(400).send('请求体中缺少用户名或密码');
    return;
  }
  // 创建 MySQL 连接
  const connection = mysql.createConnection({
    host: 'localhost',
    user: 'root',
    password: '1002chEN*',
    database:'smarthome_flow'
  });

  // 连接到 MySQL 数据库
  connection.connect((err) => {
    if (err) {
      console.error('数据库连接失败:', err);
      return;
    }
    console.log('成功连接到数据库');
  });

  const query = `SELECT * FROM users WHERE username = '${username}' AND password = '${password}';`;
  console.log(query);
  connection.query(query, (err, results) => {
    if (err) {
      console.error('查询数据库时出错:', err);
      res.status(500).send('服务器内部错误');
      return;
    }
    console.log(results);
    var tablename;
    tablename = username + "devicetable";
    console.log(tablename);
    if (results.length > 0) {
      const deviceListQuery = `select type,handle,text,unit,value from ${tablename};`;  // 将 your_table_name 替换为实际的表名
      connection.query(deviceListQuery, (err, allData) => {
        if (err) {
          console.error('查询另一张表时出错:', err);
          res.status(500).send('服务器内部错误');
          console.log("关闭数据库");
          connection.end();
          return;
        }

        const deviceList = allData;
        const responseData = {
          loginSuccess: true,
          deviceList: deviceList
        };
        console.log(responseData);
        res.send(responseData);
        console.log("关闭数据库");
        connection.end();
      });
    } else {
      res.status(401).json({ error: '用户名或密码错误' });
      console.log("关闭数据库");
      connection.end();
    }
  });
});

// 创建 HTTP 服务器
const httpServer = http.createServer(app);

// 创建 HTTPS 服务器
const httpsServer = https.createServer(options, app);

// 监听 HTTP 服务器的端口
httpServer.listen(3002, () => {
  console.log('HTTP 服务器运行在 3002 端口');
});

// 监听 HTTPS 服务器的端口
httpsServer.listen(3001, () => {
  console.log('HTTPS 服务器运行在 3001 端口');
});
