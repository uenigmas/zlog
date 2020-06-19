# 精简的日志库

跨平台Linux、Windows日志库

基于`git@gitee.com:fifilyu/zpsoe.zlog.git`进行修改
- 输出到文件改为定时刷新，减少io操作
- 将输出方式改为文件和显示两种方式，文件输出默认打开，文件输出方式为 `[程序名][进程id号].log`
- 修改临时目录，Windows默认为 `C:\User\[用户名]\appdata\Local\Temp`
