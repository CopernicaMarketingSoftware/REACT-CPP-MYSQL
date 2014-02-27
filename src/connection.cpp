/**
 *  Connection.cpp
 *
 *  Class representing a connection to a MySQL or MariaDB daemon
 *
 *  @copyright 2014 Copernica BV
 */

#include "includes.h"

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Establish a connection to mysql
 *
 *  @param  hostname    the hostname to connect to
 *  @param  username    the username to login with
 *  @param  password    the password to authenticate with
 *  @param  database    the database to use
 */
Connection::Connection(const std::string& hostname, const std::string &username, const std::string& password, const std::string& database)
{
}

/**
 *  Establish a connection to mysql
 *
 *  @param  hostname    the hostname to connect to
 *  @param  username    the username to login with
 *  @param  password    the password to authenticate with
 *  @param  database    the database to use
 *  @param  callback    the callback to inform once the connection is established or failed
 */
Connection::Connection(const std::string& hostname, const std::string &username, const std::string& password, const std::string& database, const std::function<void(Connection *connection, const char *error)>& callback)
{
}

/**
 *  End namespace
 */
}}
