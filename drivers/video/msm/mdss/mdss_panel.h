/* Copyright (c) 2008-2015, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/*
 * NOTE: This file has been modified by Sony Mobile Communications Inc.
 * Modifications are Copyright (c) 2014 Sony Mobile Communications Inc,
 * and licensed under the license of the file.
 */

#ifndef MDSS_PANEL_H
#define MDSS_PANEL_H

#include <linux/platform_device.h>
#include <linux/stringify.h>
#include <linux/types.h>
#include <linux/debugfs.h>

/* panel id type */
struct panel_id {
	u16 id;
	u16 type;
};

#define DEFAULT_FRAME_RATE	60
#define DEFAULT_ROTATOR_FRAME_RATE 120
#define ROTATOR_LOW_FRAME_RATE 30
#define MDSS_DSI_RST_SEQ_LEN	10
/* worst case prefill lines for all chipsets including all vertical blank */
#define MDSS_MDP_MAX_PREFILL_FETCH 25

#define OVERRIDE_CFG	"override"
#define SIM_PANEL	"sim"
#define SIM_SW_TE_PANEL	"sim-swte"
#define SIM_HW_TE_PANEL	"sim-hwte"

/* panel type list */
#define NO_PANEL		0xffff	/* No Panel */
#define MDDI_PANEL		1	/* MDDI */
#define EBI2_PANEL		2	/* EBI2 */
#define LCDC_PANEL		3	/* internal LCDC type */
#define EXT_MDDI_PANEL		4	/* Ext.MDDI */
#define TV_PANEL		5	/* TV */
#define HDMI_PANEL		6	/* HDMI TV */
#define DTV_PANEL		7	/* DTV */
#define MIPI_VIDEO_PANEL	8	/* MIPI */
#define MIPI_CMD_PANEL		9	/* MIPI */
#define WRITEBACK_PANEL		10	/* Wifi display */
#define LVDS_PANEL		11	/* LVDS */
#define EDP_PANEL		12	/* LVDS */

static inline const char *mdss_panel2str(u32 panel)
{
	static const char *names[] = {
#define PANEL_NAME(n) [n ## _PANEL] = __stringify(n)
		PANEL_NAME(MIPI_VIDEO),
		PANEL_NAME(MIPI_CMD),
		PANEL_NAME(EDP),
		PANEL_NAME(HDMI),
		PANEL_NAME(DTV),
		PANEL_NAME(WRITEBACK),
#undef PANEL_NAME
	};

	if (panel >= ARRAY_SIZE(names) || !names[panel])
		return "UNKNOWN";

	return names[panel];
}

/* panel class */
enum {
	DISPLAY_LCD = 0,	/* lcd = ebi2/mddi */
	DISPLAY_LCDC,		/* lcdc */
	DISPLAY_TV,		/* TV Out */
	DISPLAY_EXT_MDDI,	/* External MDDI */
	DISPLAY_WRITEBACK,
};

/* panel device locaiton */
enum {
	DISPLAY_1 = 0,		/* attached as first device */
	DISPLAY_2,		/* attached on second device */
	DISPLAY_3,              /* attached on third writeback device */
	MAX_PHYS_TARGET_NUM,
};

enum {
	MDSS_PANEL_INTF_INVALID = -1,
	MDSS_PANEL_INTF_DSI,
	MDSS_PANEL_INTF_EDP,
	MDSS_PANEL_INTF_HDMI,
};

enum {
	MDSS_PANEL_POWER_OFF = 0,
	MDSS_PANEL_POWER_ON,
	MDSS_PANEL_POWER_LP1,
	MDSS_PANEL_POWER_LP2,
};

enum {
	MDSS_PANEL_BLANK_BLANK = 0,
	MDSS_PANEL_BLANK_UNBLANK,
	MDSS_PANEL_BLANK_LOW_POWER,
};

enum {
	MODE_GPIO_NOT_VALID = 0,
	MODE_GPIO_HIGH,
	MODE_GPIO_LOW,
};

/*
 * enum sim_panel_modes - Different panel modes for simulator panels
 *
 * @SIM_MODE:		Disables all host reads for video mode simulator panels.
 * @SIM_SW_TE_MODE:	Disables all host reads and genereates the SW TE. Used
 *                      for cmd mode simulator panels.
 * @SIM_HW_TE_MODE:	Disables all host reads and expects TE from hardware
 *                      (terminator card). Used for cmd mode simulator panels.
 */
