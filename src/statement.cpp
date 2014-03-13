/**
 *  Statement.cpp
 *
 *  Class for executing prepared statements
 *
 *  @copyright 2014 Copernica BV
 */

#include "includes.h"

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Constructor
 *
 *  @param  connection  the connection to run the statement on
 *  @param  statement   the statement to execute
 */
Statement::Statement(Connection *connection, const std::string& statement, const std::function<void(Statement *statement, const char *error)>& callback) :
    _connection(connection),
    _statement(nullptr),
    _parameters(0),
    _info(nullptr)
{
    // prepare statement in worker thread
    _connection->_worker.execute([this, statement, callback]() {
        // initialize statement
        if ((_statement = mysql_stmt_init(_connection->_connection)) == nullptr)
        {
            _connection->_master.execute([this, callback]() { callback(this, "Unable to initialize statement"); });
            return;
        }

        // prepare statement
        if (mysql_stmt_prepare(_statement, statement.c_str(), statement.size()))
        {
            _connection->_master.execute([this, callback]() {
                // inform callback of problem
                callback(this, mysql_stmt_error(_statement));

                // clean up the statement
                mysql_stmt_close(_statement);
                _statement = nullptr;
            });
            return;
        }

        // store number of parameters in statement
        _parameters = mysql_stmt_param_count(_statement);

        // retrieve the list of fields for the statement result set (if any)
        auto *result = mysql_stmt_result_metadata(_statement);

        // if the statement does return fields, store information about it
        if (result != nullptr) _info = new StatementResultInfo(_statement, result);

        // all is well, inform the callback
        _connection->_master.execute([this, callback] () { callback(this, nullptr); });
    });
}

/**
 *  Destructor
 */
Statement::~Statement()
{
    // clean up the statement
    if (_statement != nullptr) mysql_stmt_close(_statement);

    // clean up the result information
    delete _info;
}

/**
 *  Execute statement with given parameters
 *
 *  @param  parameters  input parameters
 *  @param  count       number of parameters
 */
void Statement::execute(Parameter *parameters, size_t count)
{
    // execute statement in worker thread
    _connection->_worker.execute([this, parameters, count]() {
        // check for valid statement, correct number of arguments and bind the parameters
        if (_statement == nullptr || count != _parameters || mysql_stmt_bind_param(_statement, parameters))
        {
            delete [] parameters;
            return;
        }

        // execute the statement, we don't care about the result
        mysql_stmt_execute(_statement);

        // clean up input parameters
        delete [] parameters;
    });
}

/**
 *  Execute statement with given parameters
 *
 *  @param  parameters  input parameters
 *  @param  count       number of parameters
 *  @param  callback    callback to inform of success or failure
 */
void Statement::execute(std::function<void(Result&& result, const char *error)>&& callback, Parameter *parameters, size_t count)
{
    // execute statement in worker thread
    _connection->_worker.execute([this, parameters, count, callback]() {
        // check for a valid statement
        if (_statement == nullptr)
        {
            _connection->_master.execute([callback]() { callback(Result(nullptr), "Cannot execute invalid statement"); });
            delete [] parameters;
            return;
        }

        // check for correct number of arguments and bind the parameters
        if (count != _parameters)
        {
            _connection->_master.execute([callback]() { callback(Result(nullptr), "Incorrect number of arguments"); });
            delete [] parameters;
            return;
        }

        // bind the parameters and execute the statement
        if (mysql_stmt_bind_param(_statement, parameters) || mysql_stmt_execute(_statement))
        {
            _connection->_master.execute([this, callback]() { callback(Result(nullptr), mysql_stmt_error(_statement)); });
            delete [] parameters;
            return;
        }

        // clean up input parameters
        delete [] parameters;

        // if the query has no result set, we create the result with the affected rows
        if (_info == nullptr) _connection->_master.execute([this, callback]() {
            // the statement already knows the number of affected rows
            // so doing this on the master will not block at all
            callback(Result(mysql_stmt_affected_rows(_statement)), nullptr);
        });
        else
        {
            // retrieve the result
            auto rows = _info->rows();

            // send the result to the callback
           _connection->_master.execute([callback, rows]() { callback(Result(std::move(rows)), nullptr); });
        }

    });
}

/**
 *  End namespace
 */
}}
