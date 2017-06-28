#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>

#define timerclear(tvp) ((tvp)->tv_sec = (tvp)->tv_usec = 0)

void (*g_timer_cb)(void);
struct itimerval tval;

void timer_handler(int signo)
{
    assert(signo == SIGALRM);

    // turn off timer
    setitimer(ITIMER_REAL, NULL, NULL);

    // proceed to user callback
    g_timer_cb();
}

void timer_init(void (*callback)(void))
{
    printf("timer_init (callback = %p)\n", callback);

    // set callbacks
    signal(SIGALRM, timer_handler);
    g_timer_cb = callback;
}

void timer_set(uint32_t time_us)
{
    printf("timer_set (time_us = %i)\n", time_us);

    // zero interval means no reset of timer
    timerclear(&tval.it_interval);
    timerclear(&tval.it_value);

    // set timer
    tval.it_value.tv_sec = 0;
    tval.it_value.tv_usec = time_us;
    setitimer(ITIMER_REAL, &tval, NULL);
}

void delay_us(uint32_t time_us)
{
    for (volatile uint32_t i = 0; i < time_us; i++);
}