enum {
	SIM_MODE = 1,
	SIM_SW_TE_MODE,
	SIM_HW_TE_MODE,
};

struct mdss_rect {
	u16 x;
	u16 y;
	u16 w;
	u16 h;
};

#define MDSS_MAX_PANEL_LEN      256
#define MDSS_INTF_MAX_NAME_LEN 5
struct mdss_panel_intf {
	char name[MDSS_INTF_MAX_NAME_LEN];
	int  type;
};

struct mdss_panel_cfg {
	char arg_cfg[MDSS_MAX_PANEL_LEN + 1];
	int  pan_intf;
	bool lk_cfg;
	bool init_done;
};

#define MDP_INTF_DSI_CMD_FIFO_UNDERFLOW		0x0001
#define MDP_INTF_DSI_VIDEO_FIFO_OVERFLOW	0x0002

struct mdss_intf_recovery {
	void (*fxn)(void *ctx, int event);
	void *data;
};

/**
 * enum mdss_intf_events - Different events generated by MDP core
 *
 * @MDSS_EVENT_RESET:		MDP control path is being (re)initialized.
 * @MDSS_EVENT_LINK_READY	Interface data path inited to ready state.
 * @MDSS_EVENT_UNBLANK:		Sent before first frame update from MDP is
 *				sent to panel.
 * @MDSS_EVENT_PANEL_ON:	After first frame update from MDP.
 * @MDSS_EVENT_BLANK:		MDP has no contents to display only blank screen
 *				is shown in panel. Sent before panel off.
 * @MDSS_EVENT_PANEL_OFF:	MDP has suspended frame updates, panel should be
 *				completely shutdown after this call.
 * @MDSS_EVENT_CLOSE:		MDP has tore down entire session.
 * @MDSS_EVENT_SUSPEND:		Propagation of power suspend event.
 * @MDSS_EVENT_RESUME:		Propagation of power resume event.
 * @MDSS_EVENT_CHECK_PARAMS:	Event generated when a panel reconfiguration is
 *				requested including when resolution changes.
 *				The event handler receives pointer to
 *				struct mdss_panel_info and should return one of:
 *				 - negative if the configuration is invalid
 *				 - 0 if there is no panel reconfig needed
 *				 - 1 if reconfig is needed to take effect
 * @MDSS_EVENT_CONT_SPLASH_BEGIN: Special event used to handle transition of
 *				display state from boot loader to panel driver.
 *				The event handler will disable the panel.
 * @MDSS_EVENT_CONT_SPLASH_FINISH: Special event used to handle transition of
 *				display state from boot loader to panel driver.
 *				The event handler will enable the panel and
 *				vote for the display clocks.
 * @MDSS_EVENT_PANEL_UPDATE_FPS: Event to update the frame rate of the panel.
 * @MDSS_EVENT_FB_REGISTERED:	Called after fb dev driver has been registered,
 *				panel driver gets ptr to struct fb_info which
 *				holds fb dev information.
 * @MDSS_EVENT_PANEL_CLK_CTRL:	panel clock control
 *				- 0 clock disable
 *				- 1 clock enable
 * @MDSS_EVENT_DSI_CMDLIST_KOFF: acquire dsi_mdp_busy lock before kickoff.
 * @MDSS_EVENT_ENABLE_PARTIAL_ROI: Event to update ROI of the panel.
 * @MDSS_EVENT_DSI_STREAM_SIZE: Event to update DSI controller's stream size
 * @MDSS_EVENT_DSI_UPDATE_PANEL_DATA: Event to update the dsi driver structures
 *				based on the dsi mode passed as argument.
 *				- 0: update to video mode
 *				- 1: update to command mode
 * @MDSS_EVENT_REGISTER_RECOVERY_HANDLER: Event to recover the interface in
 *					case there was any errors detected.
 * @MDSS_EVENT_DSI_PANEL_STATUS: Event to check the panel status
 *				<= 0: panel check fail
 *				>  0: panel check success
 * @MDSS_EVENT_DSI_DYNAMIC_SWITCH: Send DCS command to panel to initiate
 *				switching panel to new mode
 *				- MIPI_VIDEO_PANEL: switch to video mode
 *				- MIPI_CMD_PANEL: switch to command mode
 * @MDSS_EVENT_DSI_RECONFIG_CMD: Setup DSI controller in new mode
 *				- MIPI_VIDEO_PANEL: switch to video mode
 *				- MIPI_CMD_PANEL: switch to command mode
 * @MDSS_EVENT_DSI_RESET_WRITE_PTR: Reset the write pointer coordinates on
 *				the panel.
 * @MDSS_EVENT_PANEL_TIMING_SWITCH: Panel timing switch is requested.
 *				Argument provided is new panel timing.
 */
