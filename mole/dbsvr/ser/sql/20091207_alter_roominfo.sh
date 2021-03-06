#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	alter TABLE t_roominfo_$1  
	drop candy_count,
	drop prop_count,
	drop total_score,
	drop catch_candy,
	drop candy_list,
	add socks VARBINARY(100) NOT NULL DEFAULT 0x00000000 AFTER score;
EOF
}

db_index=0
end_index=10

while [ $db_index -lt $end_index ] ; do
	dbx=`printf $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 10 ] ; do
		tbx=`printf "%d" $table_index`
		alter_user_ex_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

