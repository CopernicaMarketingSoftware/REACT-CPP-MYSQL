/**
 *  CachedStatement.h
 *
 *  This class is a cached statement that is cached
 *  for the connection. It will not clean up server
 *  resources when it falls out of scopes.
 *
 *  It assumes that a const char pointer with the
 *  same address points to the exact same statement
 *  (i.e. it will not do a string comparison to see
 *  whether it is really the same statement). it is
 *  therefor not advised to use cached statements in
 *  combination with dynamically allocated strings.
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Cached statement class
 */
class CachedStatement
{
private:
    /**
     *  The underlying statement from the connection
     */
    Statement *_statement;
public:
    /**
     *  Constructor
     *
     *  @param  connection  the connection to run the statement on
     *  @param  statement   the statement to execute
     */
    CachedStatement(Connection *connection, const char *statement) :
        _statement(connection->statement(statement))
    {}

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
        _statement->execute(callback, std::forward<Arguments>(parameters)...);
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
        _statement->execute(std::forward<Arguments>(parameters)...);
    }
};

/**
 *  End namespace
 */
}}
