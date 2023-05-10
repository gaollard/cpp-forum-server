/*********************************************
 *  Copyright(c) 2019 huishoubao.com.cn
 *  All rights reserved.
 *  Author: Palmer
 ********************************************/

#ifndef DB_CONN_POOL_H_
#define DB_CONN_POOL_H_

#include <list>
#include <mutex>
#include <memory>
#include "db_conf.h"
#include "mysql_wrapper.h"
//#include "../util/singleton.h"

namespace db {

/*
 * the database connection pool Singleton, it contains a read & write connection pool and a read only connection pool.
 */
class DbConnPool : public util::Singleton<DbConnPool> {
public:
	//the pool should be initialized only once
	int init(const DbConf::Conf& rw_db_conf, const DbConf::Conf& rd_db_conf);
	//get a database connection from the pool
	std::shared_ptr<MysqlWrapper> GetConn(DbConf::DbConnType db_conn_type);
	//release the connection to the pool
	void ReleaseConn(std::shared_ptr<MysqlWrapper> conn);
	//close the connection if error occurs
	void CloseConn(std::shared_ptr<MysqlWrapper> conn);

	//master
	int rw_conn_idle_num();
	int rw_conn_busy_num();
	//slave
	int rd_conn_idle_num();
	int rd_conn_busy_num();

public:
	/* no any instance */
	DbConnPool() ;
	~DbConnPool();

private:
	/* class uncopyable */
	DbConnPool(const DbConnPool&);
	DbConnPool& operator=(const DbConnPool&);
	//friend class Singleton<DbConnPool>; //it's my friend

	//master
	std::shared_ptr<MysqlWrapper> GetRwConn();
	std::shared_ptr<MysqlWrapper> CreateRwConn();
	void ReleaseRwPool();
	//slave
	std::shared_ptr<MysqlWrapper> GetRdConn();
	std::shared_ptr<MysqlWrapper> CreateRdConn();
	void ReleaseRdPool();

private:
	//connection pool to master database, read & write
	DbConf::Conf rw_db_conf_;
	std::mutex rw_conn_pool_lock_;
	std::list< std::shared_ptr<MysqlWrapper> > rw_conn_pool_; //the idle database connections to master

	//connection pool to slave database, read only
	DbConf::Conf rd_db_conf_;
	std::mutex rd_conn_pool_lock_;
	std::list< std::shared_ptr<MysqlWrapper> > rd_conn_pool_; //the idle database connections to slave

	bool is_init_; //the pool should be initialized only once
	int rw_conn_busy_num_; //the read & write database connection number using by business
	int rd_conn_busy_num_; //the read only database connection number using by business
};

} //namespace

#endif

