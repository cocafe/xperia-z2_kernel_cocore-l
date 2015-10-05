/* 
 * TFA98xx Speaker AMP Control
 * 
 * Author: Ji Huang <cocafehj@gmail.com> (cocafe@xda-developers.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/kobject.h>
#include <sound/soc.h>
#include "tfa/inc/Tfa98xx.h"
#include "tfa/inc/Tfa98xx_Registers.h"
#include "tfa/inc/Tfa98xx_internals.h"
#include "tfa98xx_control.h"

#define TFA98XX_CONTROL_VERSION			"alpha"

#define TOP    0
#define BOTTOM 1
#define BOTH   2

#if 0
Tfa98xx_ReadRegister16
Tfa98xx_WriteRegister16
#endif

//static Tfa98xx_handle_t tfa98xx_handles[3] = {TOP, BOTTOM, ~0};

bool speaker_mute_TOP = 0;
bool speaker_mute_BOTTOM = 0;

static const char *handle_chnl[] = {
	"TOP", 
	"BOTTOM",
	"BOTH",
};

static const int sample_rates[] = { 8, 11, 12, 16, 22, 24, 32, 44, 48 };

struct debug_reg {
	u32 handle;
	u16 addr;
	u16 regval;
};

static struct debug_reg rreg = {
	.handle = 0x00,
	.addr = 0x00,
	.regval = 0x00,
};

static struct debug_reg wreg = {
	.handle = 0x00,
	.addr = 0x00,
	.regval = 0x00,
};

static ssize_t rreg_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	u32 ret;

	ret = Tfa98xx_ReadRegister16(rreg.handle, rreg.addr, &rreg.regval);
	if (ret != Tfa98xx_Error_Ok) {
		pr_err("tfa98xx: Failed to read register\n");

		return -EIO;
	}

	return sprintf(buf, "%s %#06x %#06x\n", handle_chnl[rreg.handle], rreg.addr, rreg.regval);
}

static ssize_t rreg_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	u32 handle, addr, ret;

	if (sscanf(buf, "%u %x", &handle, &addr) == 2) {
		/* FIXME: Should check strictly */
		rreg.handle = handle;
		rreg.addr = addr;
		ret = Tfa98xx_ReadRegister16(handle, addr, &rreg.regval);
		if (ret != Tfa98xx_Error_Ok) {
			pr_err("tfa98xx: Failed to read register\n");

			return -EIO;
		}

		pr_info("tfa98xx: %s %#06x %#06x\n", handle_chnl[rreg.handle], rreg.addr, rreg.regval);

		return count;
	}

	return -EINVAL;
}
static struct kobj_attribute rreg_interface = __ATTR(regr, 0644, rreg_show, rreg_store);

static ssize_t wreg_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	u32 ret;

	ret = Tfa98xx_ReadRegister16(wreg.handle, wreg.addr, &wreg.regval);
	if (ret != Tfa98xx_Error_Ok) {
		pr_err("tfa98xx: Failed to read register\n");

		return -EIO;
	}

	return sprintf(buf, "%s %#06x %#06x\n", handle_chnl[wreg.handle], wreg.addr, wreg.regval);
}

static ssize_t wreg_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	u32 handle, addr, val, ret;

	if (sscanf(buf, "%u %x %x", &handle, &addr, &val) == 3) {
		wreg.handle = handle;
		wreg.addr = addr;
		ret = Tfa98xx_WriteRegister16(handle, addr, val);
		if (ret != Tfa98xx_Error_Ok) {
			pr_err("tfa98xx: Failed to write register\n");

			return -EIO;
		}
		
		ret = Tfa98xx_ReadRegister16(handle, addr, &wreg.regval);
		if (ret != Tfa98xx_Error_Ok) {
			pr_err("tfa98xx: Faield to read regsiter\n");

			return -EIO;
		}

		pr_info("tfa98xx: %s %#06x %#06x\n", handle_chnl[wreg.handle], wreg.addr, wreg.regval);

		return count;
	}

	return -EINVAL;
}
static struct kobj_attribute wreg_interface = __ATTR(regw, 0664, wreg_show, wreg_store);

static ssize_t battery_volt_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	u16 val;
	u32 ret;
#define PER_MV 	(5)

	ret = Tfa98xx_ReadRegister16(BOTTOM, TFA98XX_BATTERYVOLTAGE, &val);
	if (ret != Tfa98xx_Error_Ok)
		return -EIO;

	return sprintf(buf, "%umV\n", (u32)val * PER_MV);
}
static struct kobj_attribute battery_volt_interface = __ATTR(battery_volt, 0444, battery_volt_show, NULL);

/* Audio Sample Rate */
#define __SAMPLE_RATE_SYSFS(handle)								\
static ssize_t handle##_sample_rate_show							\
(struct kobject *kobj, struct kobj_attribute *attr, char *buf)					\
{												\
	u16 regval;										\
	u32 ret;										\
	int i;											\
												\
	pr_info("%s: name: %s\n", __func__, kobj->name);					\
												\
	ret = Tfa98xx_ReadRegister16(handle, TFA9897_AUDIOREG, &regval);			\
	if (ret != Tfa98xx_Error_Ok)								\
		return -EIO;									\
												\
	regval &= TFA9897_AUDIOREG_AUDFS_MSK;							\
	regval >>= TFA9897_AUDIOREG_AUDFS_POS;							\
												\
	for (i = 0; i < ARRAY_SIZE(sample_rates); i++) {					\
		sprintf(buf, "%d KHz %s\n", sample_rates[i], (i == regval) ? "(*)" : " ");	\
	}											\
												\
	return strlen(buf);									\
}												\
												\
