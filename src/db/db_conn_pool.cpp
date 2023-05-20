/*********************************************
 *  Copyright(c) 2019
 *  All rights reserved.
 *  Author: Palmer
 ********************************************/

#include "db_conn_pool.h"
#include "iostream"

namespace db {

DbConnPool::DbConnPool() {
	is_init_ = false;
	rw_conn_busy_num_ = 0;
	rd_conn_busy_num_ = 0;
}

DbConnPool::~DbConnPool() {
	ReleaseRwPool();
	ReleaseRdPool();
}

//the pool should be initialized only once
int DbConnPool::init(const DbConf::Conf& rw_db_conf, const DbConf::Conf& rd_db_conf) {
	if(is_init_) {
		return 0;
	}
	is_init_ = true;
	rw_db_conf_ = rw_db_conf;
	rd_db_conf_ = rd_db_conf;
	bool init_failed = false;
	{
		//connection pool to master database, read & write
		std::lock_guard<std::mutex> lock(rw_conn_pool_lock_);
		for(int i = 0; i < rw_db_conf_.conn_pool_size; ++i) {
			std::shared_ptr<MysqlWrapper> conn = CreateRwConn();
			if(conn != NULL) {
				rw_conn_pool_.push_back(conn);
			} else {
				init_failed = true;
				break;
			}
		}
	}
	{
		//connection pool to slave database, read only
		std::lock_guard<std::mutex> lock(rd_conn_pool_lock_);
		for(int i = 0; i < rd_db_conf_.conn_pool_size; ++i) {
			std::shared_ptr<MysqlWrapper> conn = CreateRdConn();
			if(conn != NULL) {
				rd_conn_pool_.push_back(conn);
			} else {
				init_failed = true;
				break;
			}
		}
	}
	if(init_failed) {
		ReleaseRwPool();
		ReleaseRdPool();
		return -1;
	}
	return 0;
}

//get a database connection from the pool
std::shared_ptr<MysqlWrapper> DbConnPool::GetConn(DbConf::DbConnType db_conn_type) {
        std::cout << "type: " << db_conn_type << std::endl;
	if(db_conn_type == DbConf::DB_CONN_RW) {
        std::cout << "size: " << rw_db_conf_.conn_pool_size << std::endl;
		if(rw_db_conf_.conn_pool_size > 0) {
			return GetRwConn();
		} else {
			//master database is unavailable
		}
	} else {
		if(rd_db_conf_.conn_pool_size > 0) {
			return GetRdConn();
		} else if(rw_db_conf_.conn_pool_size > 0) {
			//slave database is unavailable, try to connect to master database
			return GetRwConn();
		} else {
			//both master and slave databases are unavailable
		}
	}
	return NULL;
}

//release the database connection to the pool
void DbConnPool::ReleaseConn(std::shared_ptr<MysqlWrapper> conn) {
	if(conn == NULL) {
		return;
	} else if(conn->db_conn_type() == DbConf::DB_CONN_RW) {
		std::lock_guard<std::mutex> lock(rw_conn_pool_lock_);
		if((int)rw_conn_pool_.size() < rw_db_conf_.conn_pool_size) {
			rw_conn_pool_.push_back(conn);
		}
		rw_conn_busy_num_--;
	} else {
		std::lock_guard<std::mutex> lock(rd_conn_pool_lock_);
		if((int)rd_conn_pool_.size() < rd_db_conf_.conn_pool_size) {
			rd_conn_pool_.push_back(conn);
		}
		rd_conn_busy_num_--;
	}
}

//close the connection if error occurs
void DbConnPool::CloseConn(std::shared_ptr<MysqlWrapper> conn) {
	if(conn == NULL) {
		return;
	} else if(conn->db_conn_type() == DbConf::DB_CONN_RW) {
		std::lock_guard<std::mutex> lock(rw_conn_pool_lock_);
		rw_conn_busy_num_--;
	} else {
		std::lock_guard<std::mutex> lock(rd_conn_pool_lock_);
		rd_conn_busy_num_--;
	}
}


int DbConnPool::rw_conn_idle_num() {
	std::lock_guard<std::mutex> lock(rw_conn_pool_lock_);
	return rw_conn_pool_.size();
}
int DbConnPool::rw_conn_busy_num() {
	std::lock_guard<std::mutex> lock(rw_conn_pool_lock_);
	return rw_conn_busy_num_;
}
int DbConnPool::rd_conn_idle_num() {
	std::lock_guard<std::mutex> lock(rd_conn_pool_lock_);
	return rd_conn_pool_.size();
}
int DbConnPool::rd_conn_busy_num() {
	std::lock_guard<std::mutex> lock(rd_conn_pool_lock_);
	return rd_conn_busy_num_;
}


//master
std::shared_ptr<MysqlWrapper> DbConnPool::GetRwConn() {
	{
		//critical region, try to get an idle database connection from the pool
		std::lock_guard<std::mutex> lock(rw_conn_pool_lock_);
		if(!rw_conn_pool_.empty()) {
			std::shared_ptr<MysqlWrapper> conn = rw_conn_pool_.front();
			rw_conn_pool_.pop_front();
			rw_conn_busy_num_++;
			return conn;
		}
	}
	//no any idle database connection in the pool, create a new connection
	std::shared_ptr<MysqlWrapper> conn = CreateRwConn();
	if(conn != NULL) {
		std::lock_guard<std::mutex> lock(rw_conn_pool_lock_);
		rw_conn_busy_num_++;
	}
	return conn;
}

std::shared_ptr<MysqlWrapper> DbConnPool::CreateRwConn() {
	std::shared_ptr<MysqlWrapper> conn(new MysqlWrapper(DbConf::DB_CONN_RW));
	if(conn->connect(rw_db_conf_) < 0) {
		return NULL;
	}
	return conn;
}

void DbConnPool::ReleaseRwPool() {
	std::lock_guard<std::mutex> lock(rw_conn_pool_lock_);
	rw_conn_pool_.clear();
}

//slave
std::shared_ptr<MysqlWrapper> DbConnPool::GetRdConn() {
	{
		//critical region, try to get an idle database connection from the pool
		std::lock_guard<std::mutex> lock(rd_conn_pool_lock_);
		if(!rd_conn_pool_.empty()) {
			std::shared_ptr<MysqlWrapper> conn = rd_conn_pool_.front();
			rd_conn_pool_.pop_front();
			rd_conn_busy_num_++;
			return conn;
		}
	}
	//no any idle database connection in the pool, create a new connection
	std::shared_ptr<MysqlWrapper> conn = CreateRdConn();
	if(conn != NULL) {
		std::lock_guard<std::mutex> lock(rd_conn_pool_lock_);
		rd_conn_busy_num_++;
	}
	return conn;
}

std::shared_ptr<MysqlWrapper> DbConnPool::CreateRdConn() {
	std::shared_ptr<MysqlWrapper> conn(new MysqlWrapper(DbConf::DB_CONN_RD));
	if(conn->connect(rd_db_conf_) < 0) {
		return NULL;
	}
	return conn;
}

void DbConnPool::ReleaseRdPool() {
	std::lock_guard<std::mutex> lock(rd_conn_pool_lock_);
	rd_conn_pool_.clear();
}

}
