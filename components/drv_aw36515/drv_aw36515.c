#include <drv/iic.h>
#include "cvi_type.h"
#include "sensor_i2c.h"
#include <ulog/ulog.h>
#include <aos/kernel.h>
#include <aos/debug.h>
#include <aos/cli.h>
#include "drv_aw36515.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define AW36515_IIC_ID  1
#define AW36515_ADDR    0x63
static void strobe_nir_print_help(void);
static void strobe_str_print_help(void);

static inline int i2c_bus_send(int bus, const unsigned char *data, int size)
{
    int ret = 0;

    ret = sensor_i2c_write(bus, AW36515_ADDR, data[1], 1, data[2], 1);
    if (ret < 0) {
        printf("%s sensor_i2c_write %02x:%02x error\n", __func__, data[1], data[2]);
    }

    return 0;
}

int drv_aw36515_init()
{
    sensor_i2c_init(AW36515_IIC_ID);
    return 0;
}

int drv_aw36515_strobe_nir_level(int level)
{
    float value = level - 3.91;
    value = (value / 7.83);
    unsigned char light_level = value;

    unsigned char cmd[][3] = {
        {AW36515_ADDR, 0x02, 0x01},
        {AW36515_ADDR, 0x03, light_level},
        {AW36515_ADDR, 0x04, light_level},
        {AW36515_ADDR, 0x05, light_level},
        {AW36515_ADDR, 0x06, light_level},
        {AW36515_ADDR, 0x07, 0x09},
        {AW36515_ADDR, 0x08, 0x00},
        {AW36515_ADDR, 0x01, 0xA5},        
    };

    printf("light_level = %02x\n", light_level);
    for (int i = 0; i < ARRAY_SIZE(cmd); i++) {
        if (i2c_bus_send(AW36515_IIC_ID, cmd[i], 3)) {
                fprintf(stderr, "set led %s failed.\n", __func__);
                return -1;
        }
    }

    return 0;
}

int drv_aw36515_strobe_str_level(int level)
{
    float value = level - 3.91;
    value = (value / 7.83);
    unsigned char light_level = value;

    unsigned char cmd[][3] = {
        {AW36515_ADDR, 0x02, 0x01},
        {AW36515_ADDR, 0x03, light_level},
        {AW36515_ADDR, 0x04, light_level},
        {AW36515_ADDR, 0x05, light_level},
        {AW36515_ADDR, 0x06, light_level},
        {AW36515_ADDR, 0x07, 0x09},
        {AW36515_ADDR, 0x08, 0x00},
        {AW36515_ADDR, 0x01, 0xA6},        
    };    

    printf("light_level = %02x\n", light_level);
    for (int i = 0; i < ARRAY_SIZE(cmd); i++) {
        if (i2c_bus_send(AW36515_IIC_ID, cmd[i], 3)) {
                fprintf(stderr, "set led %s failed.\n", __func__);
                return -1;
        }
    }

    return 0;
}

int drv_aw36515_torch_nir_level(int level)
{
    float value = level - 0.98;
    value = (value / 1.96);
    unsigned char light_level = value;

    unsigned char cmd[][3] = {
        {AW36515_ADDR, 0x02, 0x01},
        {AW36515_ADDR, 0x03, light_level},
        {AW36515_ADDR, 0x04, light_level},
        {AW36515_ADDR, 0x05, light_level},
        {AW36515_ADDR, 0x06, light_level},
        {AW36515_ADDR, 0x07, 0x09},
        {AW36515_ADDR, 0x08, 0x00},
        {AW36515_ADDR, 0x01, 0x99},        
    };

    printf("light_level = %02x\n", light_level);
    for (int i = 0; i < ARRAY_SIZE(cmd); i++) {
        if (i2c_bus_send(AW36515_IIC_ID, cmd[i], 3)) {
                fprintf(stderr, "set led %s failed.\n", __func__);
                return -1;
        }
    }

    return 0;
}

int drv_aw36515_torch_str_level(int level)
{
    float value = level - 0.98;
    value = (value / 1.96);
    unsigned char light_level = value;

    unsigned char cmd[][3] = {
        {AW36515_ADDR, 0x02, 0x01},
        {AW36515_ADDR, 0x03, light_level},
        {AW36515_ADDR, 0x04, light_level},
        {AW36515_ADDR, 0x05, light_level},
        {AW36515_ADDR, 0x06, light_level},
        {AW36515_ADDR, 0x07, 0x09},
        {AW36515_ADDR, 0x08, 0x00},
        {AW36515_ADDR, 0x01, 0x9A},        
    };

    printf("light_level = %02x\n", light_level);
    for (int i = 0; i < ARRAY_SIZE(cmd); i++) {
        if (i2c_bus_send(AW36515_IIC_ID, cmd[i], 3)) {
                fprintf(stderr, "set led %s failed.\n", __func__);
                return -1;
        }
    }

    return 0;
}
static void strobe_nir_print_help()
{
    printf("strobe_nir level\n");
}

static void strobe_str_print_help()
{
    printf("strobe_str level\n");
}

static void torch_nir_print_help()
{
    printf("torch_nir level\n");
}

static void torch_str_print_help()
{
    printf("torch_str level\n");
}

static void strobe_nir(char *wbuf, int wbuf_len, int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
       printf("argv[%d]=%s\n", i, argv[i]);
    }

    if (argc == 1) {
        strobe_nir_print_help();
        return;
    }

    int level = atoi(argv[1]);
    drv_aw36515_strobe_nir_level(level);

}

static void strobe_str(char *wbuf, int wbuf_len, int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
       printf("argv[%d]=%s\n", i, argv[i]);
    }

    if (argc == 1) {
        strobe_str_print_help();
        return;
    }

    int level = atoi(argv[1]);
    drv_aw36515_strobe_str_level(level);

}

static void torch_nir(char *wbuf, int wbuf_len, int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
       printf("argv[%d]=%s\n", i, argv[i]);
    }

    if (argc == 1) {
        torch_nir_print_help();
        return;
    }

    int level = atoi(argv[1]);
    drv_aw36515_torch_nir_level(level);
}

static void torch_str(char *wbuf, int wbuf_len, int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
       printf("argv[%d]=%s\n", i, argv[i]);
    }

    if (argc == 1) {
        torch_str_print_help();
        return;
    }

    int level = atoi(argv[1]);
    drv_aw36515_torch_str_level(level);

}

void drv_aw36515_cmd_register(void)
{
    static const struct cli_command strobe_nir_cmd_info = {
        "strobe_nir",
        "strobe_nir level",
        strobe_nir
    };

    static const struct cli_command strobe_str_cmd_info = {
        "strobe_str",
        "strobe_str level",
        strobe_str
    };

    static const struct cli_command torch_nir_cmd_info = {
        "torch_nir",
        "torch_nir level",
        torch_nir
    };

    static const struct cli_command torch_str_cmd_info = {
        "torch_str",
        "torch_str level",
        torch_str
    };

    aos_cli_register_command(&strobe_nir_cmd_info);

    aos_cli_register_command(&strobe_str_cmd_info);

    aos_cli_register_command(&torch_nir_cmd_info);

    aos_cli_register_command(&torch_str_cmd_info);

    return;
}