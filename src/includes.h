/**
 *  Includes.h
 *
 *  Startup include file to compile the mysqlcpp library
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Dependencies
 */
#include <reactcpp.h>
#include <mysql/mysql.h>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <ctime>
#include <numeric>

/**
 *  Include other files from this library
 */
#include "resultfieldimpl.h"
#include "queryresultfield.h"
#include "resultimpl.h"
#include "queryresultimpl.h"
#include "statementresultfield.h"
#include "../include/deferred.h"
#include "../include/exception.h"
#include "../include/resultfield.h"
#include "../include/resultrow.h"
#include "../include/result.h"
#include "../include/localparameter.h"
#include "../include/connection.h"
#include "../include/parameter.h"
#include "../include/statement.h"
#include "../include/cachedstatement.h"
#include "statementintegralresultfield.h"
#include "statementdynamicresultfield.h"
#include "statementdatetimeresultfield.h"
#include "statementresultimpl.h"
#include "statementresultinfo.h"
