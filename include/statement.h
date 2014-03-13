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
     *  The number of parameters in this statement
     */
    size_t _parameters;

    /**
     *  Information about the query result fields
     */
    StatementResultInfo *_info;

    /**
     *  Execute statement with given parameters
     *
     *  @param  parameters  input parameters
     *  @param  count       number of parameters
     */
    void execute(Parameter *parameters, size_t count);

    /**
     *  Execute statement with given parameters
     *
     *  @param  parameters  input parameters
     *  @param  count       number of parameters
     *  @param  callback    callback to inform of success or failure
     */
    void execute(std::function<void(Result&& result, const char *error)>&& callback, Parameter *parameters, size_t count);
public:
    /**
     *  Constructor
     *
     *  @param  connection  the connection to run the statement on
     *  @param  statement   the statement to execute
     *  @param  callback    the callback to inform of success or failure
     */
    Statement(Connection *connection, const std::string& statement, const std::function<void(Statement *statement, const char *error)>& callback);

    /**
     *  Constructor
     *
     *  @param  connection  the connection to run the statement on
     *  @param  statement   the statement to execute
     */
    Statement(Connection *connection, const std::string& statement);

    /**
     *  Copy constructor
     */
    Statement(const Statement& that) = delete;

    /**
     *  Move constructor
     */
    Statement(Statement&& that) :
        _connection(that._connection),
        _statement(that._statement),
        _parameters(that._parameters),
        _info(that._info)
    {
        // reset other statement
        that._connection = nullptr;
        that._statement = nullptr;
        that._parameters = 0;
        that._info = nullptr;
    }

    /**
     *  Destructor
     */
    virtual ~Statement();

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
     *  @param  callback    the callback to be informed when the statement is executed or failed
     *  @param  mixed...    variable number of arguments of different type
     */
    template <class Callback, class ...Arguments>
    typename std::enable_if<std::is_constructible<std::function<void(Result&&, const char*)>, Callback>::value, void>::type
    execute(const Callback& callback, Arguments ...parameters)
    {
        // pass to implementation
        execute(std::function<void(Result&&, const char*)>(callback), new Parameter[sizeof...(parameters)]{ parameters... }, sizeof...(parameters));
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
    void execute(Arguments ...parameters)
    {
        // pass to implementation
        execute(new Parameter[sizeof...(parameters)]{ parameters... }, sizeof...(parameters));
    }
};

/**
 *  End namespace
 */
}}
