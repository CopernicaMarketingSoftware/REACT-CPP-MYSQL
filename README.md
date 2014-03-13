REACT-CPP-MYSQL
===============

C++ asynchronous mysql library on top of the REACT-CPP library. Uses lambdas and callbacks to return query results.

REACT-CPP-MYSQL is a MySQL library using REACT-CPP to manage an asynchronous connection to MySQL.

Connection class
================

The React::MySQL::Connection class is used to establish a connection to a MySQL daemon.

```c++
#include <reactcpp/mysql.h>
#include <iostream>
#include <cassert>

/**
 *  Main application procedure
 */
int main()
{
    // create an event loop
    React::MainLoop loop;

    // create the connection to MySQL
    React::MySQL::Connection connection(&loop, "mysql.example.org", "example user", "example password", "example database", [](React::MySQL::Connection *connection, const char *error) {
        /**
         *  This function will be called upon success/failure of the connection
         *
         *  The first parameter is always the connection object. The second,
         *  error parameter will be set to NULL upon success, and set to a
         *  c-string otherwise.
         */
        if (error) std::cout << "Failed to connect: " << error << std::endl;
        else std::cout << "Connected" << std::endl;
    });

    /**
     *  At this point, the connection is still being established. It is,
     *  however, perfectly valid to start sending queries to MySQL.
     *
     *  The queries will be executed after the connection has been established.
     *  If the connection somehow fails, all the query callbacks will be
     *  called with an error to indicate this condition
     */
    connection.query("SELECT * FROM test LIMIT 10", [](React::MySQL::Result&& result, const char *error) {
        /**
         *  As with the connection callback, the error parameter
         *  will be set to to NULL upon success, and set to a
         *  c-string otherwise.
         *
         *  In case of an error, an invalid result object will
         *  be provided, that has zero rows and no affected rows.
         */
        if (error)
        {
            std::cout << "Query error: " << error << std::endl;
            exit(1);
        }

        // this is a normal select query, affectedRows should be zero
        assert(result.affectedRows() == 0);

        // dump all rows to screen
        for (auto row : result)
        {
            // wrap each row in curly braces
            std::cout << "{" << std::endl;

            // and dump all fields
            for (auto field : row)
            {
                // field is an std::pair with as first member the name and the second member the field value
                std::cout << "  " << field.first << " => " << field.second << std::endl;
            }

            // close the row
            std::cout << "}" << std::endl;
        }

        // stop the application
        exit(0);
    });

    // run the event loop
    loop.run();

    // done
    return 0;
}
```

Both the result set and the individual rows can be iterated (as shown in the example above) or simply used
as an array. The rows can only be accessed with a numerical index, while the fields can be accessed using
their field index as well as their name:

```c++
connection.query("SELECT a, b, c FROM test", [](React::MySQL::Result&& result, const char *error) {
    // assume error handling has been done

    // loop over the rows by their index
    for (size_t i = 0; i < result.size(); ++i)
    {
        // retrieve the row
        auto row = result[i];

        // the individual fields can be accessed by name or by index
        // e.g.: the field 'b', can be accessed by index 1 or 'b':
        auto field = row[1];
        auto sameField = row["b"];

        // fields can be casted to a string, or any of the numeric types
        // assuming that 'a' holds a unsigned bigint
        unsigned long a = row["a"];
    }

    /**
     *  It is, of course, also possible to take individual rows
     *  from the result set in any order.
     *
     *  Note that this can be extremely slow when working with
     *  larger result sets. Sequential access is the fastest
     *  way to step through the results.
     */
    auto row = result[17];
});
```

For convenience, all functions can be called without their corresponding callback. This could be useful
for non-critical operations, like truncating a log table:

```c
#include <reactcpp/mysql.h>
#include <iostream>
#include <cassert>

/**
 *  Main application procedure
 */
int main()
{
    // create an event loop
    React::MainLoop loop;

    // create the connection to MySQL and clean out the logs
    React::MySQL::Connection connection(&loop, "mysql.example.org", "example user", "example password", "example database");
    connection.query("TRUNCATE logs");

    // run the event loop
    loop.run();

    // done
    return 0;
```

Prepared Statements
===================

When executing a particular query multiple times, or when dealing with unsanitized input (e.g. user input),
the fastest, and most secure, way is to use prepared statements.

When using prepared statements the query is only sent to the MySQL daemon once and the input and output to
and from the query is done using a binary format. This way the query will only have to be parsed once (the
execution plan is cached on the server) and saves on network overhead. It also completely eliminates the
possibility of SQL injection.

```c
#include <reactcpp/mysql.h>
#include <iostream>
#include <cassert>

/**
 *  Min application procedure
 */
int main()
{
    // create an event loop
    React::MainLoop loop;

    // create the connection to MySQL and clean out the logs
    React::MySQL::Connection connection(&loop, "mysql.example.org", "example user", "example password", "example database");

    // create the statement
    React::MySQL::Statement statement(&connection, "INSERT INTO logs (event_time, description) VALUES (NOW(), ?), (NOW(), ?), (NOW(), ?)", [](React::MySQL::Statement *statement, const char *error) {
        /**
         *  As with the connection class and query function
         *  the error parameter will be a NULL pointer if the
         *  statement was correctly initialized.
         */
        if (error)
        {
            std::cout << "Cannot initialize statement: " << error << std::endl;
            exit(0);
        }
    });

    /**
     *  When we get here, the statement is not yet initialized
     *  but we can already start executing it. As with the connection
     *  class, it will start executing once it is initialized.
     *
     *  The execute method takes as many arguments as there are
     *  placeholders in the query. The statement was initialized
     *  with a query containing three placeholders, so we have to
     *  provide three values.
     *
     *  The values should always be of a similar type as the type
     *  used in the table. For example, a MySQL column defined as
     *  'unsigned int' could receive a uint32_t, while a varchar
     *  column could receive a const char* or std::string.
     *
     *  This is not a hard requirement, if you send a number where
     *  the column holds a varchar, conversion will take place on
     *  the server (causing a little extra overhead). The other way
     *  around also works, but only if the string actually holds
     *  numeric data.
     */
    statement.execute("first event description", "second event description", "third event description");

    /**
     *  We can also select data with prepared statements.
     *
     *  Just as with other functions and classes, we simply
     *  provide a callback function that will receive the data.
     *  However, unlike everywhere else, the execute method
     *  takes the callback as the first parameter.
     */
    React::MySQL::Statement selectStatement(&connection, "SELECT a, b, c FROM test WHERE a BETWEEN ? and ?");
    selectStatement.execute([](React::MySQL::Result&& result, const char *error) {
        // assume proper error handling was done

        // this is a normal select query, affectedRows should be zero
        assert(result.affectedRows() == 0);

        // dump all rows to screen
        for (auto row : result)
        {
            // wrap each row in curly braces
            std::cout << "{" << std::endl;

            // and dump all fields
            for (auto field : row)
            {
                // field is an std::pair with as first member the name and the second member the field value
                std::cout << "  " << field.first << " => " << field.second << std::endl;
            }

            // close the row
            std::cout << "}" << std::endl;
        }
    }, 5, 10);
}
