/*
 * Neighbor Awareness Networking
 *
 * Support NAN (Neighbor Awareness Networking) and RTT (Round Trip Time)
 *
 * Copyright (C) 2016, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: wl_cfgnan.h 487532 2014-06-26 05:09:36Z $
 */

#ifndef _wl_cfgnan_h_
#define _wl_cfgnan_h_

#define NAN_IOCTL_BUF_SIZE			512
#define NAN_EVENT_STR_MAX_LEN		30
#define NAN_EVENT_NAME_MAX_LEN		40
#define NAN_CONFIG_ATTR_MAX_LEN		24
#define NAN_RTT_IOVAR_BUF_SIZE		1024
#define WL_NAN_EVENT_CLEAR_BIT		32
#define NAN_EVENT_MASK_ALL			0x7fffffff

#define NAN_INVALID_ID(id)	((id > 255) ? true : false)
#define NAN_INVALID_ROLE(role)	((role > WL_NAN_ROLE_ANCHOR_MASTER) ? true : false)
#define NAN_INVALID_CHANSPEC(chanspec)	((chanspec == INVCHANSPEC) || \
	(chanspec == 0) ? true : false)
#define NAN_INVALID_EVENT(num)	(((num < WL_NAN_EVENT_START) || \
	(num >= WL_NAN_EVENT_INVALID)) ? true : false)
#define NAN_INVALID_PROXD_EVENT(num)	((num != WLC_E_PROXD_NAN_EVENT) ? \
	true : false)
#define NAN_EVENT_BIT(event) (1U << (event - WL_NAN_EVENT_START))

#define SUPP_EVENT_PREFIX		"CTRL-EVENT-"
#define EVENT_RTT_STATUS_STR	"NAN-RTT-STATUS"

#define TIMESTAMP_PREFIX	"TSF="			/* timestamp */
#define AMR_PREFIX			"AMR="			/* anchor master rank */
#define DISTANCE_PREFIX		"DIST="			/* distance */
#define ATTR_PREFIX			"ATTR="			/* attribute */
#define ROLE_PREFIX			"ROLE="			/* role */
#define CHAN_PREFIX			"CHAN="			/* channel */
#define BITMAP_PREFIX		"BMAP="			/* bitmap */
#define DEBUG_PREFIX		"DEBUG="		/* debug enable/disable flag */
#define DW_LEN_PREFIX		"DW_LEN="		/* discovery window length */
#define DW_INT_PREFIX		"DW_INT="		/* discovery window interval */
#define STATUS_PREFIX		"STATUS="		/* status */
#define PUB_ID_PREFIX		"PUB_ID="		/* publisher id */
#define SUB_ID_PREFIX		"SUB_ID="		/* subscriber id */
#define PUB_PR_PREFIX		"PUB_PR="		/* publish period */
#define PUB_INT_PREFIX		"PUB_INT="		/* publish interval (ttl) */
#define CLUS_ID_PREFIX		"CLUS_ID="		/* cluster id */
#define IF_ADDR_PREFIX		"IF_ADDR="		/* IF address */
#define MAC_ADDR_PREFIX		"MAC_ADDR="		/* mac address */
#define SVC_HASH_PREFIX		"SVC_HASH="		/* service hash */
#define SVC_INFO_PREFIX		"SVC_INFO="		/* service information */
#define HOP_COUNT_PREFIX	"HOP_COUNT="	/* hop count */
#define MASTER_PREF_PREFIX	"MASTER_PREF="	/* master preference */
#define ACTIVE_OPTION		"ACTIVE"		/* Active Subscribe. */
#define SOLICITED_OPTION	"SOLICITED"		/* Solicited Publish. */
#define UNSOLICITED_OPTION	"UNSOLICITED"	/* Unsolicited Publish. */
/* anchor master beacon transmission time */
#define AMBTT_PREFIX		"AMBTT="
/* passive scan period for cluster merge */
#define SCAN_PERIOD_PREFIX	"SCAN_PERIOD="
/* passive scan interval for cluster merge */
#define SCAN_INTERVAL_PREFIX	"SCAN_INTERVAL="
#define BCN_INTERVAL_PREFIX		"BCN_INTERVAL="

