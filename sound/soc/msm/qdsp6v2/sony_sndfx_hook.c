/* Copyright (c) 2016, cocafe@xda-developers.com
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
#include "sony_sndfx_hook.h"

#define CLEARSTEREO_OFF			(0)
#define CLEARSTEREO_DEF 		(-34)
#define CLEARSTEREO_MIN			(-83)
#define CLEARSTEREO_MAX			(-20)
#define BAND_NUM			(6)
#define BAND_MIN_LEVEL			(-10)
#define BAND_MAX_LEVEL			(10)
#define VPT_MODE_MAX			(4)

enum BAND_TYPE {
	CLEARBASS = 0,
	HZ_400,
	HZ_1000,
	HZ_2500,
	HZ_6000,
	HZ_16000,
};

enum VPT_MODE {
	STUDIO = 0, 
	CLUB,
	RESERVED,
	MUSIC_HALL,
	VPT_OFF,
};

static char *cleareq_name[] = {
	"Bass",
	"400Hz",
	"1KHz",
	"2.5KHz",
	"6KHz",
	"16KHz",
};

static char *vpt_name[] = {
	"Studio",
	"Club",
	"(Reserved)",
	"Music hall",
	"Off",
};

uint16_t sndfx_enable_hook = 0;
uint16_t sndfx_enable = 0;

uint16_t clearstereo_hook = 0;
int32_t clearstereo_coef = CLEARSTEREO_DEF;

uint16_t cleareq_hook = 0;
int16_t cleareq_coef[BAND_NUM] = { 0, 0, 0, 0, 0, 0 };

uint16_t clearvpt_hook = 0;
uint16_t clearvpt_mode = 4;

int16_t sndfx_hook_get_eqband(int index)
{
	return cleareq_coef[index];
}

static ssize_t clearaudio_enable_show(struct kobject *kobj,
	struct kobj_attribute *attr,
	char *buf
)
{
	sprintf(buf, "ClearAudio hook: %s\n", sndfx_enable_hook ? "on" : "off");
	sprintf(buf, "%sClearAudio enable: %d\n", buf, sndfx_enable);
	
	return strlen(buf);
}

static ssize_t clearaudio_enable_store(struct kobject *kobj,
	struct kobj_attribute *attr,
	const char *buf,
	size_t count)
{
	int32_t val;
	
	if (sysfs_streq(buf, "on")) {
		sndfx_enable_hook = 1;
		return count;
	}
	
	if (sysfs_streq(buf, "off")) {
		sndfx_enable_hook = 0;
		return count;
	}
	
	if (sscanf(buf, "%d", &val)) {
		sndfx_enable = !!val;
		return count;
	}
	
	return -EINVAL;
}

static struct kobj_attribute clearaudio_enable_interface =
	__ATTR(fx_enable, 0644, clearaudio_enable_show, clearaudio_enable_store);
	
static ssize_t clearstereo_show(struct kobject *kobj,
	struct kobj_attribute *attr,
	char *buf)
{
	sprintf(buf, "ClearStereo hook: %s\n", clearstereo_hook ? "on" : "off");
	sprintf(buf, "%sClearStereo coef: %d\n", buf, clearstereo_coef);
	
	return strlen(buf);
}

static ssize_t clearstereo_store(struct kobject *kobj,
	struct kobj_attribute *attr,
	const char *buf,
	size_t count)
{
	int32_t coef;
	
	if (sysfs_streq(buf, "on")) {
		clearstereo_hook = 1;
		return count;
	}
	
	if (sysfs_streq(buf, "off")) {
		clearstereo_hook = 0;
		return count;
	}
	
	if (sscanf(buf, "%d", &coef)) {
		if (coef < CLEARSTEREO_MIN || coef > CLEARSTEREO_MAX)
			return -EINVAL;
			
		clearstereo_coef = (int16_t)coef;
		
		return count;	
	}
	
	return -EINVAL;
}

static struct kobj_attribute clearstereo_interface =
	__ATTR(clearstereo, 0644, clearstereo_show, clearstereo_store);

static ssize_t cleareq_show(struct kobject *kobj,
	struct kobj_attribute *attr,
	char *buf)
{
	int i;
	
	sprintf(buf, "Equalizer hook: %s\n\n", cleareq_hook ? "on" : "off");
	
	for (i = 0; i < ARRAY_SIZE(cleareq_coef); i++) {
		sprintf(buf, "%s<%d> %s:\t\t%d\n", buf, i, 
			cleareq_name[i], cleareq_coef[i]);
	}
	
	return strlen(buf);
}

static ssize_t cleareq_store(struct kobject *kobj,
	struct kobj_attribute *attr,
	const char *buf,
	size_t count
)
{
	uint32_t band;
	int32_t coef;
	
	if (sysfs_streq(buf, "on")) {
		cleareq_hook = 1;
		return count;
	}
	
	if (sysfs_streq(buf, "off")) {
		cleareq_hook = 0;
		return count;
	}
	
	if (sscanf(buf, "%d %d", &band, &coef) == 2) {
		if (band >= BAND_NUM)
			return -EINVAL;
			
		if (coef < BAND_MIN_LEVEL || coef > BAND_MAX_LEVEL)
			return -EINVAL;
			
		cleareq_coef[band] = (int16_t)coef;
		return count;
	}
	
	return -EINVAL;
}

static struct kobj_attribute cleareq_interface =
	__ATTR(equlizer, 0644, cleareq_show, cleareq_store);
	
static ssize_t clearvpt_show(struct kobject *kobj,
	struct kobj_attribute *attr,
	char *buf)
{
	int i;
	
	sprintf(buf, "ClearVPT hook: %s\n\n", clearvpt_hook ? "on" : "off");
	
	for (i = 0; i < ARRAY_SIZE(vpt_name); i++) {
		sprintf(buf, "%s<%d> %s:\t\t%s\n",
			buf, i, vpt_name[i], (clearvpt_mode == i) ? "*" : " ");
	}
	
	return strlen(buf);
}

static ssize_t clearvpt_store(struct kobject *kobj,
	struct kobj_attribute *attr,
	const char *buf,
	size_t count
)
{
	uint32_t mode;
	
	if (sysfs_streq(buf, "on")) {
		clearvpt_hook = 1;
		return count;
	}
	
	if (sysfs_streq(buf, "off")) {
		clearvpt_hook = 0;
		return count;
	}
	
	if (sscanf(buf, "%d", &mode)) {
		if (mode > VPT_MODE_MAX)
			return -EINVAL;
			
		clearvpt_mode = (uint16_t)mode;
		return count;
	}
	
	return -EINVAL;
}

static struct kobj_attribute clearvpt_interface =
	__ATTR(clearvpt, 0644, clearvpt_show, clearvpt_store);

static struct attribute *sony_sndfx_attrs[] = {
	NULL,
};

static struct attribute_group sony_sndfx_interface_group = {
	.attrs = sony_sndfx_attrs,
};

static struct attribute *clearaudio_attrs[] = {
	&clearaudio_enable_interface.attr,
	&clearstereo_interface.attr,
	&cleareq_interface.attr,
	&clearvpt_interface.attr,
	NULL,
};

static struct attribute_group clearaudio_interface_group = {
	.name = "clearaudio",
	.attrs = clearaudio_attrs,
};

static struct kobject *sony_sndfx_hook_kobj;

static __init int sony_sndfx_hook_init(void)
{
	int ret;
	
	sony_sndfx_hook_kobj = kobject_create_and_add("sndfx", kernel_kobj);
	if (!sony_sndfx_hook_kobj) {
		pr_err("%s: Failed to create sysfs kobject\n", __func__);
		return -EINVAL;
	}
	
	ret = sysfs_create_group(sony_sndfx_hook_kobj,
		&sony_sndfx_interface_group);
	if (ret) {
		pr_err("%s: Failed to add interface group\n", __func__);
		kobject_put(sony_sndfx_hook_kobj);
		
		return -EINVAL;
	}
	
	ret = sysfs_create_group(sony_sndfx_hook_kobj,
		&clearaudio_interface_group);
	if (ret) {
		pr_err("%s: Failed to add interface group(s)\n", __func__);
		kobject_put(sony_sndfx_hook_kobj);
		
		return -EINVAL;
	}
	
	return ret;
}

static __exit void sony_sndfx_hook_exit(void)
{
	kobject_put(sony_sndfx_hook_kobj);
}

module_init(sony_sndfx_hook_init);
module_exit(sony_sndfx_hook_exit);
