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
     *  The worker operating on MySQL
     */
    Worker _worker;

    /**
     *  Worker for main thread
     */
    Worker _master;
public:
    /**
     *  Establish a connection to mysql
     *
     *  @param  loop        the loop to bind to
     *  @param  hostname    the hostname to connect to
     *  @param  username    the username to login with
     *  @param  password    the password to authenticate with
     *  @param  database    the database to use
     */
    Connection(Loop *loop, const std::string& hostname, const std::string &username, const std::string& password, const std::string& database);

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
    Connection(Loop *loop, const std::string& hostname, const std::string &username, const std::string& password, const std::string& database, const std::function<void(Connection *connection, const char *error)>& callback);

    /**
     *  Destructor
     */
    virtual ~Connection();
};

/**
 *  End namespace
 */
}}
