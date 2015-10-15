/**
 *  Statement.h
 *
 *  Class for executing prepared statements
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

// forward declaration
class StatementResultInfo;

/**
 *  Prepared statement class
 */
class Statement
{
private:
    /**
     *  The MySQL connection
     */
    Connection *_connection;

    /**
     *  The prepared statement
     */
    MYSQL_STMT *_statement;

    /**
     *  The query the statement is based on
     */
    std::string _query;

    /**
     *  Callback to execute when the statement was prepared
     */
    std::function<void(const char *error)> _prepareCallback;

    /**
     *  The number of parameters in this statement
     */
    size_t _parameters;

    /**
     *  Information about the query result fields
     */
    std::unique_ptr<StatementResultInfo> _info;

    /**
     *  Initialize the statement
     *
     *  @note:  This function is to be executed from
     *          worker context only
     *
     *  @param  reference   The loop reference to stay alive
     */
    void initialize(const std::shared_ptr<React::LoopReference> &reference);

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
    void execute(Parameter *parameters, size_t count, const std::shared_ptr<React::LoopReference> &reference, const std::shared_ptr<Deferred> &deferred);
public:
    /**
     *  Constructor
     *
     *  @param  connection  the connection to run the statement on
     *  @param  statement   the statement to execute
     */
    Statement(Connection *connection, std::string statement);

    /**
     *  Copy constructor
     */
    Statement(const Statement& that) = delete;

    /**
     *  Move constructor
     */
    Statement(Statement&& that);

    /**
     *  Destructor
     */
    virtual ~Statement();

    /**
     *  Get a call when the statement is prepared or fails
     *
     *  @param  callback    the callback to execute
     */
    void onPrepared(const std::function<void(const char *error)>& callback)
    {
        _prepareCallback = callback;
    }

    /**
     *  Execute the statement
     *
     *  Note:   the number of arguments (excluding the callback) must match
     *          the number of placeholders in the statement string. If not,
     *          the statement will not be executed and the callback will be
     *          invoked immediately with an error.
     *
     *  The following parameter types are supported and map to the following MySQL types
     *
     *  Variable type       MySQL column type
     *  -------------       -----------------
     *  signed char         TINYINT
     *  short int           SMALLINT
     *  int                 INT
     *  long long int       BIGINT
     *  float               FLOAT
     *  double              DOUBLE
     *  std::string         TEXT, CHAR or VARCHAR
     *  std::vector<char>   BLOB, BINARY or VARBINARY
     *  std::nullptr_t      NULL
     *
     *  @param  mixed...    variable number of arguments of different type
     *  @param  callback    the callback to be informed when the statement is executed or failed
     */
    template <class ...Arguments>
    Deferred& execute(Arguments ...params)
    {
        // create the deferred handler
        auto deferred = std::make_shared<Deferred>();

        // keep the loop alive while the callback runs
        auto reference = std::make_shared<React::LoopReference>(_connection->_loop);

        // allocate the parameters
        auto count = sizeof...(params);
        auto *parameters = new Parameter[sizeof...(params)]{ params... };

        // execute statement in worker thread
        _connection->_worker.execute([this, reference, parameters, count, deferred]() { execute(parameters, count, reference, deferred); });

        // return the deferred handler
        return *deferred;
    }
};

/**
 *  End namespace
 */
}}
