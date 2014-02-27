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
 *  @param  loop        the loop to bind to
 *  @param  hostname    the hostname to connect to
 *  @param  username    the username to login with
 *  @param  password    the password to authenticate with
 *  @param  database    the database to use
 */
Connection::Connection(Loop *loop, const std::string& hostname, const std::string &username, const std::string& password, const std::string& database) :
    _connection(NULL),
    _worker(loop),
    _master()
{
    // establish the connection in the worker thread
    _worker.execute([this, hostname, username, password, database]() {
        // initialize connection object
        if ((_connection = mysql_init(NULL)) == NULL) return;

        // connect to mysql
        if (mysql_real_connect(_connection, hostname.c_str(), username.c_str(), password.c_str(), database.c_str(), 0, NULL, CLIENT_IGNORE_SIGPIPE | CLIENT_MULTI_STATEMENTS) == NULL) return;
    });
}

/**
 *  Establish a connection to mysql
 *
 *  @param  loop        the loop to bind to
 *  @param  hostname    the hostname to connect to
 *  @param  username    the username to login with
 *  @param  password    the password to authenticate with
 *  @param  database    the database to use
 *  @param  callback    the callback to inform once the connection is established or failed
 */
Connection::Connection(Loop *loop, const std::string& hostname, const std::string &username, const std::string& password, const std::string& database, const std::function<void(Connection *connection, const char *error)>& callback) :
    _connection(NULL),
    _worker(loop),
    _master()
{
    // establish the connection in the worker thread
    _worker.execute([this, hostname, username, password, database, callback]() {
        // initialize connection object
        if ((_connection = mysql_init(NULL)) == NULL)
        {
            // could not initialize connection object
            _master.execute([this, callback]() { callback(this, mysql_error(_connection)); });
            return;
        }

        // connect to mysql
        if (mysql_real_connect(_connection, hostname.c_str(), username.c_str(), password.c_str(), database.c_str(), 0, NULL, CLIENT_IGNORE_SIGPIPE | CLIENT_MULTI_STATEMENTS) == NULL)
        {
            // could not connect to mysql
            _master.execute([this, callback]() { callback(this, mysql_error(_connection)); });
            return;
        }

        // we are connected, signal success to the callback
        _master.execute([this, callback]() { callback(this, NULL); });
    });
}

/**
 *  Destructor
 */
Connection::~Connection()
{
    // close a possible connection
    if (_connection) mysql_close(_connection);
}

/**
 *  End namespace
 */
}}
