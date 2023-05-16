/*********************************************
 *  Copyright(c) 2019
 *  All rights reserved.
 *  Author: Palmer
 ********************************************/

#ifndef DATABASE_MYSQL_WRAPPER_H_
#define DATABASE_MYSQL_WRAPPER_H_

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include <mysql/mysql.h>
#include "db_conf.h"

namespace db {

/*
 * MySQL API wrapper
 * class method return value: 0 - success, <0 - error(-1: MySQL system error, -2: caller SQL statement error).
 */
class MysqlWrapper {
public:
	MysqlWrapper(DbConf::DbConnType db_conn_type);
	MysqlWrapper(MysqlWrapper* rh);
	~MysqlWrapper();
	DbConf::DbConnType db_conn_type() { return db_conn_type_; }

	//connect to MySQL database
	int connect(const DbConf::Conf& db_conf);
	//get the count of sql query result, select count(*) from ...
	int GetRecordCount(const std::string& sql);
	//insert/update/delete database operation, return the affected rows
	int UpdateDb(const std::string& sql);

	/*
	 * text data query
	 */
	//query only one row result, map structure: first = field name, second = field values
	int SelectSingleLineQuery(const std::string& sql, std::map<std::string, std::string>& result);
	//query multiple row results, map structure: first = field name, second = field values
	int SelectQuery(const std::string& sql, std::vector< std::map<std::string, std::string> >& result);

	/*
	 * blob data query
	 */
	//query only one row result, map structure: first = field name, second = field values
	int BlobSelectSingleLineQuery(const std::string& sql, std::map<std::string, std::string>& result);
	//query multiple row results, map structure: first = field name, second = field values
	int BlobSelectQuery(const std::string& sql, std::vector< std::map<std::string, std::string> >& result);

	/*
	 * MySQL transaction
	 */
	int StartTransaction();
	int commit();
	int rollback();
	int EndTransaction();

	//escapes special characters in a string
	unsigned long MySqlEscapeString(char *to, const char *from, unsigned long length);
	std::string MySqlEscapeString(const std::string& src);
	//check whether the connection to the server is working
	bool ping();
	//the latest error message
	std::string LastError();

protected:
	std::shared_ptr<MYSQL_RES> execute(const std::string& sql);

private:
	/*
	 * make sure the thread-safe MySql connecting
	 * Warning: mysql_init & mysql_real_connect are NOT thread-safe
	 */
	static std::mutex mutex_mysql_conn_;
	MYSQL sql_conn_;
	MYSQL_ROW sql_row_;

	const DbConf::DbConnType db_conn_type_; //database connection to master or slave
};

}

#endif /* DATABASE_MYSQL_WRAPPER_H_ */
