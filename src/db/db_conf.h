/*********************************************
 *  Copyright(c) 2019
 *  All rights reserved.
 *  Author: Palmer
 ********************************************/

#ifndef DATABASE_DB_CONF_H_
#define DATABASE_DB_CONF_H_

#include <string>

namespace db {

//database configuration
class DbConf {
public:
	struct Conf_ {
		std::string host; //ip
		unsigned short port; //port
		std::string user;
		std::string password;
		std::string db_name; //database name
		std::string char_set; //e.g.: utf8
		int conn_pool_size; //the maximum idle connection number in the pool, 0 means don't create connection to the database.

		//default constructor
		Conf_() {
			host = "";
			port = 0;
			user = "";
			password = "";
			db_name = "";
			char_set = "";
			conn_pool_size = 0;
		}
		//assignment
		void set(Conf_* dst, Conf_* src) {
			if(!dst || !src || dst == src) {
				return;
			}
			dst->host = src->host;
			dst->port = src->port;
			dst->user = src->user;
			dst->password = src->password;
			dst->db_name = src->db_name;
			dst->char_set = src->char_set;
			dst->conn_pool_size = src->conn_pool_size;
		}
		//override operator=
		Conf_& operator=(const Conf_& rh) {
			if(this == &rh) {
				return *this;
			}
			set(this, (Conf_*)&rh);
			return *this;
		}
		//copy constructor
		Conf_(const Conf_&rh) {
			*this = rh;
		}
	};
	typedef struct Conf_ Conf;

	//the database connection to master or slave
	enum DbConnType {
		DB_CONN_RW = 1, //database connection to master, read & write
		DB_CONN_RD = 2 //database connection to slave, read & only
	};

};

}//namespace

#endif

