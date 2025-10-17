#include <drv/pwm.h>
#include <stdio.h>
#include <aos/cli.h>
#include <drv/pin.h>
#include <posix/timer.h>
#include <unistd.h>
#include <mmio.h>
#include <stdlib.h>

#define PWM_CLK_HZ  (100*1000*1000UL)  /* pwm clock default 100 MHz */



void test_pwm(int32_t argc, char **argv)
{
	/* test sdcard 命令实现
	 * for example:
	 * testpwm bank(0~3) channel(0~3) o period_us duty_us polarity
     * testpwm bank(0~3) channel(0~3) i
	 */

    // csi_error_t ret;
    csi_pwm_t pwm;
    int value, timeout = 5;
    uint32_t bank, channel, period_us, duty_us, polarity;

	if (argc < 4) {
		aos_cli_printf("please run(example): \n"
                        "\ttestpwm bank(0~3) channel(0~3) o period_us duty_us polarity\n"
                        "\ttestpwm bank(0~3) channel(0~3) i\n");
		return;
	}

    bank = atoi(argv[1]);
    channel = atoi(argv[2]);

    for (value=0; value<argc; value++)
		aos_cli_printf("argv[%d] = %s\r\n", value, argv[value]);

    switch (argv[3][0])
    {
        case 'o':
            // ret = csi_pin_set_mux(JTAG_CPU_TCK, JTAG_CPU_TCK__PWM_6);
            // if (ret != CSI_OK)
            // {
            //     aos_cli_printf("csi_pin_set_mux failed\r\n");
            //     return;
            // }

            period_us = atoi(argv[4]);
            duty_us = atoi(argv[5]);
            polarity = atoi(argv[6]);

            csi_pwm_init(&pwm, bank);
            csi_pwm_out_stop(&pwm, channel);
            csi_pwm_out_config(&pwm, channel, period_us, duty_us,
                                polarity ? PWM_POLARITY_HIGH : PWM_POLARITY_LOW);
            csi_pwm_out_start(&pwm, channel);

            // aos_cli_printf("pwm test success\n");
            break;

        case 'i':
            // ret = csi_pin_set_mux(JTAG_CPU_TMS, JTAG_CPU_TMS__PWM_7);
            // if (ret != CSI_OK)
            // {
            //     aos_cli_printf("csi_pin_set_mux failed\r\n");
            //     return;
            // }

            csi_pwm_init(&pwm, bank);
            csi_pwm_capture_config(&pwm, channel, 0, 0);
            csi_pwm_capture_start(&pwm, channel);

            while(timeout--)
            {
                sleep(1);
                value = *((uint32_t *)(pwm.dev.reg_base + 0x24 + 3 * 8));
                printf("test read reg %p value: %ld\n", ((uint32_t *)(pwm.dev.reg_base + 0x24 + 3 * 8)), PWM_CLK_HZ/value);
            }

            // aos_cli_printf("pwm test success\n");
            break;

        default:
            break;
    }

}
ALIOS_CLI_CMD_REGISTER(test_pwm, testpwm, test pwm function);

/*
 * CLI: testpwm_shift <dev_idx> <shift0_ns> <shift1_ns> <shift2_ns> <shift3_ns> [delay_ms]
 * defaults: freq=100Hz, duty=0.6, polarity=normal, delay=120000ms
 */