enum mdss_intf_events {
	MDSS_EVENT_RESET = 1,
	MDSS_EVENT_LINK_READY,
	MDSS_EVENT_UNBLANK,
	MDSS_EVENT_PANEL_ON,
	MDSS_EVENT_BLANK,
	MDSS_EVENT_PANEL_OFF,
	MDSS_EVENT_CLOSE,
	MDSS_EVENT_SUSPEND,
	MDSS_EVENT_RESUME,
	MDSS_EVENT_CHECK_PARAMS,
	MDSS_EVENT_CONT_SPLASH_BEGIN,
	MDSS_EVENT_CONT_SPLASH_FINISH,
	MDSS_EVENT_PANEL_UPDATE_FPS,
	MDSS_EVENT_FB_REGISTERED,
	MDSS_EVENT_PANEL_CLK_CTRL,
	MDSS_EVENT_DSI_CMDLIST_KOFF,
	MDSS_EVENT_ENABLE_PARTIAL_ROI,
	MDSS_EVENT_DSI_STREAM_SIZE,
	MDSS_EVENT_DSI_UPDATE_PANEL_DATA,
	MDSS_EVENT_REGISTER_RECOVERY_HANDLER,
	MDSS_EVENT_DSI_PANEL_STATUS,
	MDSS_EVENT_DSI_DYNAMIC_SWITCH,
	MDSS_EVENT_DSI_RECONFIG_CMD,
	MDSS_EVENT_DSI_RESET_WRITE_PTR,
	MDSS_EVENT_PANEL_TIMING_SWITCH,
#ifdef CONFIG_FB_MSM_MDSS_SPECIFIC_PANEL
	MDSS_EVENT_DISP_ON,
#endif	/* CONFIG_FB_MSM_MDSS_SPECIFIC_PANEL */
};

#ifdef CONFIG_FB_MSM_MDSS_SPECIFIC_PANEL
struct change_fps_rtn_pos {
	int num;
	int *pos;
};

struct change_fps {
	bool enable;
	u32 disp_clk;
	u32 dric_vbp;
	u32 dric_vfp;
	bool rtn_adj;
	struct change_fps_rtn_pos rtn_pos;
	bool te_c_update;
	u32 threshold;
	u32 te_c_60fps[2];
	u32 te_c_45fps[2];
	u32 te_c_pos[2];
	bool susres_mode;
};
#endif	/* CONFIG_FB_MSM_MDSS_SPECIFIC_PANEL */

struct lcd_panel_info {
	u32 h_back_porch;
	u32 h_front_porch;
	u32 h_pulse_width;
	u32 v_back_porch;
	u32 v_front_porch;
	u32 v_pulse_width;
	u32 border_clr;
	u32 underflow_clr;
	u32 hsync_skew;
	u32 border_top;
	u32 border_bottom;
	u32 border_left;
	u32 border_right;
	/* Pad width */
	u32 xres_pad;
	/* Pad height */
	u32 yres_pad;
	u32 h_polarity;
	u32 v_polarity;
#ifdef CONFIG_FB_MSM_MDSS_SPECIFIC_PANEL
	struct change_fps chg_fps;
#endif	/* CONFIG_FB_MSM_MDSS_SPECIFIC_PANEL */
};


/* DSI PHY configuration */
struct mdss_dsi_phy_ctrl {
	uint32_t regulator[7];
	uint32_t timing[12];
	uint32_t ctrl[4];
	uint32_t strength[2];
	char bistctrl[6];
	uint32_t pll[21];
	char lanecfg[45];
	bool reg_ldo_mode;
};

enum dynamic_mode_switch {
	DYNAMIC_MODE_SWITCH_DISABLED = 0,
	DYNAMIC_MODE_SWITCH_SUSPEND_RESUME,
	DYNAMIC_MODE_SWITCH_IMMEDIATE,
	DYNAMIC_MODE_RESOLUTION_SWITCH_IMMEDIATE,
};

