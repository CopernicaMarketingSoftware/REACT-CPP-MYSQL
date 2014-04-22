/**
 *  Connection.h
 *
 *  Class representing a connection to a MySQL or MariaDB daemon
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

// forward declaration
class Statement;

/**
 *  Connection class
 */
class Connection
{
private:
    /**
     *  Connection to mysql
     */
    MYSQL *_connection;

    /**
     *  Callback to execute once the connection is established
     */
    std::function<void(const char *error)> _connectCallback;

    /**
     *  Did we encounter a reconnect?
     */
    my_bool _reconnected;

    /**
     *  The worker operating on MySQL
     */
    Worker _worker;

    /**
     *  Worker for main thread
     */
    Worker _master;

    /**
     *  Cached prepared statements
     */
    std::map<const char *, Statement*> _statements;

    /**
     *  Retrieve or create a cached prepared statement
     *
     *  @param  query   the query to use for preparing the statement
     */
    Statement *statement(const char *query);

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
    void prepare(const std::string& query, LocalParameter *parameters, size_t count, const std::function<void(const std::string& query)>& callback);
public:
    /**
     *  Establish a connection to mysql
     *
     *  @param  loop        the loop to bind to
     *  @param  hostname    the hostname to connect to
     *  @param  username    the username to login with
     *  @param  password    the password to authenticate with
     *  @param  database    the database to use
     *  @param  flags       connection flags
     */
    Connection(Loop *loop, const std::string& hostname, const std::string &username, const std::string& password, const std::string& database, uint64_t flags = CLIENT_IGNORE_SIGPIPE | CLIENT_MULTI_STATEMENTS);

    /**
     *  Destructor
     */
    virtual ~Connection();

    /**
     *  Get a call when the connection succeeds or fails
     *
     *  @param  callback    the callback that will be informed of the connection status
     */
    void onConnected(const std::function<void(const char *error)>& callback);

    /**
     *  Execute a query
     *
     *  @param  query       the query to execute
     */
    Deferred& query(const std::string& query);

    /**
     *  Execute a query with placeholders
     *
     *  This works sort-of like a prepared statement, the difference being that the
     *  query is prepared completely at the client side. This has the advantage of
     *  allowing placeholders anywhere (including identifiers) as opposed to only
     *  in WHERE, ON and LIMIT clauses.
     *
     *  The query is sent as a regular query, which means it won't use the binary
     *  protocol. This comes with some extra network overhead. The query is also
     *  not cached on the server, as is done with prepared statements.
     *
     *  The following placeholders are supported:
     *
     *  ?   escape string data and quote when necessary
     *  !   escape string data but do not quote
     *
     *  @param  query       the query to execute
     *  @param  mixed...    placeholder values
     */
    template <class ...Arguments>
    Deferred& execute(const std::string& query, Arguments ...parameters)
    {
        // if there are no parameters, we don't need to parse anything
        if (sizeof...(parameters) == 0) return this->query(query);

        // create the deferred handler
        auto deferred = std::make_shared<Deferred>();

        // prepare the query, then execute it
        prepare(query, new LocalParameter[sizeof...(parameters)]{ parameters... }, sizeof...(parameters), [this, deferred](const std::string& query) {
            // execute query
            auto &result = this->query(query);

            // pass on the completed event
            result.onComplete([deferred]() { deferred->complete(); });

            // only register success and failure if necessary
            if (deferred->requireStatus())
            {
                result.onSuccess([deferred](Result&& result)   { deferred->success(std::move(result)); });
                result.onFailure([deferred](const char *error) { deferred->failure(error); });
            }
        });

        // return the deferred handler
        return *deferred;
    }

    /**
     *  Friends and family
     */
    friend class Statement;
    friend class CachedStatement;
};

/**
 *  End namespace
 */
}}
