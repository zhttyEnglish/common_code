// 队列的最大大小
#define QUEUE_SIZE 5

// 队列元素结构体
typedef struct {
    unsigned char *address;  // 指向存储图像数据的内存块
    int in_use;              // 标志，表示该内存块是否已被占用
} QueueElement;

// 队列结构体
typedef struct {
    QueueElement elements[QUEUE_SIZE];  // 队列元素
    int head;                           // 队列头
    int tail;                           // 队列尾
    pthread_mutex_t mutex;              // 用于线程同步
    pthread_cond_t cond;                // 用于条件变量
} ImageQueue;
ImageQueue queue;

// 初始化队列
void queue_init(ImageQueue *queue, int size) {
    memset(queue, 0, sizeof(ImageQueue));
    for (int i = 0; i < QUEUE_SIZE; i++) {
        queue->elements[i].address = (unsigned char *)malloc(size);  // 分配内存
        if (queue->elements[i].address == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        queue->elements[i].in_use = 0;  // 标记为空闲
    }
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
    // cout << "[D] queue_init" << endl;
}

// 从队列中获取空闲的地址
unsigned char *queue_allocate(ImageQueue *queue) {
    pthread_mutex_lock(&queue->mutex);

    // 等待队列中有空闲的内存块
    while (queue->elements[queue->head].in_use == 1) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    // 获取空闲内存块
    unsigned char *addr = queue->elements[queue->head].address;
    queue->elements[queue->head].in_use = 1;  // 标记为已占用
    queue->head = (queue->head + 1) % QUEUE_SIZE;

    pthread_mutex_unlock(&queue->mutex);
    // cout << "[D] queue_allocate" << endl;
    return addr;
}

// 将占用的内存块释放并标记为空闲
void queue_release(ImageQueue *queue, unsigned char *addr) {
    pthread_mutex_lock(&queue->mutex);

    // 查找释放的内存块并标记为空闲
    for (int i = 0; i < QUEUE_SIZE; i++) {
        if (queue->elements[i].address == addr) {
            queue->elements[i].in_use = 0;  // 标记为未占用
            queue->tail = (queue->tail + 1) % QUEUE_SIZE;
            break;
        }
    }

    // 唤醒等待的线程
    pthread_cond_signal(&queue->cond);

    pthread_mutex_unlock(&queue->mutex);
    // cout << "[D] queue_release" << endl;
}

// 销毁队列并释放内存
void queue_destroy(ImageQueue *queue) {
    for (int i = 0; i < QUEUE_SIZE; i++) {
        free(queue->elements[i].address);
    }
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond);
}

#define BUFFER_SIZE 16
void main()
{
    
    queue_init(&queue, BUFFER_SIZE);

    unsigned char * addr = queue_allocate(&queue);

    memset(addr, 0xff, BUFFER_SIZE);
    for(int i = 0; i < BUFFER_SIZE; i ++){
        printf("%X ", queue.elements[queue.tail].addr[i];)
    }
    queue_release(&queue);
    queue_destroy(&queue);
}
