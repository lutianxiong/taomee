/*
 * =====================================================================================
 *
 *       Filename:  CtableRoute100.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007��11��06�� 19ʱ52��56�� CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "CtableRoute100.h"

#include "common.h"
#include "proto.h"
#include "benchapi.h"
	// id_name : ����id_is_existed������
CtableRoute100::CtableRoute100(mysql_interface * db, const char * db_name_pre,  
		 const char * table_name_pre,const char* id_name )
	: CtableRoute(db, db_name_pre, table_name_pre, id_name )
{

}



char * CtableRoute100::get_table_name(uint32_t id)
{
	this->db->select_db(this->db_name_pre);
	sprintf (this->db_table_name,"%s.%s_%02d",
	this->db_name_pre,this->table_name_pre,id%100);
	return this->db_table_name;
}