void test_pwm_shift_cli(int32_t argc, char **argv)
{
	uint32_t dev_idx;
	uint64_t shift_ns[4];
	uint32_t delay_ms;
	csi_pwm_t csi_hd;
	int ch;

	if (argc < 6) {
		printf("usage: testpwm_shift <dev_idx> <shift0_ns> <shift1_ns> <shift2_ns> <shift3_ns> [delay_ms]\n");
		return;
	}
	dev_idx = (uint32_t)atoi(argv[1]);
	shift_ns[0] = strtoull(argv[2], NULL, 0);
	shift_ns[1] = strtoull(argv[3], NULL, 0);
	shift_ns[2] = strtoull(argv[4], NULL, 0);
	shift_ns[3] = strtoull(argv[5], NULL, 0);
	delay_ms = (argc > 6) ? (uint32_t)atoi(argv[6]) : 120000U;



	if (csi_pwm_init(&csi_hd, dev_idx) != CSI_OK) {
		printf("csi_pwm_init failed\n");
		return;
	}

	/* defaults:  10000ns period, 60% duty -> 6000ns */
	uint32_t period_ns = 10000U;
	uint32_t duty_ns = (uint32_t)(period_ns * 0.6f);

	for (ch = 0; ch < 4; ch++) {
		csi_pwm_out_stop(&csi_hd, (uint32_t)ch);
		if (csi_pwm_out_config(&csi_hd, (uint32_t)ch, period_ns, duty_ns, PWM_POLARITY_LOW) != CSI_OK) {
			printf("csi config ch%d failed\n", ch);
			csi_pwm_uninit(&csi_hd);
			return;
		}
	}


	/* shift: stop -> config per channel -> start */
	csi_pwm_shift_stop(&csi_hd);
	for (ch = 0; ch < 4; ch++) {
		if (csi_pwm_shift_config(&csi_hd, (uint32_t)ch, shift_ns[ch]) != CSI_OK) {
			printf("shift cfg ch%d failed\n", ch);
		}
	}
	(void)csi_pwm_shift_start(&csi_hd);
	usleep((unsigned int)delay_ms * 1000U);
	csi_pwm_shift_stop(&csi_hd);

	for (ch = 0; ch < 4; ch++) {
		csi_pwm_out_stop(&csi_hd, (uint32_t)ch);
	}
	csi_pwm_uninit(&csi_hd);
	printf("testpwm_shift done\n");
}
ALIOS_CLI_CMD_REGISTER(test_pwm_shift_cli, testpwm_shift, pwm shift test);

/*
 * CLI: testpwm_count <dev_idx> <count0> <count1> <count2> <count3>
 * defaults:  10000ns period, 60% duty -> 6000ns
 */
void test_pwm_count_cli(int32_t argc, char **argv)
{
	uint32_t dev_idx;
	uint32_t count_ch[4];
	csi_pwm_t csi_hd;
	int ch;

	if (argc < 6) {
		printf("usage: testpwm_count <dev_idx> <count0> <count1> <count2> <count3>\n");
		return;
	}
	dev_idx = (uint32_t)atoi(argv[1]);
	count_ch[0] = (uint32_t)strtoul(argv[2], NULL, 0);
	count_ch[1] = (uint32_t)strtoul(argv[3], NULL, 0);
	count_ch[2] = (uint32_t)strtoul(argv[4], NULL, 0);
	count_ch[3] = (uint32_t)strtoul(argv[5], NULL, 0);


	if (csi_pwm_init(&csi_hd, dev_idx) != CSI_OK) {
		printf("csi_pwm_init failed\n");
		return;
	}

	/* defaults: 100Hz -> 10000ns period, 60% duty -> 6000ns */
	{
		uint32_t period_ns = 10000U;
		uint32_t duty_ns = (uint32_t)(period_ns * 0.6f);
		for (ch = 0; ch < 4; ch++) {
			csi_pwm_out_stop(&csi_hd, (uint32_t)ch);
			if (csi_pwm_out_config(&csi_hd, (uint32_t)ch, period_ns, duty_ns, PWM_POLARITY_LOW) != CSI_OK) {
				printf("csi config ch%d failed\n", ch);
				csi_pwm_uninit(&csi_hd);
				return;
			}
		}
	}

	for (ch = 0; ch < 4; ch++) {
		if (csi_pwm_count_mode_enable(&csi_hd, (uint32_t)ch) != CSI_OK) {
			printf("enable count ch%d failed\n", ch);
			csi_pwm_uninit(&csi_hd);
			return;
		}
		if (csi_pwm_set_pulse_count(&csi_hd, (uint32_t)ch, count_ch[ch]) != CSI_OK) {
			printf("set count ch%d failed\n", ch);
			csi_pwm_uninit(&csi_hd);
			return;
		}
	}

	for (ch = 0; ch < 4; ch++) {
		if (csi_pwm_out_start(&csi_hd, (uint32_t)ch) != CSI_OK) {
			printf("start ch%d failed\n", ch);
			csi_pwm_uninit(&csi_hd);
			return;
		}
	}
}
ALIOS_CLI_CMD_REGISTER(test_pwm_count_cli, testpwm_count, pwm count test);
