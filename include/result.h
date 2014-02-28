/**
 *  Result.h
 *
 *  Class representing a MySQL result set
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

// forward declaration
class ResultImpl;

/**
 *  Result class
 */
class Result
{
    /**
     *  The result from MySQL
     */
    std::shared_ptr<ResultImpl> _result;
public:
    /**
     *  Constructor
     */
    Result(MYSQL_RES *result);

    /**
     *  Invalid constructor
     */
    Result(std::nullptr_t result);

    /**
     *  Move constructor
     */
    Result(Result&& that);

    /**
     *  Results may never be copied
     */
    Result(const Result& that) = delete;

    /**
     *  Destructor
     */
    ~Result();

    /**
     *  Is this a valid result?
     */
    bool valid();

    /**
     *  Get the number of rows in this result
     */
    size_t size();

    /**
     *  Retrieve a row at the given offset
     *
     *  This function will throw an exception if no
     *  row exists at the given index (i.e.: index
     *  is greater than size()).
     *
     *  @param  index   row index in the result
     *  @throws Exception
     */
    ResultRow operator [] (size_t index);
};

/**
 *  End namespace
 */
}}
