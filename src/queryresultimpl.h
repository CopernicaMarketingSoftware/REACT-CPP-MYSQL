/**
 *  QueryResultImpl.h
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
class QueryResultImpl : public ResultImpl
{
private:
    /**
     *  MySQL result
     */
    MYSQL_RES *_result;

    /**
     *  Already parsed rows
     */
    std::vector<std::vector<std::unique_ptr<ResultFieldImpl>>> _rows;

    /**
     *  Field info
     */
    std::map<std::string, size_t> _fields;

    /**
     *  The current position in the result set
     */
    size_t _position;

public:
    /**
     *  Construct result implementation
     *
     *  @param  result  mysql result
     */
    QueryResultImpl(MYSQL_RES *result) :
        ResultImpl(),
        _result(result),
        _rows(mysql_num_rows(_result)),
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
    }

    /**
     *  Clean up the result
     */
    virtual ~QueryResultImpl()
    {
        // clean up result
        mysql_free_result(_result);
    }

    /**
     *  Get the fields and their index
     */
    const std::map<std::string, size_t>& fields() const override
    {
        return _fields;
    }

    /**
     *  Get the number of rows in this result set
     */
    size_t size() const override
    {
        return _rows.size();
    }

    /**
     *  Retrieve row at the given index
     */
    const std::vector<std::unique_ptr<ResultFieldImpl>>& fetch(size_t index) override
    {
        // check whether the index is valid
        if (index > size()) throw Exception("Invalid result offset");

        // did we already prepare this row?
        if (_rows[index].size() == 0)
        {
            // are we already at the required offset?
            if (_position != index)
            {
                // seek to the desired offset
                mysql_data_seek(_result, index);
            }

            // store current position
            // we add one to the position since
            // the fetch operation advances to
            // the next row (which happens below)
            _position = index + 1;

            // retrieve the row
            auto row = mysql_fetch_row(_result);

            // reserve space for the rows
            _rows[index].reserve(_fields.size());

            // and parse them
            for (size_t i = 0; i < _fields.size(); ++i)
            {
                // create field implementation
                _rows[index].emplace_back(new QueryResultField(row[i]));
            }
        }

        // return the cached row
        return _rows[index];
    }
};

/**
 *  End namespace
 */
}}
