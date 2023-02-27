#include <jni.h>
#include <cstring>
#include <cinttypes>
#include <pthread.h>
#include <android/log.h>
#include <cassert>
#include <ctime>
#include <sched.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include "so_protector.h"
#include "log_ext.h"
#include <string>
#include <dlfcn.h>

#define   LOG_TAG    "cjf_os_protector"

jboolean verify(JNIEnv *env, jobject caller, jobject contextObject) {
    LOG_E(LOG_TAG, "cjf main:verify");
    return false;
}

using namespace so_protector;

void module_init() {
    int pid = getpid();
    LOG_E(LOG_TAG, "cjf main module:init %d", pid);
}

int read_filev2() {
#ifdef __LP64__
    char raw[64000];
#else
    char raw[8000];
#endif
    char *p;
    unsigned long start, end;
    int itemCount = 0, fd, returnValue;
    int pid = getpid();
    sprintf(raw, "/proc/%d/maps", pid);
    fd = open(raw, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    memset(raw, 0, sizeof(raw));
    p = raw;
    while (true) {
        returnValue = read(fd, p, sizeof(raw) - (p - raw));
        if (returnValue < 0) {
            return -1;
        }
        if (returnValue == 0) {
            break;
        }
        p += returnValue;
        if (p > raw + sizeof(raw)) {
            return -1;
        }
    }
    close(fd);
    p = strtok(raw, "\n");
    LOG_E(LOG_TAG, "entry pid:%d size:%d \n %s", pid, strlen(p), p);
    return 0;
}

void read_file(char *file_name, char *buffer) {
    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        LOG_E(LOG_TAG, "Open process map file failed. ");
        return;
    }
    int ret;
    while ((ret = read(fd, buffer, sizeof(buffer)) != 0)) {
        buffer += ret;
    }
    close(fd);
}

MemoryMap load_memorymap(pid_t pid) {
    auto module_init_addr = reinterpret_cast<u_long>(module_init);
    u_long start=0X7f75040000;
    u_long end=0X7f75080000;

    LOG_E(LOG_TAG, "load_memorymap pid:%d module_init_addr:%lx %012lx-%012lx", pid, module_init_addr,start,end);
#ifdef __LP64__
    char file_name[64000];
#else
    char file_name[8000];
#endif
    //[Understanding Linux /proc/id/maps](https://stackoverflow.com/questions/1401359/understanding-linux-proc-id-maps)
    sprintf(file_name, "/proc/%d/maps", pid);//字符串格式化
//    printf("cjf %s", file_name);//打印到终端
//    char buffer[sizeof(file_name)];
//    read_file(file_name, buffer);
//    char *p = strtok(buffer, "\n");
//    LOG_E(LOG_TAG, "entry11 pid:%d size:%d \n %s", pid, strlen(p), p);
//    read_filev2();

    MemoryMap map = MemoryMap();
//    AddressRegion region = AddressRegion();
//    region.start = 1;
//    region.end = 2;
//    std::string key("cjf");
//    map[key] = region;
    return map;
}

bool so_protector::entry() {
    int pid = getpid();
    MemoryMap map = load_memorymap(pid);
    if (!map.empty()) {
        LOG_E(LOG_TAG, "memory map is empty");
        return false;
    }
    return true;
}

void module_fini() {
    LOG_E(LOG_TAG, "cjf main module:fini");
}
