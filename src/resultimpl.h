/**
 *  ResultImpl.cpp
 *
 *  A wrapper for the mysql result that includes
 *  name -> field wrapping
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Result wrapper class
 */
class ResultImpl
{
private:
    /**
     *  MySQL result
     */
    MYSQL_RES *_result;

    /**
     *  Field info
     */
    std::map<std::string, size_t> _fields;

    /**
     *  The current position in the result set
     */
    size_t _position;

    /**
     *  The number of rows
     */
    size_t _size;

public:
    /**
     *  Construct result implementation
     *
     *  @param  result  mysql result
     */
    ResultImpl(MYSQL_RES *result) :
        _result(result),
        _position(0)
    {
        // retrieve number of fields
        auto size = mysql_num_fields(_result);

        // initialize all fields
        for (size_t i = 0; i < size; i++)
        {
            // retrieve field info
            auto field = mysql_fetch_field_direct(_result, i);

            // store field in map
            _fields[std::string(field->name, field->name_length)] = i;
        }

        // the number of rows
        _size = mysql_num_rows(_result);
    }

    /**
     *  Clean up the result
     */
    virtual ~ResultImpl()
    {
        // clean up result
        mysql_free_result(_result);
    }

    /**
     *  Get the fields and their index
     */
    const std::map<std::string, size_t>& fields() const
    {
        return _fields;
    }

    /**
     *  Get the number of rows in this result set
     */
    size_t size() const
    {
        return _size;
    }

    /**
     *  Seek to a specific row
     *
     *  This function will throw an exception if no
     *  row exists at the given index (i.e.: index
     *  is greater than size()) or if the object is
     *  in an invalid state.
     *
     *  @param  offset
     */
    void seek(size_t index)
    {
        // check whether the index is valid
        if (index > size()) throw Exception("Invalid result offset");

        // are we already at the required offset?
        if (_position == index) return;

        /**
         *  TODO:   Optimize this for bigger result sets.
         *          We could, for example, store the result
         *          of mysql_row_tell every 1000 rows, so
         *          we never have to travel through thousands
         *          of rows to get where we want to.
         */

        // remember the new position
        _position = index;

        // seek to the desired offset
        mysql_data_seek(_result, index);
    }

    /**
     *  Retrieve row at current index and advance to next row
     */
    MYSQL_ROW fetch()
    {
        // we just moved to the next row
        _position++;

        // return the next row
        return mysql_fetch_row(_result);
    }
};

/**
 *  End namespace
 */
}}
