/**
 *
 *
 * service_manager为守护进程，常驻与操作系统。
 * 存放一张全局的服务表，相当于服务中心对外提供CURD。
 * 跟其他进程的通信采用读写binder驱动事件event，来判断执行哪一种操作
 *
 *
 *
 *
 */

#include <sys/ioctl.h>
#include <linux/android/binder.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/mman.h>
#include <fstream>
#include <unistd.h>

struct svcinfo {
    struct svcinfo *next;
    uint32_t handle;//binder对象地址
    struct binder_death *death;
    int allow_isolated;
    size_t len;
    uint16_t name[0];
};
struct svcinfo *svclist = NULL;
struct binder_state {
    int fd;
    void *mapped;
    size_t mapsize;
};
struct binder_state;

struct binder_io {
    char *data;            /* pointer to read/write from */
    binder_size_t *offs;   /* array of offsets */
    size_t data_avail;     /* bytes available in data buffer */
    size_t offs_avail;     /* entries available in offsets array */

    char *data0;           /* start of data buffer */
    binder_size_t *offs0;  /* start of offsets buffer */
    uint32_t flags;
    uint32_t unused;
};
struct binder_io;

typedef int (*binder_handler)(struct binder_state *bs,
                              struct binder_transaction_data *txn,
                              struct binder_io *msg,
                              struct binder_io *reply);

struct binder_state *binder_open(const char *driver, size_t mapsize) {
    struct binder_state *bs;
    struct binder_version vers;
    bs = static_cast<binder_state *>(malloc(sizeof(*bs)));
    bs->fd = open(driver, O_RDWR | O_CLOEXEC);
    if (ioctl(bs->fd, BINDER_VERSION, &vers) == -1 ||
        vers.protocol_version != BINDER_CURRENT_PROTOCOL_VERSION) {
        //goto fail open
        goto fail_open;
    }
    bs->mapsize = mapsize;
    bs->mapped = mmap(NULL, mapsize, PROT_READ, MAP_PRIVATE, bs->fd, 0);
    if (bs->mapped == MAP_FAILED) {
        //goto fail map
        goto fail_map;
    }
    return bs;
    fail_map:
    close(bs->fd);
    fail_open:
    free(bs);
    return NULL;

}

int binder_become_context_manager(struct binder_state *bs) {
    return ioctl(bs->fd, BINDER_SET_CONTEXT_MGR, 0);
}

int binder_parse(struct binder_state *bs, struct binder_io *bio,
                 uintptr_t ptr, size_t size, binder_handler func) {
    //从buffer中按照位数读取数据
    int r = 1;
    uintptr_t end = ptr + (uintptr_t) size;
    while (ptr < end) {
        uint32_t cmd = *(uint32_t *) ptr;
        switch (cmd) {
            case BR_TRANSACTION: {
                struct binder_transaction_data *txn = (struct binder_transaction_data *) ptr;
                if ((end - ptr) < sizeof(*txn)) {
                    return -1;
                }
//                binder_dump_txn(txn);
                if (func) {
                    unsigned rdata[256 / 4];
                    struct binder_io msg;
                    struct binder_io reply;
                    int res;

//                    bio_init(&reply, rdata, sizeof(rdata), 4);
//                    bio_init_from_txn(&msg, txn);
                    res = func(bs, txn, &msg, &reply);
                    if (txn->flags & TF_ONE_WAY) {
//                        binder_free_buffer(bs, txn->data.ptr.buffer);
                    } else {
//                        binder_send_reply(bs, &reply, txn->data.ptr.buffer, res);
                    }
                }
                ptr += sizeof(*txn);
                break;
            }
        }
    }

}

int binder_write(struct binder_state *bs, void *data, size_t len) {
    struct binder_write_read bwr;
    int res;
    bwr.write_size = len;
    bwr.write_consumed = 0;
    bwr.write_buffer = (uintptr_t) data;
    bwr.read_size = 0;
    bwr.read_consumed = 0;
    bwr.read_buffer = 0;
    res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
    if (res < 0) {
        fprintf(stderr, "binder_write: ioctl failed (%s)\n",
                strerror(errno));
    }
    return res;
}

void binder_loop(struct binder_state *bs, binder_handler func) {
    int res;
    struct binder_write_read bwr;
    uint32_t readbuf[32];

    bwr.write_size = 0;
    bwr.write_consumed = 0;
    bwr.write_buffer = 0;

    readbuf[0] = BC_ENTER_LOOPER;
    binder_write(bs, readbuf, sizeof(uint32_t));
    for (;;) {
        bwr.read_size = sizeof(readbuf);
        bwr.read_consumed = 0;
        bwr.read_buffer = (uintptr_t) readbuf;
        res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
        if (res < 0) {
            break;
        }
        res = binder_parse(bs, 0, (uintptr_t) readbuf, bwr.read_consumed, func);
        if (res == 0) {
            break;
        }
        if (res < 0) {
            break;
        }
    }
}

enum {
    /* Must match definitions in IBinder.h and IServiceManager.h */
    PING_TRANSACTION = B_PACK_CHARS('_', 'P', 'N', 'G'),
    SVC_MGR_GET_SERVICE = 1,
    SVC_MGR_CHECK_SERVICE,
    SVC_MGR_ADD_SERVICE,
    SVC_MGR_LIST_SERVICES,
};

int svcmgr_handler(struct binder_state *bs,
                   struct binder_transaction_data *txn,
                   struct binder_io *msg,
                   struct binder_io *reply) {
    struct svcinfo *si;
    uint16_t *s;
    size_t len;
    uint32_t handle;
    uint32_t strict_policy;
    int allow_isolated;

    switch (txn->code) {
        case SVC_MGR_GET_SERVICE:
        case SVC_MGR_CHECK_SERVICE:
//            s = bio_get_string16(msg, &len);
            if (s == NULL) {
                return -1;
            }
//            handle = do_find_service(s, len, txn->sender_euid, txn->sender_pid);
//            if (!handle)
//                break;
//            bio_put_ref(reply, handle);
            return 0;

        case SVC_MGR_ADD_SERVICE:
//            s = bio_get_string16(msg, &len);
            if (s == NULL) {
                return -1;
            }
//            handle = bio_get_ref(msg);
//            allow_isolated = bio_get_uint32(msg) ? 1 : 0;
//            if (do_add_service(bs, s, len, handle, txn->sender_euid,
//                               allow_isolated, txn->sender_pid))
            return -1;
            break;

        case SVC_MGR_LIST_SERVICES: {

        }
        default:
            return -1;
    }

//    bio_put_uint32(reply, 0);
    return 0;
}

int main(int argc, char **argv) {
    char *driver = "/dev/binder";
    //128k
    binder_state *bs = binder_open(driver, 128 * 1024);
    binder_become_context_manager(bs);
    binder_loop(bs, svcmgr_handler);
}
