/**
 *  Connection.cpp
 *
 *  Class representing a connection to a MySQL or MariaDB daemon
 *
 *  @copyright 2014 Copernica BV
 */

#include "includes.h"
#include "library.h"

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Initializes the MySQL library
 *
 *  The library will be automatically de-initialized
 *  on program termination.
 */
static void initialize()
{
    // keep a single static library instance available
    static Library library;
}

/**
 *  Establish a connection to mysql
 *
 *  @param  loop        the loop to bind to
 *  @param  hostname    the hostname to connect to
 *  @param  username    the username to login with
 *  @param  password    the password to authenticate with
 *  @param  database    the database to use
 */
Connection::Connection(Loop *loop, const std::string& hostname, const std::string &username, const std::string& password, const std::string& database, uint64_t flags) :
    _connection(nullptr),
    _reconnected(false),
    _worker(),
    _master(loop)
{
    // initialize the library
    initialize();

    // establish the connection in the worker thread
    _worker.execute([this, hostname, username, password, database, flags]() {
        // initialize connection object
        if ((_connection = mysql_init(nullptr)) == nullptr)
        {
            // could not initialize connection object
            if (_connectCallback) _master.execute([this]() { _connectCallback(mysql_error(_connection)); });
            return;
        }

        // let us know when we are reconnected
        mysql_options(_connection, MYSQL_OPT_RECONNECT, &_reconnected);

        // connect to mysql
        if (mysql_real_connect(_connection, hostname.c_str(), username.c_str(), password.c_str(), database.c_str(), 0, nullptr, flags) == nullptr)
        {
            // could not connect to mysql
            if (_connectCallback) _master.execute([this]() { _connectCallback(mysql_error(_connection)); });
            return;
        }

        // we are connected, signal success to the callback
        if (_connectCallback) _master.execute([this]() { _connectCallback(nullptr); });
    });
}

/**
 *  Destructor
 */
Connection::~Connection()
{
    // clean up mysql data when the worker stops
    _worker.execute([this]() {
        // close a possible connection
        if (_connection) mysql_close(_connection);

        // this would be nice with a unique_ptr, but
        // that cannot be easily done due to circular
        // dependencies, so we do this by hand.
        for (auto &statement : _statements) delete statement.second;

        // clean up thread-local mysql data
        mysql_thread_end();
    });
}

/**
 *  Get a call when the connection succeeds or fails
 *
 *  @param  callback    the callback that will be informed of the connection status
 */
void Connection::onConnected(const std::function<void(const char *error)>& callback)
{
    // store callback for later
    _connectCallback = callback;
}

/**
 *  Retrieve or create a cached prepared statement
 *
 *  @param  query   the query to use for preparing the statement
 */
Statement *Connection::statement(const char *query)
{
    // are we reconnected?
    if (_reconnected)
    {
        // delete all statements
        for (auto &statement : _statements) delete statement.second;
        _statements.clear();

        // clear reconnect flag
        _reconnected = false;
    }

    // find a possibly existing statement
    auto iter = _statements.find(query);

    // do we already have this statement?
    if (iter != _statements.end()) return iter->second;

    // create a new statement and store it
    auto *statement = new Statement(this, query);
    _statements[query] = statement;

    // return the newfangled statement
    return statement;
}

/**
 *  Parse the string and replace all placeholders with
 *  the provided values.
 *
 *  The callback is executed with the result.
 *
 *  @param  query       the query to parse
 *  @param  callback    the callback to give the result
 *  @param  parameters  placeholder values
 *  @param  count       number of placeholder values
 */
void Connection::prepare(const std::string& query, LocalParameter *parameters, size_t count, const std::function<void(const std::string& query)>& callback)
{
    // execute prepare in worker thread
    _worker.execute([this, callback, query, parameters, count] () {
        /**
        *  Calculate the maximum storage size for the parameters.
        *
        *  Initializes to the length of the given query, and adds
        *  the size of each parameter to the total.
        */
        size_t size = query.size();

        // add all elements
        for (size_t i = 0; i < count; ++i) size += parameters[i].size();

        // the parsed query result
        std::string result;
        result.reserve(size);

        // begin parsing at the beginning of the string
        size_t position = query.find_first_of("?!");

        // add the first part of the query (before the first placeholder)
        result.append(query, 0, position);

        // process all parameters
        for (size_t i = 0; i < count; ++i)
        {
            // are there more parameters to be escaped?
            if (position == std::string::npos) break;

            // retrieve parameter
            auto &parameter = parameters[i];

            // should we just escape or also quote
            switch (query[position])
            {
                case '?':
                    // we need to escape and quote
                    result.append(parameter.quote(_connection));
                    break;
                case '!':
                    // we only need to escape
                    result.append(parameter.escape(_connection));
                    break;
            }

            // find the next placeholder
            size_t next = query.find_first_of("?!", position + 1);

            // add the regular query part and store new position
            result.append(query, position + 1, next - position - 1);
            position = next;
        }

        // clean up the parameters
        delete [] parameters;

        // and inform the callback
        _master.execute([callback, result]() { callback(result); });
    });
}

/**
 *  Execute a query
 *
 *  @param  query       the query to execute
 */
Deferred& Connection::query(const std::string& query)
{
    // create a new deferred handler
    auto deferred = std::make_shared<Deferred>();

    // execute query in the worker thread
    _worker.execute([this, query, deferred]() {
        // run the query, should get zero on success
        if (mysql_query(_connection, query.c_str()))
        {
            // query failed, report to listener
            if (deferred->requireStatus()) _master.execute([this, deferred]() { deferred->failure(mysql_error(_connection)); });
            return;
        }

        // process all result sets
        while (true)
        {
            // retrieve result set
            auto *result = mysql_store_result(_connection);

            // are we at all interested in the result?
            if (!deferred->requireStatus())
            {
                // clean up the result
                if (result) mysql_free_result(result);
            }
            else
            {
                // get the number of rows affected in the query
                size_t affectedRows = mysql_affected_rows(_connection);

                // did we get a valid response?
                if (result)
                {
                    // create the result and pass it to the listener
                    _master.execute([this, deferred, result]() { deferred->success(Result(result)); });
                }
                else if (mysql_field_count(_connection))
                {
                    // the query *should* have returned a result, this is an error
                    _master.execute([this, deferred]() { deferred->failure(mysql_error(_connection)); });
                }
                else
                {
                    // this is a query without a result set (i.e.: update, insert or delete)
                    _master.execute([this, deferred, affectedRows]() { deferred->success(Result(affectedRows)); });
                }
            }

            // check whether there are more results
            switch(mysql_next_result(_connection))
            {
                case -1:
                    // all result sets were processed
                    return;
                case 0:
                    // ready for next result
                    continue;
                default:
                    // this is an error
                    _master.execute([this, deferred]() { deferred->failure(mysql_error(_connection)); });
                    return;
            }
        }
    });

    // return the deferred handler
    return *deferred;
}

/**
 *  End namespace
 */
}}
