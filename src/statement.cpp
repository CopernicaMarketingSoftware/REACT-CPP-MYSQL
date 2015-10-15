/**
 *  Statement.cpp
 *
 *  Class for executing prepared statements
 *
 *  @copyright 2014 Copernica BV
 */

#include "includes.h"
#include "mysql/errmsg.h"

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
Statement::Statement(Connection *connection, std::string statement) :
    _connection(connection),
    _statement(nullptr),
    _query(std::move(statement)),
    _parameters(0)
{
    // keep the loop alive while the callback runs
    auto reference = std::make_shared<React::LoopReference>(_connection->_loop);

    // initialize statement in worker thread
    _connection->_worker.execute([this, reference]() { initialize(reference); });
}

/**
 *  Move constructor
 */
Statement::Statement(Statement&& that) :
    _connection(that._connection),
    _statement(that._statement),
    _parameters(that._parameters),
    _info(std::move(that._info))
{
    // reset other statement
    that._connection = nullptr;
    that._statement = nullptr;
    that._parameters = 0;
}

/**
 *  Destructor
 */
Statement::~Statement()
{
    // clean up the statement
    if (_statement != nullptr) mysql_stmt_close(_statement);
}

/**
 *  Initialize the statement
 *
 *  @note:  This function is to be executed from
 *          worker context only
 *
 *  @param  reference   The loop reference to stay alive
 */
void Statement::initialize(const std::shared_ptr<React::LoopReference> &reference)
{
    // initialize statement
    if ((_statement = mysql_stmt_init(_connection->_connection)) == nullptr)
    {
        _connection->_master.execute([this, reference]() { if (_prepareCallback) _prepareCallback("Unable to initialize statement"); });
        return;
    }

    // prepare statement
    if (mysql_stmt_prepare(_statement, _query.c_str(), _query.size()))
    {
        _connection->_master.execute([this, reference]() {
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
    if (result != nullptr) _info.reset(new StatementResultInfo(_statement, result));

    // all is well, inform the callback
    _connection->_master.execute([this, reference] () { if (_prepareCallback) _prepareCallback(nullptr); });
}

/**
 *  Execute statement with given parameters
 *
 *  @note:  This function is to be executed from
 *          worker context only
 *
 *  @param  parameters  The parameters to retry with
 *  @param  count       The number of parameters
 *  @param  reference   The loop reference
 *  @param  deferred    The previously created deferred handler
 */
void Statement::execute(Parameter *parameters, size_t count, const std::shared_ptr<React::LoopReference> &reference, const std::shared_ptr<Deferred> &deferred)
{
    // check for a valid statement
    if (_statement == nullptr)
    {
        _connection->_master.execute([reference, deferred]() { deferred->failure("Cannot execute invalid statement"); });
        delete [] parameters;
        return;
    }

    // check for correct number of arguments and bind the parameters
    if (count != _parameters)
    {
        _connection->_master.execute([reference, deferred]() { deferred->failure("Incorrect number of arguments"); });
        delete [] parameters;
        return;
    }

    // bind the parameters
    if (mysql_stmt_bind_param(_statement, parameters))
    {
        _connection->_master.execute([this, reference, deferred]() { deferred->failure(mysql_stmt_error(_statement)); });
        delete [] parameters;
        return;
    }

    // execute the statement
    if (mysql_stmt_execute(_statement))
    {
        // check if the connection was reset, in which case we will
        // have to completely re-initialize the statement :(
        if (mysql_stmt_errno(_statement) == CR_SERVER_LOST)
        {
            // the statement is now invalid
            // the client code cleans it up
            _statement = nullptr;

            // reset parameter count
            _parameters = 0;

            // clean up the info
            _info.reset();

            // initialize the statement again
            initialize(reference);

            // and retry execution again
            execute(parameters, count, reference, deferred);
        }
        else
        {
            // an error occured that we can't recover from
            _connection->_master.execute([this, reference, deferred]() { deferred->failure(mysql_stmt_error(_statement)); });
            delete [] parameters;
        }

        // an error occured, don't proceed
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
    if (!_info) _connection->_master.execute([this, reference, deferred]() {
        // the statement already knows the number of affected rows
        // so doing this on the master will not block at all
        deferred->success(Result(mysql_stmt_affected_rows(_statement), mysql_stmt_insert_id(_statement)));
    });
    else
    {
        // retrieve the result
        auto rows = _info->rows();

        // send the result to the callback
       _connection->_master.execute([reference, deferred, rows]() { deferred->success(Result(std::move(rows))); });
    }
}

/**
 *  End namespace
 */
}}
