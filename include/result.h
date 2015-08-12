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

    /**
     *  Number of affected rows
     */
    size_t _affectedRows = 0;

    /**
     *  Last insert id, only relevant
     *  if this is the result of a query
     *  that inserted into a table with
     *  an auto_increment column
     */
    uint64_t _insertID = 0;

public:
    /**
     *  Result iterator
     */
    class iterator
    {
    private:
        /**
         *  The result from MySQL
         */
        std::shared_ptr<ResultImpl> _result;

        /**
         *  The index we are at
         */
        size_t _index;

        /**
         *  Is this iterator pointing to a valid position?
         */
        bool valid() const;
    public:
        /**
         *  Empty constructor
         */
        iterator();

        /**
         *  Constructor
         *
         *  @param  result  mysql result set
         *  @param  index   index to start from
         */
        iterator(std::shared_ptr<ResultImpl> result, size_t index);

        /**
         *  Copy constructor
         *
         *  @param  that    iterator to copy
         */
        iterator(const iterator& that);

        /**
         *  Assign another iterator
         */
        iterator& operator=(const iterator& that);

        /**
         *  Increment operator
         */
        iterator& operator++();

        /**
         *  Increment operator (postfix)
         */
        iterator operator++(int);

        /**
         *  Compare with other iterator
         *
         *  @param  that    iterator to compare with
         */
        bool operator==(const iterator& that);

        /**
         *  Compare with otehr iterator
         *
         *  @param  that    iterator to compare with
         */
        bool operator!=(const iterator& that);

        /**
         *  Dereference
         */
        ResultRow operator*();

        /**
         *  Call method on dereferenced row
         */
        std::unique_ptr<ResultRow> operator->();
    };

    /**
     *  Constructor
     */
    Result(MYSQL_RES *result);

    /**
     *  Constructor
     */
    Result(std::shared_ptr<ResultImpl>&& implementation);

    /**
     *  Constructor for affected rows
     *
     *  @param  affectedRows    Number of rows affected
     *  @param  insertID        ID for the inserted row, if applicable
     */
    Result(size_t affectedRows, uint64_t insertID);

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
    virtual ~Result();

    /**
     *  Is this a valid result?
     */
    bool valid() const;

    /**
     *  The number of rows affected
     */
    size_t affectedRows() const;

    /**
     *  ID of the last inserted row, but only
     *  if the insert was using an auto_increment
     */
    uint64_t insertID() const;

    /**
     *  Get the number of rows in this result
     */
    size_t size() const;

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

    /**
     *  Retrieve iterator for first row
     */
    iterator begin() const;

    /**
     *  Retrieve iterator past the end
     */
    iterator end() const;
};

/**
 *  End namespace
 */
}}
