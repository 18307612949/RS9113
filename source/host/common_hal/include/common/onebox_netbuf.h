/**
 * @file onebox_netbuf.h
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
 * This file contians the netbuf control block data structures used
 * to communicate packets to and from the device .
 */

#ifndef __ONEBOX_NETBUF_H__
#define __ONEBOX_NETBUF_H__

typedef struct netbuf_ctrl_block_s 
{
	struct netbuf_ctrl_block_s *next; // next ptr
	uint8_t                    *data;          // Data address
	void                       *head;          // head address
	void                       *dev;
	void                       *pkt_addr;
	void                       *ni;                   //store the node reference in ni 
	uint16_t                    len;            // length of the packet
	uint16_t                    aggr_len;       // Total aggregation length
	uint8_t                     mac_hdr_len;    // MAC header length(80211)
	uint8_t                     hdr_pad_len;    // Header padding length
	uint8_t                     sta_id;         // Station id
	uint8_t                     tid;            // Traffic identifier
	uint16_t                     flags;          // Flags
	uint8_t                     aggr_flag;      // Aggregation flags
	uint8_t                     aggr_pcnt;      // No. of AMPDU aggregated packets
	uint8_t                     agg_retry;      // Is it a retry
	uint8_t                     retry_count;    // No. of times this packet retried
	uint8_t                     priority;      
	uint8_t                     skb_priority;
	uint8_t          	    bt_pkt_type;
	uint8_t          	    zigb_pkt_type;
	uint8_t          	    tx_pkt_type;
	uint8_t          	    rx_pkt_type;
	uint8_t                     vap[6];        // Pointer to vap structure reserved
	/* Removed frag_flags, need to use flags in case required */
}netbuf_ctrl_block_t;

#endif