enum dynamic_switch_modes {
	SWITCH_MODE_UNKNOWN = 0,
	SWITCH_TO_CMD_MODE,
	SWITCH_TO_VIDEO_MODE,
	SWITCH_RESOLUTION,
};

struct mipi_panel_info {
	char boot_mode;	/* identify if mode switched from starting mode */
	char mode;		/* video/cmd */
	char interleave_mode;
	char crc_check;
	char ecc_check;
	char dst_format;	/* shared by video and command */
	char data_lane0;
	char data_lane1;
	char data_lane2;
	char data_lane3;
	char dlane_swap;	/* data lane swap */
	char rgb_swap;
	char b_sel;
	char g_sel;
	char r_sel;
	char rx_eot_ignore;
	char tx_eot_append;
	char t_clk_post; /* 0xc0, DSI_CLKOUT_TIMING_CTRL */
	char t_clk_pre;  /* 0xc0, DSI_CLKOUT_TIMING_CTRL */
	char vc;	/* virtual channel */
	struct mdss_dsi_phy_ctrl dsi_phy_db;
	/* video mode */
	char pulse_mode_hsa_he;
	char hfp_power_stop;
	char hbp_power_stop;
	char hsa_power_stop;
	char eof_bllp_power_stop;
	char last_line_interleave_en;
	char bllp_power_stop;
	char traffic_mode;
	char frame_rate;
	/* command mode */
	char interleave_max;
	char insert_dcs_cmd;
	char wr_mem_continue;
	char wr_mem_start;
	char te_sel;
	char stream;	/* 0 or 1 */
	char mdp_trigger;
	char dma_trigger;
	/* Dynamic Switch Support */
	enum dynamic_mode_switch dms_mode;

	u32 pixel_packing;
	u32 dsi_pclk_rate;
	/* The packet-size should not bet changed */
	char no_max_pkt_size;
	/* Clock required during LP commands */
	bool force_clk_lane_hs;

	char vsync_enable;
	char hw_vsync_mode;

	char lp11_init;
	u32  init_delay;

	int input_fpks;
};

struct edp_panel_info {
	char frame_rate;	/* fps */
};

enum dynamic_fps_update {
	DFPS_SUSPEND_RESUME_MODE,
	DFPS_IMMEDIATE_CLK_UPDATE_MODE,
	DFPS_IMMEDIATE_PORCH_UPDATE_MODE_VFP,
	DFPS_IMMEDIATE_PORCH_UPDATE_MODE_HFP,
	DFPS_MODE_MAX
};

enum lvds_mode {
	LVDS_SINGLE_CHANNEL_MODE,
	LVDS_DUAL_CHANNEL_MODE,
};

struct lvds_panel_info {
	enum lvds_mode channel_mode;
	/* Channel swap in dual mode */
	char channel_swap;
};

struct fbc_panel_info {
	u32 enabled;
	u32 target_bpp;
	u32 comp_mode;
	u32 qerr_enable;
	u32 cd_bias;
	u32 pat_enable;
	u32 vlc_enable;
	u32 bflc_enable;

	u32 line_x_budget;
	u32 block_x_budget;
	u32 block_budget;

	u32 lossless_mode_thd;
	u32 lossy_mode_thd;
	u32 lossy_rgb_thd;
	u32 lossy_mode_idx;

	u32 slice_height;
	bool pred_mode;
	bool enc_mode;
	u32 max_pred_err;
};

struct mdss_mdp_pp_tear_check {
	u32 tear_check_en;
	u32 sync_cfg_height;
	u32 vsync_init_val;
	u32 sync_threshold_start;
	u32 sync_threshold_continue;
	u32 start_pos;
	u32 rd_ptr_irq;
	u32 refx100;
};

struct mdss_livedisplay_ctx;

