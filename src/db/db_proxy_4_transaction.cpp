/*********************************************
 *  Copyright(c) 2019
 *  All rights reserved.
 *  Author: Palmer
 ********************************************/

#include <sstream>
#include "./db_proxy_4_transaction.h"

using namespace std;

namespace db {

DbProxy4Transaction::DbProxy4Transaction() {
    //ptr_db_pool_mgr_ = Singleton<DbConnPool>::instance(); //please initialize DbConnPool before this constructor.
	ptr_db_pool_mgr_ = DbConnPool::getInstance();
    db_conn_ = NULL;
    is_in_transaction_ = false;
}

DbProxy4Transaction::~DbProxy4Transaction() {
	if(is_in_transaction_) {
		//the transaction is not finished correctly
		is_in_transaction_ = false;
		ptr_db_pool_mgr_->CloseConn(db_conn_);
		db_conn_ = NULL;
	} else if(db_conn_) {
		ptr_db_pool_mgr_->ReleaseConn(db_conn_);
        db_conn_ = NULL;
    }
}

/*
 * warning: please use below method to get the MysqlWrapper object if the database operation should support transaction!
 */
bool DbProxy4Transaction::GetDbConn(DbConf::DbConnType db_conn_type) {
	if(is_in_transaction_) {
		if(!db_conn_) {
			return false;
		}
		return true;
	} else {
		if(!db_conn_) {
			db_conn_ = ptr_db_pool_mgr_->GetConn(db_conn_type);
			return (db_conn_ != NULL);
		} else if(db_conn_->db_conn_type() != db_conn_type) {
			//release the previous connection for it has different connection type.
			ptr_db_pool_mgr_->ReleaseConn(db_conn_);
			db_conn_ = ptr_db_pool_mgr_->GetConn(db_conn_type);
			return (db_conn_ != NULL);
		}
		return true;
	}
}


int DbProxy4Transaction::StartTransaction() {
	if(!db_conn_) {
		db_conn_ = ptr_db_pool_mgr_->GetConn(DbConf::DB_CONN_RW);
	} else if(db_conn_->db_conn_type() != DbConf::DB_CONN_RW) {
		//release the previous connection for it has different connection type.
		ptr_db_pool_mgr_->ReleaseConn(db_conn_);
		db_conn_ = ptr_db_pool_mgr_->GetConn(DbConf::DB_CONN_RW);
	}
	if(db_conn_ == NULL || db_conn_->StartTransaction() < 0) {
		return -1;
	}
	is_in_transaction_ = true;
    return 0;
}

int DbProxy4Transaction::commit() {
	is_in_transaction_ = false;
    if(db_conn_->commit() < 0 || db_conn_->EndTransaction() < 0) {
    	ptr_db_pool_mgr_->CloseConn(db_conn_);
    	db_conn_ = NULL;
    	return -1;
    }
    return 0;
}

int DbProxy4Transaction::rollback() {
	is_in_transaction_ = false;
    if (db_conn_->rollback() < 0 || db_conn_->EndTransaction() < 0) {
    	ptr_db_pool_mgr_->CloseConn(db_conn_);
    	db_conn_ = NULL;
        return -1;
    }
    return 0;
}

}
