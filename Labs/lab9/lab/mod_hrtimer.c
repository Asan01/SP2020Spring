#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>

MODULE_LICENSE("GPL");

#define INTERVAL_BETWEEN_CALLBACKS (100 * 10000000LL) 

int arr[10] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34}; // array of fibonacci numbers sequence.
int counter = 1; 
struct timespec tv; // used to get current time.


static struct hrtimer timer;
static ktime_t kt;

static s64 starttime_ns;

void print_current_time(void) {
   getnstimeofday(&tv);
   long long hours = (tv.tv_sec/3600 + 6)%24;
   long long minutes = (tv.tv_sec/60)%60;
   long long seconds = tv.tv_sec%60; 
   pr_info("Current time: %lld:%lld:%lld", hours, minutes, seconds);
}


static enum hrtimer_restart my_hrtimer_callback( struct hrtimer *timer )
{
    pr_info("---------------------------------------------------------");
	printk("I'm in hrtimer handler. Counter now is %d\r\n", counter);
    print_current_time();
    printk("Current fibonacci number: %d", arr[counter]);
    s64 now_ns = ktime_to_ns(ktime_get());
    // Change time to next fibonacci sequence number, 
    kt = ktime_set(0, arr[counter]*INTERVAL_BETWEEN_CALLBACKS); //next call relative to expired timestamp
    // Set the timeout time, when the times timeout, you can use this function to set the next timeout time.
    hrtimer_forward(timer, timer->_softexpires, kt);
     // Contraint: so that counter doesn't go beyond 9
    // If we don't use it then there will be segmentation fault, since arr has no more than 10 elements.
    // You can continue the sequence 
    // by adding other numbers and increasing constraint number from 9 to your number.
    if(counter <= 9) {
        counter++;
    }
    return HRTIMER_RESTART;
}
 

static int init_module_hrtimer( void )
{
	printk("mod_hrtimer: installing module...\n");
    // define a ktime variable with the interval time defined on top of this file
	kt = ktime_set(0, INTERVAL_BETWEEN_CALLBACKS);
    // init a high resolution timer named 'timer'
    hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    //activate the high resolution timer
    hrtimer_start(&timer, kt, HRTIMER_MODE_REL);
    // set the callback function for timer
    timer.function = my_hrtimer_callback;
    return 0;
}
 

// when write sudo rmmod ctimer
// this function activates
// it prints message The timer was still in use
// ret value will be none zero if the timer was active 
// and deletes timer 
static void cleanup_module_hrtimer( void )
{
	int ret;
	ret = hrtimer_cancel( &timer );
	if (ret)
	printk("mod_hrtimer: The timer was still in use...\n");
	printk("mod_hrtimer: HR Timer module uninstalling\n");
}
 
module_init(init_module_hrtimer);
module_exit(cleanup_module_hrtimer);