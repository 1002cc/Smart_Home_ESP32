## **esp32 智能家居设备**
### 1 介绍
esp32 是一款集成了 Wi-Fi 和蓝牙的微控制器，它可以用来制作智能家居设备，如智能门锁、智能灯、智能窗帘、智能电视、智能音响等。

esp32 具有以下特性：

- 低功耗：esp32 采用了低功耗的芯片，可以运行在 3.3V 电压的环境中，所以它可以长时间处于待机状态。
- 集成 Wi-Fi 和蓝牙：esp32 集成了 Wi-Fi 和蓝牙模块，可以实现 Wi-Fi 和蓝牙通信。
- 丰富的外设：esp32 具有丰富的外设，如摄像头、麦克风、LED 灯、电子元件、传感器等。
- 易于使用：esp32 采用了开源的 RTOS 操作系统，使得它易于使用。


### 2 开发环境准备

#### 2.1 硬件准备

1.esp32 开发板：

- esp32-wroom-32：一款集成了 Wi-Fi 和蓝牙的微控制器，它具有 4MB 的闪存和 520KB 的 RAM。

2.开发板连接线

3.触摸屏(320x240 ili9341)

4.面包板

5.杜邦线

#### 2.2 软件准备

开发工具：
    vscode、esp-idf 开发框架、串口工具
- 安装 esp-idf 开发框架：下载 esp-idf 压缩包，解压后进入 esp-idf 目录，运行 install.bat 文件，根据提示完成安装。
- 配置环境变量：将 esp-idf 目录添加到环境变量中，这样就可以在命令行中使用 idf.py 命令。
- 下载示例程序：下载 esp-idf 目录下的示例程序，并解压到本地。

### 3 开发流程

#### 3.1 新建工程   

打开命令行，进入到 esp-idf 目录，输入命令 `idf.py create-project hello-world` 创建一个新的工程。

```
C:\Users\Administrator>cd E:\Smart_Home\ESP32\esp-idf

C:\Users\Administrator\E\Smart_Home\ESP32\esp-idf>idf.py create-project hello-world
Project name: hello-world
Creating new project directory 'hello-world'
Creating new program
Copying files from E:\Smart_Home\ESP32\esp-idf\examples\get-started\hello_world to hello-world
The following files have been created in hello-world:

    hello-world\CMakeLists.txt
    hello-world\main\CMakeLists.txt
    hello-world\main\hello_world_main.c
    hello-world\README.md                        

Project has been created. You can now compile the project by running "idf.py -B build" in the project directory.
```

#### 3.2 编译工程

在命令行中进入到 hello-world 目录，输入命令 `idf.py -B build` 编译工程。

####  3.3 烧写固件

在命令行中进入到 hello-world 目录，输入命令 `idf.py -p PORT -b BAUD flash` 烧写固件。

- PORT：串口号，如 COM3。
- BAUD：波特率，如 921600。

#### 3.4 运行程序   

1. 打开串口工具，设置串口号和波特率。
2. 连接开发板。
3. 打开烧写好的固件。
4. 按下复位按键。       
5. 等待程序运行。

### 4 移植库
    1.lvgl
    2.TFT_eSPI
    3.arduino-esp32

### 5.设置flash大小