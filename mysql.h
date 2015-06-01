/**
 *  MysqlCpp.h
 *
 *  Main header file for the MYSQL-CPP wrapper library
 *
 *  @copyright 2014 Copernica BV
 */

#pragma once

/**
 *  Dependencies
 */
#include <reactcpp.h>
#include <mysql/mysql.h>
#include <functional>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <numeric>

/**
 *  Other include files
 */
#include <reactcpp/mysql/deferred.h>
#include <reactcpp/mysql/exception.h>
#include <reactcpp/mysql/resultfield.h>
#include <reactcpp/mysql/resultrow.h>
#include <reactcpp/mysql/result.h>
#include <reactcpp/mysql/parameter.h>
#include <reactcpp/mysql/localparameter.h>
#include <reactcpp/mysql/connection.h>
#include <reactcpp/mysql/statement.h>
#include <reactcpp/mysql/cachedstatement.h>
