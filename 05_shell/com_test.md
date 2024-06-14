## Linux 模拟串口 （U 型环）
 在没有物理串口的设备上调试串口程序，Windows 操作系统中可以使用 Virtual COM Port Driver 创建虚拟串口对，那 Linux 中是不是
 也可以用创建虚拟串口的方式。今天就遇到了这样的问题，查阅相关资料后记录解决方案。

```sh
# 安装 socat
sudo apt install socat

#（终端0）建立虚拟串口对。 在 /dev/pts/2 和 /dev/pts/3 之间建立关联。
socat -d -d pty,raw,echo=0 pty,raw,echo=0
# nohup socat -d -d pty,raw,echo=0 pty,raw,echo=0 &>com.log & #后台运行
2023/10/10 11:23:59 socat[603] N PTY is /dev/pts/7
2023/10/10 11:23:59 socat[603] N PTY is /dev/pts/8
2023/10/10 11:23:59 socat[603] N starting data transfer loop with FDs [5,5] and [7,7]

#（终端1）监听其中一个串口
cat /dev/pts/7
#（终端2）另一个串口写入数据
echo "Hello xxWorldsdfaf" >/dev/pts/8

```

## 串口指令测试

```sh
# 设置 COM1 串口波特率9600 数据位8位 校验无 停止位1位
stty -F /dev/COM1 9600 cs8 -cstopb -parity igncr
# 打开读窗口, xxd=16进制输出
cat /dev/COM1|xxd

# 另外新开一个窗口输入命令
echo  -ne "$(sed 's/../\\x&/g' <<<021006ffff14)" >/dev/COM1
```