struct mdss_panel_info {
	u32 xres;
	u32 yres;
	u32 physical_width;
	u32 physical_height;
	u32 bpp;
	u32 type;
	u32 wait_cycle;
	u32 pdest;
	u32 brightness_max;
	u32 bl_max;
	u32 bl_min;
	u32 fb_num;
	u32 clk_rate;
	u32 clk_min;
	u32 clk_max;
	u32 mdp_transfer_time_us;
	u32 frame_count;
	u32 is_3d_panel;
	u32 out_format;
	u32 rst_seq[MDSS_DSI_RST_SEQ_LEN];
	u32 rst_seq_len;
	u32 vic; /* video identification code */
	struct mdss_rect roi;
	int pwm_pmic_gpio;
	int pwm_lpg_chan;
	int pwm_period;
	bool dynamic_fps;
	bool ulps_feature_enabled;
	bool ulps_suspend_enabled;
	bool panel_ack_disabled;
	bool esd_check_enabled;
	char dfps_update;
	int new_fps;
	int panel_max_fps;
	int panel_max_vtotal;
	u32 mode_gpio_state;
	u32 xstart_pix_align;
	u32 width_pix_align;
	u32 ystart_pix_align;
	u32 height_pix_align;
	u32 min_width;
	u32 min_height;
	u32 min_fps;
	u32 max_fps;

	u32 cont_splash_enabled;
	bool esd_rdy;
	bool partial_update_supported; /* value from dts if pu is supported */
	bool partial_update_enabled; /* is pu currently allowed */
	u32 dcs_cmd_by_left;
	u32 partial_update_roi_merge;
	struct ion_handle *splash_ihdl;
	int panel_power_state;
	int blank_state;

	uint32_t panel_dead;
	u32 panel_force_dead;
	u32 panel_orientation;
	bool dynamic_switch_pending;
	bool is_lpm_mode;
	bool is_split_display;

	bool is_prim_panel;

	/* refer sim_panel_modes enum for different modes */
	u8 sim_panel_mode;

	char panel_name[MDSS_MAX_PANEL_LEN];
	struct mdss_mdp_pp_tear_check te;

	struct lcd_panel_info lcdc;
	struct fbc_panel_info fbc;
	struct mipi_panel_info mipi;
	struct lvds_panel_info lvds;
	struct edp_panel_info edp;
	const char *panel_id_name;

	struct mdss_livedisplay_ctx *livedisplay;

	/* debugfs structure for the panel */
	struct mdss_panel_debugfs_info *debugfs_info;

#ifdef CONFIG_FB_MSM_MDSS_SPECIFIC_PANEL
	int dsi_master;
	int disp_on_in_hs;
	int wait_time_before_on_cmd;
#endif
};

struct mdss_panel_timing {
	struct list_head list;
	const char *name;

	u32 xres;
	u32 yres;

	u32 h_back_porch;
	u32 h_front_porch;
	u32 h_pulse_width;
	u32 hsync_skew;
	u32 v_back_porch;
	u32 v_front_porch;
	u32 v_pulse_width;

	u32 border_top;
	u32 border_bottom;
	u32 border_left;
	u32 border_right;

	u32 clk_rate;
	char frame_rate;

	struct fbc_panel_info fbc;
	struct mdss_mdp_pp_tear_check te;
};

struct mdss_panel_data {
	struct mdss_panel_info panel_info;
	void (*set_backlight) (struct mdss_panel_data *pdata, u32 bl_level);
	unsigned char *mmss_cc_base;
#ifdef CONFIG_FB_MSM_MDSS_SPECIFIC_PANEL
	struct platform_device *panel_pdev;
	void (*crash_counter_reset) (void);
	void (*blackscreen_det) (struct mdss_panel_data *pdata);
	void (*fff_time_update) (struct mdss_panel_data *pdata);
	bool resume_started;
#endif

	/**
	 * event_handler() - callback handler for MDP core events
	 * @pdata:	Pointer refering to the panel struct associated to this
	 *		event. Can be used to retrieve panel info.
	 * @e:		Event being generated, see enum mdss_intf_events
	 * @arg:	Optional argument to pass some info from some events.
	 *
	 * Used to register handler to be used to propagate different events
	 * happening in MDP core driver. Panel driver can listen for any of
	 * these events to perform appropriate actions for panel initialization
	 * and teardown.
	 */
	int (*event_handler) (struct mdss_panel_data *pdata, int e, void *arg);

	struct list_head timings_list;
	struct mdss_panel_timing *current_timing;
	bool active;

	struct mdss_panel_data *next;
};

struct mdss_panel_debugfs_info {
	struct dentry *root;
	struct dentry *parent;
	struct mdss_panel_info panel_info;
	u32 override_flag;
	struct mdss_panel_debugfs_info *next;
};

/**
 * mdss_get_panel_framerate() - get panel frame rate based on panel information
 * @panel_info:	Pointer to panel info containing all panel information
 */
