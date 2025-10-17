#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <csi_core.h>
#include "aos/cli.h"
#include "aos/kernel.h"
#include "debug/dbg.h"
#include "drv/common.h"
#include "drv/timer.h"

static csi_timer_t timer1 = {0};
static uint32_t period = 0;

void oneshot_callback(void *arg)
{
    aos_debug_printf("one shot\n");
}

void periodic_callback(void *arg)
{
    aos_debug_printf("periodic\n");
    csi_timer_start(&timer1, period);
}

void cvi_timer_test(int32_t argc, char **argv)
{
    if (argc < 3) {
        printf("one-shot usage: timer 0 period\n");
        printf("periodic usage: timer 1 period\n");
        return;
    }

    int mode = atoi(argv[1]);
    period = atoi(argv[2]);
    if (mode < 0 || mode >= 2) {
        printf("Invalid mode, only 0 / 1 is supported.\n");
        return;
    }
    if (mode == 0) {
        printf("One-shot timer test\n");
        csi_timer_init(&timer1, 0);
        csi_timer_attach_callback(&timer1, oneshot_callback, NULL);
        csi_timer_start(&timer1, period);
    } else {
        printf("Periodic timer test\n");
        csi_timer_init(&timer1, 0);
        csi_timer_attach_callback(&timer1, periodic_callback, NULL);
        csi_timer_start(&timer1, period);
    }

}
ALIOS_CLI_CMD_REGISTER(cvi_timer_test, timer, timer test);