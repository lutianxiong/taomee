
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>
#ifdef __cplusplus
}
#endif


#include <libtaomee++/utils/md5.h> 
#include <libtaomee++/inet/pdumanip.hpp> 
#include "Csprite.h"
#include "util.h"
#include "cli_proto.h"
#include "svr_proto.h"
#include "global.h"
#include "Ctimer.h"
#include "timer_func.h"
#include "./proto/pop_switch.h"
#include "./proto/pop_switch_enum.h"
#include "./proto/main_login.h"
#include "./proto/main_login_enum.h"

int send_msg_to_db(Csprite* p, uint16_t cmd, Cmessage * c_in)
{
	return send_msg_to_db_ex(p,p->id,cmd,c_in  );
}

int send_msg_to_db_ex(Csprite* p, userid_t id, uint16_t cmd,  Cmessage * c_in )
{
	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("service_dbproxy"), 0, 65535, 1);
	}

	if ((proxysvr_fd == -1)	) {
		KDEBUG_LOG(id, "send to dbproxy failed: fd=%d ", proxysvr_fd );
		if (p) {
			if (p->get_waitcmd()== cli_login_cmd) {
				return -1;
			}
			return p->send_err( cli_err_system_error );
		}
		return 0;
	}
	static char dbbuf[ sizeof(db_proto_t) ];

	db_proto_t* pkg = (db_proto_t*)dbbuf;
	pkg->len = sizeof(db_proto_t);
	pkg->seq = (p ? ((p->fdsess->fd << 16) | p->get_waitcmd()) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	KDEBUG_LOG(pkg->id,"SO[0x%04X]",pkg->cmd );

	return net_send_msg(proxysvr_fd, (char*)dbbuf, c_in   );
}

int send_udp_post_msg_to_db(userid_t id, uint16_t cmd, Cmessage* c_in)
{
	db_proto_t head;
	head.len = sizeof(db_proto_t);
	head.cmd = cmd;
	head.id = id;
	head.seq = 0;
	head.ret = 0;

    static byte_array_t ba;
    ba.init_postion();
    ba.set_is_bigendian(false);
    ba.write_buf((const char *)&head, sizeof(db_proto_t));

	if (c_in != NULL)
        c_in->write_to_buf(ba);

	uint32_t total_size = ba.get_postion();
    *(uint32_t*)(ba.get_buf()) = total_size;

	sendto(udp_post_fd, ba.get_buf(), total_size, 0, (const sockaddr *)&udp_post_addr, sizeof(struct sockaddr_in));
	return 0;
}

int send_msg_to_switch(Csprite* p, uint16_t cmd, userid_t sender_id ,Cmessage * c_in)
{
	//Common buffer for sending package to switch

	//Connect to switch and send init package in case of disconnection
	if (switch_fd == -1) {
		connect_to_switch();
	}
	
	if ((switch_fd == -1)  ) {	
		KDEBUG_LOG(sender_id, "FAILED SENDING PACKAGE TO SWITCH\t[switch_fd=%d]", switch_fd);
		/*
		if (p) {
			return p->send_err( cli_err_system_error );
		}
		*/
		return 0;
	}

	//Build the package
	static uint8_t sw_pkg[sizeof(switch_proto_t) ];
	switch_proto_t* pkg = (switch_proto_t*)sw_pkg;
	pkg->len = sizeof(switch_proto_t );
	pkg->seq = (p ? ((p->fdsess->fd) << 16) | (p->get_waitcmd()) : 0); // set higher 16 bytes fd and lower 16 bytes waitcmd
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->sender_id  = sender_id;
	if (p) {
		KDEBUG_LOG(p->id, "TO SW\t[cmd=%u waitcmd=%u]", cmd, p->get_waitcmd());
	}

	return net_send_msg(switch_fd , (char*)pkg , c_in   );
}

/**
  * @brief connect to switch and send an init package of online information when online is launched
  */
void connect_to_switch()
{
	DEBUG_LOG("do  connect_to_switch");
	switch_fd = connect_to_service(config_get_strval("service_switch"), 0, 65535, 1);
	if (switch_fd != -1) {
		//连上时
 		is_add_timer_to_connect_switch=false;
	   	static uint32_t seqno = 0;
		sw_report_online_info_in sw_in;
	 	sw_in.domain_id= config_get_intval("domain", 0);
	 	sw_in.online_id= config_cache.bc_elem->online_id; 
	 	memcpy(sw_in.online_name,config_cache.bc_elem->online_name,sizeof(sw_in.online_name)); 
	 	memcpy(sw_in.online_ip,config_cache.bc_elem->bind_ip,sizeof(sw_in.online_ip)); 
	 	sw_in.online_port=config_cache.bc_elem->bind_port;
		sw_in.seqno=seqno;
		g_sprite_map->get_userid_list(sw_in.userid_list);

	 	sw_in.user_num=sw_in.userid_list.size() ;
	
	    ++seqno;
	    send_msg_to_switch(NULL,sw_report_online_info_cmd,0,&sw_in );

		KDEBUG_LOG(0, "BEGIN CONNECTING TO CENTRAL ONLINE AND SEND INIT PKG");
	}else{//
		if(! is_add_timer_to_connect_switch){//还没有加入定时器
			DEBUG_LOG("ADD TIMER connect_switch ");
			g_timer_map->add_timer( get_now_tv()->tv_sec+3,n_sw_report_online_info,0 );
			is_add_timer_to_connect_switch=true;
		}
	}
}


void send_sw_report_user_onoff( Csprite* p, bool is_on_online )
{
	sw_report_user_onoff_in sw_in;
	sw_in.is_on_online=is_on_online?1:0;
    send_msg_to_switch(p,sw_report_user_onoff_cmd,p->id,&sw_in );
}