static ssize_t handle##_sample_rate_store							\
(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)		\
{												\
	u16 regval;										\
	u32 rate;										\
	u32 ret;										\
	int i;											\
												\
	if (sscanf(buf, "%u", &rate) == 1) {							\
		for (i = 0; i < ARRAY_SIZE(sample_rates); i++) {				\
			if (sample_rates[i] == rate)						\
				rate = i;							\
		}										\
												\
		ret = Tfa98xx_ReadRegister16(handle, TFA9897_AUDIOREG, &regval);		\
		if (ret != Tfa98xx_Error_Ok)							\
			return -EIO;								\
												\
		regval &= ~(TFA9897_AUDIOREG_AUDFS_MSK);					\
		regval |= (rate << TFA9897_AUDIOREG_AUDFS_POS);					\
												\
		ret = Tfa98xx_WriteRegister16(handle, TFA9897_AUDIOREG, regval);		\
		if (ret != Tfa98xx_Error_Ok)							\
			return -EIO;								\
												\
		return count;									\
	}											\
												\
	return -EINVAL;										\
}												\
static struct kobj_attribute handle##_sample_rate_interface = 					\
	__ATTR(sample_rate, 0644, handle##_sample_rate_show, handle##_sample_rate_store);

__SAMPLE_RATE_SYSFS(TOP);
__SAMPLE_RATE_SYSFS(BOTTOM);

#define __CHNL_MUTE_SYSFS(handle)							\
static ssize_t handle##_mute_show							\
(struct kobject *kobj, struct kobj_attribute *attr, char *buf)				\
{											\
	return sprintf(buf, "%d", speaker_mute_##handle);				\
}											\
											\
static ssize_t handle##_mute_store							\
(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)	\
{											\
	u32 val, ret;									\
											\
	if (sscanf(buf, "%d", &val)) {							\
		if (!!val) {								\
			ret = Tfa98xx_SetMute(handle, Tfa98xx_Mute_Amplifier);		\
			if (ret != Tfa98xx_Error_Ok) {					\
				pr_err("%s: Failed to SetMute()\n", __func__);		\
											\
				return -EIO;						\
			}								\
			speaker_mute_##handle = 1;					\
		} else {								\
			ret = Tfa98xx_SetMute(handle, Tfa98xx_Mute_Off);		\
			if (ret != Tfa98xx_Error_Ok) {					\
				pr_err("%s: Failed to SetMute()\n", __func__);		\
											\
				return -EIO;						\
			}								\
			speaker_mute_##handle = 0;					\
		}									\
											\
		return count;								\
	}										\
											\
	return -EINVAL;									\
}											\
static struct kobj_attribute handle##_mute_interface = 					\
	__ATTR(mute, 0644, handle##_mute_show, handle##_mute_store);

__CHNL_MUTE_SYSFS(TOP);
__CHNL_MUTE_SYSFS(BOTTOM);

/* Version info */
static ssize_t version_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "TFA98xx Control, (cocafe), version: %s\n", TFA98XX_CONTROL_VERSION);
}

static ssize_t version_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	return count;
}
static struct kobj_attribute version_interface = __ATTR(version, 0644, version_show, version_store);

static struct attribute *tfa98xx_control_attrs[] = {
	&version_interface.attr,
	NULL,
};

static struct attribute_group tfa98xx_control_interface_group = {
	.attrs = tfa98xx_control_attrs,
};

static struct attribute *tfa98xx_debug_attrs[] = {
	&rreg_interface.attr,
	&wreg_interface.attr,
	&battery_volt_interface.attr,
	NULL,
};

static struct attribute_group tfa98xx_debug_interface_group = {
	.attrs = tfa98xx_debug_attrs,
	.name  = "debug",
};

static struct attribute *tfa98xx_top_attrs[] = {
	&TOP_sample_rate_interface.attr,
	&TOP_mute_interface.attr,
	NULL,
};

static struct attribute_group tfa98xx_top_interface_group = {
	.attrs = tfa98xx_top_attrs,
	.name  = "top",
};

static struct attribute *tfa98xx_bottom_attrs[] = {
	&BOTTOM_sample_rate_interface.attr,
	&BOTTOM_mute_interface.attr,
	NULL,
};

static struct attribute_group tfa98xx_bottom_interface_group = {
	.attrs = tfa98xx_bottom_attrs,
	.name  = "bottom",
};

static struct kobject *tfa98xx_control_kobject;

static int __init tfa98xx_control_init(void)
{
	int ret;

	tfa98xx_control_kobject = kobject_create_and_add("tfa98xx", kernel_kobj);
	if (!tfa98xx_control_kobject)
		pr_err("tfa98xx Control: Failed to create kobject interface\n");

	ret = sysfs_create_group(tfa98xx_control_kobject, &tfa98xx_control_interface_group);
	if (ret)
		kobject_put(tfa98xx_control_kobject);

	ret = sysfs_create_group(tfa98xx_control_kobject, &tfa98xx_debug_interface_group);
	ret = sysfs_create_group(tfa98xx_control_kobject, &tfa98xx_top_interface_group);
	ret = sysfs_create_group(tfa98xx_control_kobject, &tfa98xx_bottom_interface_group);
	if (ret)
		pr_err("tfa98xx Control: Failed to create sysfs group(s)\n");

	return ret;
}

static void __exit tfa98xx_control_exit(void)
{
	kobject_put(tfa98xx_control_kobject);

	return;
}

module_init(tfa98xx_control_init);
module_exit(tfa98xx_control_exit);

MODULE_DESCRIPTION("tfa98xx speaker amp codec controller");
MODULE_AUTHOR("Ji Huang (cocafe)");
MODULE_LICENSE("GPL v2");
