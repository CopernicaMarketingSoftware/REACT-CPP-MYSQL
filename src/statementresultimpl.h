/**
 *  StatementResultImpl.h
 *
 *  Result set from a prepared statement
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Result set from a prepared statement
 */
class StatementResultImpl : public ResultImpl
{
    /**
     *  The map from field name to index
     */
    std::map<std::string, size_t> _fields;

    /**
     *  The field data
     */
    std::vector<std::vector<std::unique_ptr<StatementResultField>>> _rows;
public:
    /**
     *  Construct result
     */
    StatementResultImpl(const std::map<std::string, size_t>& fields, std::vector<std::vector<std::unique_ptr<StatementResultField>>>&& rows) :
        _fields(fields),
        _rows(std::move(rows))
    {}

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
    MYSQL_ROW fetch(size_t index) override
    {
        return nullptr;
    }
};

/**
 *  End namespace
 */
}}
