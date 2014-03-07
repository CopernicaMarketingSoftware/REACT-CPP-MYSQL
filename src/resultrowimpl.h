/**
 *  ResultRowImpl
 *
 *  Result row wrapper class to be wrapped around
 *  either a MYSQL_ROW or a vector of field objects
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Row implementation
 */
class ResultRowImpl
{
    /**
     *  MySQL row
     */
    std::vector<std::unique_ptr<QueryResultField>> _row;

    /**
     *  Field vector
     */
    std::vector<std::unique_ptr<StatementResultField>> *_fields
public:
    /**
     *  Constructor
     */
    ResultRowImpl(MYSQL_ROW row, size_t count) :
        _fields(nullptr)
    {
        // reserve fields
        _row.reserve(count);

        // and create them
        for (size_t i = 0; i < count; ++i) _row.emplace_back(row[i]);
    }

    /**
     *  Constructor
     *
     *  @param  fields  field data
     */
    ResultRowImpl(std::vector<std::unique_ptr<StatementResultField>> *fields) :
        _row(nullptr),
        _fields(fields)
    {}

    /**
     *  Retrieve a field
     */
    ResultFieldImpl *field(size_t index)
    {
        if (!_row.empty()) return row[i].get();
        else return _fields[i].get();
    }
};

/**
 *  End namespace
 */
}}
