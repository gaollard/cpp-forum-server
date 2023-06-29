//
// Created by Xiong Gao on 2023/4/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
//#include <errmsg.h>

int db_query()
{
    MYSQL mysql;
    mysql_init(&mysql);

    if (mysql_real_connect(&mysql, "192.168.1.9", "root",
                           "123456", "test", 0, NULL, 0))
    {
        printf("Connect success\n");

        // insert
        if (mysql_query(&mysql, "insert into user(name) values('Ann')") != 0)
        {
            fprintf(stderr, "mysql_query failed:\n\tcode:%d\n\treason:%s\n", mysql_errno(&mysql), mysql_error(&mysql));
        }
        else
        {
            printf("Insert success, affect row are %lu\n", mysql_affected_rows(&mysql));
        }

        mysql_close(&mysql);
    }
    else
    {
        fprintf(stderr, "Connect failed:\n");
        if (mysql_errno(&mysql))
        {
            printf("\terror code is %d\n\treason:%s\n", mysql_errno(&mysql), mysql_error(&mysql));
        }
    }

    return 0;
}