static inline u32 mdss_panel_get_framerate(struct mdss_panel_info *panel_info)
{
	u32 frame_rate, pixel_total;

	if (panel_info == NULL)
		return DEFAULT_FRAME_RATE;

	switch (panel_info->type) {
	case MIPI_VIDEO_PANEL:
	case MIPI_CMD_PANEL:
		frame_rate = panel_info->mipi.frame_rate;
		break;
	case EDP_PANEL:
		frame_rate = panel_info->edp.frame_rate;
		break;
	case WRITEBACK_PANEL:
		frame_rate = DEFAULT_FRAME_RATE;
		break;
	default:
		pixel_total = (panel_info->lcdc.h_back_porch +
			  panel_info->lcdc.h_front_porch +
			  panel_info->lcdc.h_pulse_width +
			  panel_info->xres) *
			 (panel_info->lcdc.v_back_porch +
			  panel_info->lcdc.v_front_porch +
			  panel_info->lcdc.v_pulse_width +
			  panel_info->yres);
		if (pixel_total)
			frame_rate = panel_info->clk_rate / pixel_total;
		else
			frame_rate = DEFAULT_FRAME_RATE;

		break;
	}
	return frame_rate;
}

/*
 * mdss_panel_get_vtotal() - return panel vertical height
 * @pinfo:	Pointer to panel info containing all panel information
 *
 * Returns the total height of the panel including any blanking regions
 * which are not visible to user but used to calculate panel pixel clock.
 */
static inline int mdss_panel_get_vtotal(struct mdss_panel_info *pinfo)
{
	return pinfo->yres + pinfo->lcdc.v_back_porch +
			pinfo->lcdc.v_front_porch +
			pinfo->lcdc.v_pulse_width+
			pinfo->lcdc.border_top +
			pinfo->lcdc.border_bottom;
}

/*
 * mdss_panel_get_htotal() - return panel horizontal width
 * @pinfo:	Pointer to panel info containing all panel information
 * @consider_fbc: true to factor fbc settings, false to ignore.
 *
 * Returns the total width of the panel including any blanking regions
 * which are not visible to user but used for calculations. For certain
 * usescases where the fbc parameters need to be ignored like bandwidth
 * calculation, the appropriate flag can be passed.
 */
static inline int mdss_panel_get_htotal(struct mdss_panel_info *pinfo, bool
		consider_fbc)
{
	int adj_xres = pinfo->xres + pinfo->lcdc.border_left +
				pinfo->lcdc.border_right;

	if (consider_fbc && pinfo->fbc.enabled)
		adj_xres = mult_frac(adj_xres,
				pinfo->fbc.target_bpp, pinfo->bpp);

	return adj_xres + pinfo->lcdc.h_back_porch +
		pinfo->lcdc.h_front_porch +
		pinfo->lcdc.h_pulse_width;
}

int mdss_register_panel(struct platform_device *pdev,
	struct mdss_panel_data *pdata);

/*
 * mdss_panel_is_power_off: - checks if a panel is off
 * @panel_power_state: enum identifying the power state to be checked
 */
static inline bool mdss_panel_is_power_off(int panel_power_state)
{
	return (panel_power_state == MDSS_PANEL_POWER_OFF);
}

/**
 * mdss_panel_is_power_on_interactive: - checks if a panel is on and interactive
 * @panel_power_state: enum identifying the power state to be checked
 *
 * This function returns true only is the panel is fully interactive and
 * opertaing in normal mode.
 */
static inline bool mdss_panel_is_power_on_interactive(int panel_power_state)
{
	return (panel_power_state == MDSS_PANEL_POWER_ON);
}

/**
 * mdss_panel_is_panel_power_on: - checks if a panel is on
 * @panel_power_state: enum identifying the power state to be checked
 *
 * A panel is considered to be on as long as it can accept any commands
 * or data. Sometimes it is posible to program the panel to be in a low
 * power non-interactive state. This function returns false only if panel
 * has explicitly been turned off.
 */
static inline bool mdss_panel_is_power_on(int panel_power_state)
{
	return !mdss_panel_is_power_off(panel_power_state);
}

/**
 * mdss_panel_is_panel_power_on_lp: - checks if a panel is in a low power mode
 * @pdata: pointer to the panel struct associated to the panel
 * @panel_power_state: enum identifying the power state to be checked
 *
 * This function returns true if the panel is in an intermediate low power
 * state where it is still on but not fully interactive. It may or may not
 * accept any commands and display updates.
 */
