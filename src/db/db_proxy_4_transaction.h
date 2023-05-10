/*********************************************
 *  Copyright(c) 2019 huishoubao.com.cn
 *  All rights reserved.
 *  Author: Palmer
 ********************************************/

#ifndef DATABASE_DB_PROXY_4_TRANSACTION_H_
#define DATABASE_DB_PROXY_4_TRANSACTION_H_

#include "db_conn_pool.h"

namespace db {

/*
 * Class DbProxy4Transaction supports simple database operation and database transaction transparently.
 * If the database proxy should have the ability to support transaction, please inherit from DbProxy4Transaction.
 */
class DbProxy4Transaction {
public:
    int StartTransaction();
    int commit();
    int rollback();

protected:
	/* no any instance */
    DbProxy4Transaction();
	~DbProxy4Transaction();
	DbProxy4Transaction(const DbProxy4Transaction&);
	DbProxy4Transaction& operator=(const DbProxy4Transaction&);

    /*
     * warning: please use below method to get the MysqlWrapper object if the database operation should support transaction!
     */
    bool GetDbConn(DbConf::DbConnType db_conn_type);

protected:
    DbConnPool* ptr_db_pool_mgr_; //singleton
    std::shared_ptr<MysqlWrapper> db_conn_;
    bool is_in_transaction_;

};

}

#endif /* DATABASE_DB_PROXY_4_TRANSACTION_H_ */
