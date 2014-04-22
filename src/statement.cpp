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
 *  @param  callback    the callback to inform of success or failure
 */
Statement::Statement(Connection *connection, const std::string& statement) :
    _connection(connection),
    _statement(nullptr),
    _parameters(0),
    _info(nullptr)
{
    // prepare statement in worker thread
    _connection->_worker.execute([this, statement]() {
        // initialize statement
        if ((_statement = mysql_stmt_init(_connection->_connection)) == nullptr)
        {
            if (_prepareCallback) _connection->_master.execute([this]() { _prepareCallback("Unable to initialize statement"); });
            return;
        }

        // prepare statement
        if (mysql_stmt_prepare(_statement, statement.c_str(), statement.size()))
        {
            _connection->_master.execute([this]() {
                // inform callback of problem
                if (_prepareCallback) _prepareCallback(mysql_stmt_error(_statement));

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
        if (_prepareCallback) _connection->_master.execute([this] () { _prepareCallback(nullptr); });
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
Deferred& Statement::execute(Parameter *parameters, size_t count)
{
    // create the deferred handler
    auto deferred = std::make_shared<Deferred>();

    // execute statement in worker thread
    _connection->_worker.execute([this, parameters, count, deferred]() {
        // check for a valid statement
        if (_statement == nullptr)
        {
            _connection->_master.execute([deferred]() { deferred->failure("Cannot execute invalid statement"); });
            delete [] parameters;
            return;
        }

        // check for correct number of arguments and bind the parameters
        if (count != _parameters)
        {
            _connection->_master.execute([deferred]() { deferred->failure("Incorrect number of arguments"); });
            delete [] parameters;
            return;
        }

        // bind the parameters and execute the statement
        if (mysql_stmt_bind_param(_statement, parameters) || mysql_stmt_execute(_statement))
        {
            _connection->_master.execute([this, deferred]() { deferred->failure(mysql_stmt_error(_statement)); });
            delete [] parameters;
            return;
        }

        // clean up input parameters
        delete [] parameters;

        // anyone interested in the result?
        if (!deferred->requireStatus())
        {
            deferred->complete();
            return;
        }

        // if the query has no result set, we create the result with the affected rows
        if (_info == nullptr) _connection->_master.execute([this, deferred]() {
            // the statement already knows the number of affected rows
            // so doing this on the master will not block at all
            deferred->success(Result(mysql_stmt_affected_rows(_statement)));
        });
        else
        {
            // retrieve the result
            auto rows = _info->rows();

            // send the result to the callback
           _connection->_master.execute([deferred, rows]() { deferred->success(Result(std::move(rows))); });
        }
    });

    // return the deferred handler
    return *deferred;
}

/**
 *  End namespace
 */
}}