static inline bool mdss_panel_is_power_on_lp(int panel_power_state)
{
	return !mdss_panel_is_power_off(panel_power_state) &&
		!mdss_panel_is_power_on_interactive(panel_power_state);
}

/**
 * mdss_panel_is_panel_power_on_ulp: - checks if panel is in ultra low power mode
 * @pdata: pointer to the panel struct associated to the panel
 * @panel_power_state: enum identifying the power state to be checked
 *
 * This function returns true if the panel is in a ultra low power
 * state where it is still on but cannot recieve any display updates.
 */
static inline bool mdss_panel_is_power_on_ulp(int panel_power_state)
{
	return panel_power_state == MDSS_PANEL_POWER_LP2;
}

/**
 * mdss_panel_intf_type: - checks if a given intf type is primary
 * @intf_val: panel interface type of the individual controller
 *
 * Individual controller queries with MDP to check if it is
 * configured as the primary interface.
 *
 * returns a pointer to the configured structure mdss_panel_cfg
 * to the controller that's configured as the primary panel interface.
 * returns NULL on error or if @intf_val is not the configured
 * controller.
 */

struct mdss_panel_cfg *mdss_panel_intf_type(int intf_val);

/**
 * mdss_panel_get_boot_cfg() - checks if bootloader config present
 *
 * Function returns true if bootloader has configured the parameters
 * for primary controller and panel config data.
 *
 * returns true if bootloader configured, else false
 */
int mdss_panel_get_boot_cfg(void);

/**
 * mdss_is_ready() - checks if mdss is probed and ready
 *
 * Checks if mdss resources have been initialized
 *
 * returns true if mdss is ready, else returns false.
 */
bool mdss_is_ready(void);
int mdss_rect_cmp(struct mdss_rect *rect1, struct mdss_rect *rect2);

/**
 * mdss_panel_override_te_params() - overrides TE params to enable SW TE
 * @pinfo: panel info
 */
void mdss_panel_override_te_params(struct mdss_panel_info *pinfo);

#ifdef CONFIG_FB_MSM_MDSS
int mdss_panel_debugfs_init(struct mdss_panel_info *panel_info);
void mdss_panel_debugfs_cleanup(struct mdss_panel_info *panel_info);
void mdss_panel_debugfsinfo_to_panelinfo(struct mdss_panel_info *panel_info);

/*
 * mdss_panel_info_from_timing() - return panel timing with matching name
 * @pdata:	pointer to panel data struct containing list of panel timings
 * @name:	name of the panel timing to be returned
 *
 * Looks through list of timings provided in panel data and returns pointer
 * to panel timing matching it. If none is found, NULL is returned.
 */
void mdss_panel_info_from_timing(struct mdss_panel_timing *pt,
		struct mdss_panel_info *pinfo);

/**
 * mdss_panel_get_timing_by_name() - return panel timing with matching name
 * @pdata:	pointer to panel data struct containing list of panel timings
 * @name:	name of the panel timing to be returned
 *
 * Looks through list of timings provided in panel data and returns pointer
 * to panel timing matching it. If none is found, NULL is returned.
 */
struct mdss_panel_timing *mdss_panel_get_timing_by_name(
		struct mdss_panel_data *pdata,
		const char *name);
#else
static inline int mdss_panel_debugfs_init(
			struct mdss_panel_info *panel_info) { return 0; };
static inline void mdss_panel_debugfs_cleanup(
			struct mdss_panel_info *panel_info) { };
static inline void mdss_panel_debugfsinfo_to_panelinfo(
			struct mdss_panel_info *panel_info) { };
static inline void mdss_panel_info_from_timing(struct mdss_panel_timing *pt,
		struct mdss_panel_info *pinfo) { };
static inline struct mdss_panel_timing *mdss_panel_get_timing_by_name(
		struct mdss_panel_data *pdata,
		const char *name) { return NULL; };
#endif

struct msm_fb_data_type;
#if defined(CONFIG_DEBUG_FS) && defined(CONFIG_FB_MSM_MDSS_SPECIFIC_PANEL)
void mipi_dsi_panel_create_debugfs(struct msm_fb_data_type *mfd);
#else
static inline void mipi_dsi_panel_create_debugfs(struct msm_fb_data_type *mfd)
{
	/* empty */
}
#endif
#endif /* MDSS_PANEL_H */
