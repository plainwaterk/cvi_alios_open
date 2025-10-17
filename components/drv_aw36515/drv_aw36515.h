#ifndef __DRV_AW36515_H__
#define __DRV_AW36515_H__

int drv_aw36515_strobe_nir_level(int level);
int drv_aw36515_strobe_str_level(int level);
int drv_aw36515_torch_nir_level(int level);
int drv_aw36515_torch_str_level(int level);
void drv_aw36515_cmd_register(void);

#endif