typedef struct nan_str_data {
	u8 *data;
	u32 dlen;
} nan_str_data_t;

typedef struct nan_config_attr {
	char name[NAN_CONFIG_ATTR_MAX_LEN];	/* attribute name */
	u16 type;							/* attribute xtlv type */
} nan_config_attr_t;

typedef struct nan_cmd_data {
	nan_config_attr_t attr;			/* set config attributes */
	nan_str_data_t svc_hash;		/* service hash */
	nan_str_data_t svc_info;		/* service information */
	struct ether_addr mac_addr;		/* mac address */
	struct ether_addr clus_id;		/* cluster id */
	struct ether_addr if_addr;		/* if addr */
	u32 beacon_int;					/* beacon interval */
	u32 pub_int;					/* publish interval (ttl) */
	u32 pub_pr;						/* publish period */
	u32 bmap;						/* bitmap */
	u32 role;						/* role */
	u16 pub_id;						/* publisher id */
	u16 sub_id;						/* subscriber id */
	uint32 flags;					/* Flag bits */
	u16 dw_len;						/* discovery window length */
	u16 master_pref;				/* master preference */
	chanspec_t chanspec;			/* channel */
	u8 debug_flag;					/* debug enable/disable flag */
} nan_cmd_data_t;

typedef int (nan_func_t)(struct net_device *ndev, struct bcm_cfg80211 *cfg,
	char *cmd, nan_cmd_data_t *cmd_data);

typedef struct nan_cmd {
	const char *name;					/* command name */
	nan_func_t *func;					/* command hadler */
} nan_cmd_t;

typedef struct nan_event_hdr {
	u16 event_subtype;
	u32 flags;							/* future use */
} nan_event_hdr_t;

typedef struct wl_nan_tlv_data {
	wl_nan_status_t nstatus;			/* status data */
	wl_nan_disc_params_t params;		/* discovery parameters */
	struct ether_addr mac_addr;			/* peer mac address */
	struct ether_addr clus_id;			/* cluster id */
	nan_str_data_t svc_info;			/* service info */
	nan_str_data_t vend_info;			/* vendor info */
	/* anchor master beacon transmission time */
	u32 ambtt;
	u32 dev_role;						/* device role */
	u16 inst_id;						/* instance id */
	u16 pub_id;							/* publisher id */
	u16 sub_id;							/* subscriber id */
	u16 master_pref;					/* master preference */
	chanspec_t chanspec;				/* channel */
	u8 amr[NAN_MASTER_RANK_LEN];		/* anchor master role */
	u8 svc_name[WL_NAN_SVC_HASH_LEN];	/* service name */
	u8 hop_count;						/* hop count */
	u8 enabled;							/* nan status flag */
	nan_scan_params_t scan_params;		/* scan_param */
} wl_nan_tlv_data_t;

extern int wl_cfgnan_set_vars_cbfn(void *ctx, uint8 *tlv_buf,
	uint16 type, uint16 len);
extern int wl_cfgnan_enable_events(struct net_device *ndev,
	struct bcm_cfg80211 *cfg);
extern int wl_cfgnan_start_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_stop_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_support_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_status_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_pub_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_sub_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_cancel_pub_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_cancel_sub_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_transmit_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_set_config_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_rtt_config_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_rtt_find_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_debug_handler(struct net_device *ndev,
	struct bcm_cfg80211 *cfg, char *cmd, nan_cmd_data_t *cmd_data);
extern int wl_cfgnan_cmd_handler(struct net_device *dev,
	struct bcm_cfg80211 *cfg, char *cmd, int cmd_len);
extern s32 wl_cfgnan_notify_nan_status(struct bcm_cfg80211 *cfg,
	bcm_struct_cfgdev *cfgdev, const wl_event_msg_t *e, void *data);
extern s32 wl_cfgnan_notify_proxd_status(struct bcm_cfg80211 *cfg,
	bcm_struct_cfgdev *cfgdev, const wl_event_msg_t *e, void *data);

#endif	/* _wl_cfgnan_h_ */
