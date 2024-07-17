# 配置阿里云服务器  
## 一.配置安全组
开放3000端口
## 二.配置UBUNTU服务器(20.04)
### 1.切换源服务器
1.查询版本  
```
lsb_release -a  
```
2.备份源文件  
```
sudo cp /etc/apt/sources.list /etc/apt/sources.list.back  
``` 
3.修改源文件(https://mirrors.tuna.tsinghua.edu.cn/help/ubuntu/)  
``` 
vim /etc/apt/sources.list
```
``` shell
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-updates main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-updates main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-backports main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-backports main restricted universe multiverse

# 以下安全更新软件源包含了官方源与镜像站配置，如有需要可自行修改注释切换
deb http://security.ubuntu.com/ubuntu/ focal-security main restricted universe multiverse
# deb-src http://security.ubuntu.com/ubuntu/ focal-security main restricted universe multiverse

# 预发布软件源，不建议启用
# deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-proposed main restricted universe multiverse
# # deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-proposed main restricted universe multiverse
```
4.更新源  
```
sudo apt update  
sudo apt upgrade
```
### 2.配置用户(hich)
```
adduser hich
```

修改用户名权限
```
sudo chmod +w /etc/sudoers
sudo vim /etc/sudoers
``` 

```
hich ALL=(ALL:ALL) ALL
```
```
sudo chmod -w /etc/sudoers
```
### 3.修改文件权限(hich)
```
sudo chmod 777 filename
```

## 三.配置MYSQL
### 1.安装mysql
``` shell
sudo apt update
sudo apt install mysql-server
mysql --version

# 配置sqml
sudo mysql

>>use mysql；
>>select user, plugin from mysql.user;  #root用户plugin为auth_socket，之后会出现错误
>>update mysql.user set plugin='mysql_native_password' where user='root';  #修改plugin
>>update user set host = '%' where user = 'root';  #给root用户授权使之可以在任何网络中访问
>>alter user 'root'@'%' IDENTIFIED WITH mysql_native_password BY '修改的密码';  #修改密码
>>FLUSH PRIVILEGES;  #更新配置
>> exit #退出

$service mysql restart  #重启mysql服务

mysql -u root -p

```

### 2.mysql使用
``` shell
# 启动mysql服务
sudo systemctl start mysql
# 停止mysql服务
sudo systemctl stop mysql
# 设置开机自动启动mysql服务
sudo systemctl enable mysql
# 检查mysql服务状态
sudo systemctl status mysql
# 重启mysql服务
sudo service mysql restart
```
