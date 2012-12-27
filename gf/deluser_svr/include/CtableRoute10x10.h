/*
 * =====================================================================================
 * 
 *       Filename:  CtableRoute10x10.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007��11��06�� 19ʱ52��58�� CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CTABLEROUTE10x10_INCL
#define  CTABLEROUTE10x10_INCL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "CtableRoute.h"


class CtableRoute10x10 : public CtableRoute {
	protected:
		virtual char*  get_table_name(uint32_t id);
	public:
		virtual ~CtableRoute10x10(void){}; 
		CtableRoute10x10(mysql_interface * db, const char * db_name_pre,  
		 const char * table_name_pre,const char* id_name );
};

#endif   /* ----- #ifndef CTABLEROUTE_INCL  ----- */
