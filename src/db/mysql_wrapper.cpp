/*********************************************
 *  Copyright(c) 2019
 *  All rights reserved.
 *  Author: Palmer
 ********************************************/
#include <iostream>
#include "./mysql_wrapper.h"

namespace db {

std::mutex MysqlWrapper::mutex_mysql_conn_;

MysqlWrapper::MysqlWrapper(DbConf::DbConnType db_conn_type) : db_conn_type_(db_conn_type) {
}

MysqlWrapper::MysqlWrapper(MysqlWrapper* rh) : db_conn_type_(rh->db_conn_type_) {
	this->sql_conn_ = rh->sql_conn_;
	this->sql_row_ = rh->sql_row_;
}

MysqlWrapper::~MysqlWrapper() {
	std::lock_guard<std::mutex> lock(mutex_mysql_conn_);
	mysql_close(&sql_conn_); //close the mysql connection
	mysql_thread_end();
}

int MysqlWrapper::connect(const DbConf::Conf& db_conf) {
	std::lock_guard<std::mutex> lock(mutex_mysql_conn_);
	mysql_thread_init();
	if(mysql_init(&sql_conn_) == NULL) {
        std::cout << "connect error" << std::endl;
		return -1;
	}
	/*
	 * The MySQL client library can perform an automatic reconnection to the server if it finds that the connection is down when you attempt to send a statement to the server to be executed.
	 * If auto-reconnect is enabled, the library tries once to reconnect to the server and send the statement again.
	 * Auto-reconnect is disabled by default.
	 */
	char option = 1;
	mysql_options(&sql_conn_, MYSQL_OPT_RECONNECT, (char*)&option); //re-connect if the connection is broken
	if(!db_conf.char_set.empty()) {
		mysql_options(&sql_conn_, MYSQL_SET_CHARSET_NAME, db_conf.char_set.c_str()); //set the charset
	}
	if(mysql_real_connect(&sql_conn_, db_conf.host.c_str(), db_conf.user.c_str(), db_conf.password.c_str(), db_conf.db_name.c_str(), db_conf.port, NULL, 0) == NULL) {
        std::cout << "mysql_real_connect error" << std::endl;
		return -1;
	}
	return 0;
}

int MysqlWrapper::GetRecordCount(const std::string& sql) {
	std::shared_ptr<MYSQL_RES> sql_result = execute(sql);
	if(!sql_result) {
		return -1;
	}
	sql_row_ = mysql_fetch_row(sql_result.get());
	int column_count = mysql_num_fields(sql_result.get());
	if(sql_row_ == NULL || column_count == 0) {
		return -2;
	}
	int count = std::stoi(sql_row_[0]);
	return count;
}

int MysqlWrapper::UpdateDb(const std::string& sql) {
	if(db_conn_type_ != DbConf::DB_CONN_RW) {
		//the database connection doesn't support insert/update/delete operation
		return -2;
	} else if(mysql_real_query(&sql_conn_, sql.c_str(), sql.length()) != 0) {
		//an error occurred in database operation
		return -1;
	}
	return (int)mysql_affected_rows(&sql_conn_);
}

/*
 * text data query
 */
int MysqlWrapper::SelectSingleLineQuery(const std::string& sql, std::map<std::string, std::string>& result) {
	std::shared_ptr<MYSQL_RES> sql_result = execute(sql);
	if(!sql_result) {
		return -1;
	}
	int column_count = mysql_num_fields(sql_result.get());
	MYSQL_FIELD* sql_fields = mysql_fetch_fields(sql_result.get());
	sql_row_ = mysql_fetch_row(sql_result.get());
	if(sql_fields && sql_row_) {
		for(int i = 0; i < column_count; ++i) {

			if(sql_row_[i] != NULL) {
				result[sql_fields[i].name] = sql_row_[i];
			} else {
				result[sql_fields[i].name] = "";
			}
		}
	}
	return 0;
}

int MysqlWrapper::SelectQuery(const std::string& sql, std::vector< std::map<std::string, std::string> >& result) {
	std::shared_ptr<MYSQL_RES> sql_result = execute(sql);
	if(!sql_result) {
		return -1;
	}
	int column_count = mysql_num_fields(sql_result.get());
	MYSQL_FIELD* sql_fields = mysql_fetch_fields(sql_result.get());
	if(sql_fields == NULL) {
		return -2;
	}
	while((sql_row_ = mysql_fetch_row(sql_result.get())) != NULL) {
		std::map<std::string, std::string> row_map;
		for(int i = 0; i < column_count; ++i) {
			if(sql_row_[i] != NULL) {
				row_map[sql_fields[i].name] = sql_row_[i];
			} else {
				row_map[sql_fields[i].name] = "";
			}
		}
		result.push_back(row_map);
	}
	return 0;
}


/*
 * blob data query
 */
int MysqlWrapper::BlobSelectSingleLineQuery(const std::string& sql, std::map<std::string, std::string>& result) {
	std::shared_ptr<MYSQL_RES> sql_result = execute(sql);
	if(!sql_result) {
		return -1;
	}
	int column_count = mysql_num_fields(sql_result.get());
	MYSQL_FIELD* sql_fields = mysql_fetch_fields(sql_result.get());
	sql_row_ = mysql_fetch_row(sql_result.get());
	if(sql_fields && sql_row_) {
		unsigned long* lengths = mysql_fetch_lengths(sql_result.get()); //get the data length
		for(int i = 0; i < column_count; ++i) {
			if(sql_row_[i] != NULL) {
				result[sql_fields[i].name] = std::string(sql_row_[i], lengths[i]);
			} else {
				result[sql_fields[i].name] = "";
			}
		}
	}
	return 0;
}

int MysqlWrapper::BlobSelectQuery(const std::string& sql, std::vector< std::map<std::string, std::string> >& result) {
	std::shared_ptr<MYSQL_RES> sql_result = execute(sql);
	if(!sql_result) {
		return -1;
	}
	int column_count = mysql_num_fields(sql_result.get());
	MYSQL_FIELD* sql_fields = mysql_fetch_fields(sql_result.get());
	if(sql_fields == NULL) {
		return -2;
	}
	while((sql_row_ = mysql_fetch_row(sql_result.get())) != NULL) {
		std::map<std::string, std::string> row_map;
		unsigned long* lengths = mysql_fetch_lengths(sql_result.get()); //get the data length
		for(int i = 0; i < column_count; ++i) {
			if(sql_row_[i] != NULL) {
				row_map[sql_fields[i].name] = std::string(sql_row_[i], lengths[i]);
			} else {
				row_map[sql_fields[i].name] = "";
			}
		}
		result.push_back(row_map);
	}
	return 0;
}


/*
 * MySQL transaction
 */
int MysqlWrapper::StartTransaction() {
	bool OFF = 0;
	if(mysql_autocommit(&sql_conn_, OFF) != 0) {
		return -1;
	}
	return 0;
}

int MysqlWrapper::commit() {
	if(mysql_commit(&sql_conn_) != 0) {
		return -1;
	}
	return 0;
}

int MysqlWrapper::rollback() {
	if(mysql_rollback(&sql_conn_) != 0) {
		return -1;
	}
	return 0;
}

int MysqlWrapper::EndTransaction() {
	bool ON = 1;
	if(mysql_autocommit(&sql_conn_, ON) != 0) {
		return -1;
	}
	return 0;
}


unsigned long MysqlWrapper::MySqlEscapeString(char *to, const char *from, unsigned long length) {
	if(to == NULL || from == NULL) {
		return 0;
	}
	return mysql_real_escape_string(&sql_conn_, to, from, length);
}

std::string MysqlWrapper::MySqlEscapeString(const std::string& src) {
    if(src.empty()) {
        return "";
    }
    char* dst = new char[src.length() * 2 + 1];
    if(dst == NULL) {
        return "";
    }
    unsigned long ret = mysql_real_escape_string(&sql_conn_, dst, src.c_str(), src.length());
    std::string dst_str(dst, ret);
    delete dst;
    return dst_str;
}

bool MysqlWrapper::ping() {
	return mysql_ping(&sql_conn_) == 0;
}

std::string MysqlWrapper::LastError() {
	return mysql_error(&sql_conn_);
}


std::shared_ptr<MYSQL_RES> MysqlWrapper::execute(const std::string& sql) {
	if(mysql_real_query(&sql_conn_, sql.c_str(), sql.length()) != 0) {
		return NULL;
	}
	MYSQL_RES* sql_result = mysql_use_result(&sql_conn_);
	if(sql_result == NULL) {
		return NULL;
	}
	std::shared_ptr<MYSQL_RES> auto_free_reply(sql_result, mysql_free_result);
	return auto_free_reply;
}

}
