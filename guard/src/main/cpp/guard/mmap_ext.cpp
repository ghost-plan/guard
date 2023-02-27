#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>
#include <cstdlib>

//const char *fileName = "/Users/jf.chen/CLionProjects/untitled/cjf.txt";
const char *fileName = "cjf.txt";

void test_mmap2() {
    printf("test_mmap2==============start\n");
    int fd = open(fileName, O_RDWR | O_CREAT, 0666);
    printf("打开文件fd:%d \n", fd);
    write(fd, "start header\n", strlen("start header\n"));
//    if (fd == -1) {
//        fd = open(fileName, O_CREAT, 0777);
//        if (fd == -1) {
//            printf("创建文件失败");
//            exit(1);
//        }
//    }
    char *address = (char *) mmap(NULL, 2049, PROT_READ | PROT_WRITE|PROT_EXEC, MAP_SHARED, fd, 0);
    if (address == NULL) {
        printf("映射失败 address为nullptr\n");
    } else if (address == MAP_FAILED) {
        printf("映射失败 address为MAP_FAILED\n");
    }
    printf("开始读写文件\n");
    //写数据
    char ret[10];
//    printf("address:%s\n", address);
//    sprintf(ret, "%s write to fil\n", address);
//    printf("ret:%s\n", ret);
//    strcpy(address, ret);
    for (int i = 0; i < 100; ++i) {
        sprintf(ret,">>%d:write to \n", i);
        strcpy(address, ret);
    }

    //读取数据
//    char buffer[2049];
//    strcpy(buffer, address);
//    printf("%s \n", buffer);
//    munmap(address, 2049);
//    close(fd);
    printf("test_mmap2==============end\n");
}