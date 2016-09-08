/**
* @file sysctl.h
* @author 
* @version 1.0
*
* @section LICENSE
*
* This software embodies materials and concepts that are confidential to Redpine
* Signals and is made available solely pursuant to the terms of a written license
* agreement with Redpine Signals
*
* @section DESCRIPTION
*
* The file contains the Linux specific sysctl declarations
*/

#ifndef __SYSCTL_DEFINE_H_
#define __SYSCTL_DEFINE_H_

#include <linux/sysctl.h>
#include <linux/version.h>
#include "ieee80211_linux.h"

struct sysctl_ctx_list
{
	struct ctl_table_header *iv_sysctl_header;
	struct ctl_table    sysctl_list[0];
};

struct sysctl_oid
{
	uint8_t res[0];
};

#define KERNEL_VERSION_GREATER_THAN_2_6_(a) \
 (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,a))

/* Kernel version between and including a & b */
#define KERNEL_VERSION_BTWN_2_6_(a,b) \
 ((LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,a)) && \
  (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,b)))

#define SYSCTL_ADD_INT(a, b, c, d, e, f, g, h)
#define SYSCTL_ADD_PROC(a, b, c, d, e, f, g, h)
#define SYSCTL_NODE(a, b, c, d, e, f)  
#define SYSCTL_INT(a, b, c, d, e, f, g)
#define SYSCTL_PROC(a, b, c, d, e, f, g, h, i)
#define SYSCTL_CHILDREN(a)
#define sysctl_ctx_init(ctx)    register_sysctl_table(ctx)
#define sysctl_ctx_free(ctx)    unregister_sysctl_table(ctx)
#if KERNEL_VERSION_GREATER_THAN_2_6_(32)
#define IEEE80211_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos) \
        proc_dointvec(ctl, write, buffer, lenp, ppos)
#else
#define IEEE80211_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos) \
        proc_dointvec(ctl, write, filp, buffer, lenp, ppos)
#endif
#define SYSCTL_HANDLER_ARGS struct ctl_table *ctl,int write,struct file *filp,void __user *buffer,size_t *lenp,loff_t *ppos
#define SYSINIT(a, b, c, d, e) 

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#define CTL_AUTO -2
#else
#define CTL_AUTO CTL_UNNUMBERED
#endif
struct ieee80211com;
struct ieee80211vap;
void ieee80211_sysctl_attach(struct ieee80211com *ic);
void ieee80211_sysctl_detach(struct ieee80211com *ic);
void ieee80211_sysctl_vattach(struct ieee80211vap *vap);
#endif
