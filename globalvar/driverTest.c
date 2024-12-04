#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

int main()
{
    int fd, num;

    // 打开 "/dev/ch_device" 设备文件，O_RDWR 表示读写打开，S_IRUSR | S_IWUSR 设置权限为用户可读可写
    fd = open("/dev/globalvar", O_RDWR, S_IRUSR | S_IWUSR);

    // 如果打开成功
    if (fd != -1) 
    {
        // 初次从设备读取数据
        read(fd, &num, sizeof(int));
        printf("The ch_device is %d\n", num);

        // 提示用户输入一个数字并写入设备
        printf("Please input the num written to ch_device\n");
        scanf("%d", &num);

        // 写入设备
        write(fd, &num, sizeof(int));

        // 再次读取设备数据并显示
        read(fd, &num, sizeof(int));
        printf("The ch_device is %d\n", num);

        // 关闭设备文件
        close(fd);
    }
    else
    {
        // 打开设备失败，输出错误信息
        printf("Device open failure\n");
    }

    return 0;
}

