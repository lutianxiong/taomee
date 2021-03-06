/**
 *============================================================
 *  @file      dbproxy.hpp
 *  @brief    dbproxy related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFTRD_DBPROXY_HPP_
#define KFTRD_DBPROXY_HPP_

extern "C" {
#include <libtaomee/project/types.h>
}

#include "fwd_decl.hpp"

/**
  * @brief command id for dbproxy
  */
enum db_cmd_t {
	//dbproto_pve_get_player_btl_info	= 0x06DD,
	dbproto_get_summon_list = 0x0620,
	dbproto_get_plants_list = 0x061F,
	dbproto_set_plant_status = 0x0720,
	dbproto_add_effect_to_all_plants = 0x072A,
	dbproto_add_home_log	= 0x072B,
	dbproto_pick_fruit		 = 0x0729,
	dbproto_get_home_data    = 0x0780,
//	dbproto_replace_home_exp_level = 0x0781,
//	dbproto_update_home_active_point = 0x0782,
	dbproto_set_home_update_tm    = 0x0782,

    //--------- fro decorate ----------
	dbproto_get_decorate_list   = 0x06DD,
	dbproto_set_decorate        = 0x06DE,
	dbproto_add_decorate_lv     = 0x06DF,
};

/**
  * @brief errno returned from dbproxy
  */
enum db_err_t {
	dberr_sys_error      = 1001,
	dberr_db_error       = 1002,
	dberr_net_error      = 1003,
	/*! no such user id */
	dberr_no_uid         = 1105,
};

/**
  * @brief some constants
  */
enum {
	/*! max acceptable length in bytes for each package from/to dbproxy */
	dbproto_max_len	= 8 * 1024,
};


#pragma pack(1)

/**
  * @brief db protocol type
  */
struct db_proto_t {
	/*! package length */
	uint32_t	len;
	/*! sequence number */
	uint32_t	seq;
	/*! command id */
	uint16_t	cmd;
	/*! errno */
	uint32_t	ret;
	/*! user id */
	userid_t	id;
	/*! role create time */
	uint32_t	role_tm;
	/*! package body */
	uint8_t		body[];
};

#pragma pack()

/*! for packing protocol data and send to dbproxy */
extern uint8_t dbpkgbuf[dbproto_max_len];

/*! socket fd that is connecting to dbproxy */
extern int proxysvr_fd;

/**
  * @brief send a request to db proxy
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_request_to_db(Player* p, userid_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, uint32_t body_len);

/**
  * @brief handle package return from dbproxy
  * @param dbpkg package from dbproxy
  * @param pkglen length of dbpkg
  */
void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen);

/**
  * @brief init handles to handle protocol packages from dbproxy
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_db_proto_handles();

/**
 * @brief init UDP socket for db server
 */
int init_dbsvr_udp_socket();

/**
 * @brief send a UDP request to db server
 * @param p the player who launches the request to db proxy 
 * @param id id of the requested player
 * @param cmd command id
 * @param body_buf body of the request
 * @param body_len length of the body_buf
 * @return 0 on success, -1 on error
 */
int send_udp_request_to_db(const Player* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);

/**
 * @brief report public information to server for minitor
 */
void report_add_to_monitor(const Player* p, uint32_t opt_type,uint32_t cnt, uint32_t id, uint32_t stageid);

#endif // KF_DBPROXY_HPP_

