#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/spinlock.h>

#define TAG "Lab8"
#define MAX_NUM 100
//#define MAX_NUM 100000 


static struct task_struct *t1;
static struct task_struct *t2;
static struct task_struct *t3;
static struct task_struct *t4;

DEFINE_SPINLOCK(lock);

int arrSorted[MAX_NUM];
int arrUnsorted[MAX_NUM];

struct sortParams {
    int start;
    int end;
};

int gen_int(void) {
    int n;
    get_random_bytes(&n, sizeof(n));
    n = n < 0 ? n * -1 : n;
    n = n % MAX_NUM;
    return n;
}

void printSorted(void) {
    int i = 0;
    printk(KERN_INFO "First 10 elements of Sorted Array\n");
    for(i = 0; i < 10; i++) {
        printk("Element number %d is: %d", i, arrSorted[i]);
    }
    printk(KERN_INFO "Last 10 elements of Sorteddd Array\n");
    for(i = MAX_NUM - 11; i < MAX_NUM; i++) {
        printk("Element number %d is: %d", i, arrSorted[i]);
    }
}

void printUnsorted(void) {
    int i = 0;
    printk(KERN_INFO "First 10 elements of Unsorted Array\n");
    for(i = 0; i < 10; i++) {
        printk("Element number %d is: %d", i, arrUnsorted[i]);
    }
    printk(KERN_INFO "Last 10 elements of Unsorted Array\n");
    for(i = MAX_NUM - 11; i < MAX_NUM; i++) {
        printk("Element number %d is: %d", i, arrUnsorted[i]);
    }
}

void fillArrayWithRandomVariables(int numberOfElements) {
    int i = 0;
    for(i = 0; i < numberOfElements; i++) {
        arrUnsorted[i] = gen_int();
    }
}

static int threadSort(void *args) {
    spin_lock(&lock);
    struct sortParams *params = (struct sortParams*) args;

    int start = params->start;
    int end = params->end;
    int i = params->start;
    int temp_element;
    int j;
    while(i <= end) {
        temp_element = arrUnsorted[i];
        j = i-1;
        while(j >= start && temp_element < arrUnsorted[j]){
            arrUnsorted[j+1] = arrUnsorted[j];
            j--;
        }
        arrUnsorted[j+1] = temp_element;
        i++;
    }
    spin_unlock(&lock);
    return 0;
}

static int threadMerge(void *unused) {
    spin_lock(&lock);
    int i = 0;
    int j = MAX_NUM/2 + 1;
    int n = MAX_NUM - 1;
    int k = 0;
    
    while(i < MAX_NUM/2 + 1 && j <= n) {
        if(arrUnsorted[i] < arrUnsorted[j]) {
            arrSorted[k++] = arrUnsorted[i++];
        }
        else {
            arrSorted[k++] = arrUnsorted[j++];
        }
    }

    while(i < (MAX_NUM/2 + 1)) {
        arrSorted[k++] = arrUnsorted[i++];
    }
    while(j <= n) {
        arrSorted[k++] = arrUnsorted[j++];
    }
    printSorted();
    spin_unlock(&lock);
    return 0;
}

int init_module(void)
{
    printk(KERN_INFO "START %s\n", TAG);

    fillArrayWithRandomVariables(MAX_NUM);

    struct sortParams Args[2];
    Args[0].start = 0;
    Args[0].end = MAX_NUM/2;
    Args[1].start = MAX_NUM/2 + 1;
    Args[1].end = MAX_NUM - 1; 

    t1 = kthread_run(threadSort, &Args[0], "sortBegin");
    t2 = kthread_run(threadSort, &Args[1], "sortEnd");
    t3 = kthread_run(threadMerge, NULL, "merge1");   
    t4 = kthread_run(threadMerge, NULL, "merge2"); 
    
    printUnsorted();

    kthread_stop(t1);
    kthread_stop(t2);
    kthread_stop(t3);
    kthread_stop(t4);
    
    return 0;
}

void cleanup_module(void)
{
    printk(KERN_DEBUG "END %s\n", TAG);
}