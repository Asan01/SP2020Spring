#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>


#define TAG "week10"
#define TIMER_TIMEOUT 1

static struct timer_list mytimer; // declaration
struct timespec tv; // structure for getting time

// Function that prints current time by using tv_sec(returns seconds) value of timespec structure
// Convert seconds into hours
void print_current_time(void) {
   getnstimeofday(&tv);
   long long hours = (tv.tv_sec/3600 + 6)%24;
   long long minutes = (tv.tv_sec/60)%60;
   long long seconds = tv.tv_sec%60; 
   pr_info("Current time: %lld:%lld:%lld", hours, minutes, seconds);
}


// Function that runs with ctimer
// It prints counter for each tick as nseconds +
// current time
// Then I change expire time of my timer with mod_timer function
static void timer_handler(struct timer_list *t1) {
   static size_t nseconds;
   nseconds += TIMER_TIMEOUT;
   pr_info("------------------------------------------");
   pr_info("[timer_handler] nseconds = %d\n", nseconds);
   print_current_time();
   mod_timer(t1, jiffies + TIMER_TIMEOUT * HZ);
}

// mod_timer function also activates timer is it is inactive
// this function sets up timer to the first usage
// and inserts this module into the kernel
int init_module(void) {
   pr_info("[timer_init] Init module\n");
   timer_setup(&mytimer, timer_handler, 0);
   mod_timer(&mytimer, jiffies + TIMER_TIMEOUT * HZ);
   return 0;
}


// when write sudo rmmod ctimer it delets the timer.
void cleanup_module(void) {
   printk(KERN_INFO "Cleanup %s\n", TAG);
   del_timer(&mytimer);
}