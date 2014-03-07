/**
 *  StatementResultInfo.h
 *
 *  Information about the result set of a prepared statement
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Result info class
 */
class StatementResultInfo
{
private:
    /**
     *  MySQL bind structures for retrieving data
     */
    std::vector<MYSQL_BIND> _bind;

    /**
     *  The statement we are the result info for
     */
    MYSQL_STMT *_statement;

    /**
     *  Field names to field index
     */
    std::map<std::string, size_t> _fields;
public:
    /**
     *  Constructor
     *
     *  @param  result  field result set
     */
    StatementResultInfo(MYSQL_STMT *statement, MYSQL_RES *result) :
        _statement(statement)
    {
        // get the number of fields in the statement
        size_t fields = mysql_num_fields(result);

        // prepare the vector for data
        _bind.reserve(fields);

        // process all fields
        while (auto *field = mysql_fetch_field(result))
        {
            // create and initialize bound parameter
            MYSQL_BIND bind;
            std::memset(&bind, 0, sizeof(bind));

            // if we are a numeric field, we should store whether we are unsigned
            if (IS_NUM(field->type)) bind.is_unsigned = field->flags & UNSIGNED_FLAG;

            // store the field type
            bind.buffer_type = field->type;

            // store name of the field
            _fields[std::string(field->name, field->name_length)] = _bind.size();

            // add the bound field to the list
            _bind.push_back(bind);
        }

        // in the end, we have to clean up the result set
        mysql_free_result(result);
    }

    /**
     *  Get the fields and their index
     */
    const std::map<std::string, size_t>& fields() const
    {
        return _fields;
    }

    /**
     *  Number of rows in the result set
     */
    size_t size()
    {
        return _bind.size();
    }

    /**
     *  Retrieve the rows in the result
     */
    std::shared_ptr<StatementResultImpl> rows()
    {
        // store all the rows locally
        if (mysql_stmt_store_result(_statement)) throw Exception(mysql_stmt_error(_statement));

        // retrieve the number of rows
        size_t count = mysql_stmt_num_rows(_statement);

        // the result value with all the rows
        std::vector<std::vector<std::unique_ptr<StatementResultField>>> result(count);

        // fetch all rows
        for (auto &row : result)
        {
            // prepare the row for data
            row.reserve(_bind.size());

            // prepare all fields
            for (auto &bind : _bind)
            {
                // create the field
                switch (bind.buffer_type)
                {
                    case MYSQL_TYPE_TINY:
                        row.emplace_back(new StatementSignedCharResultField());
                        break;
                    case MYSQL_TYPE_SHORT:
                        if (bind.is_unsigned)   row.emplace_back(new StatementUnsignedShortResultField());
                        else                    row.emplace_back(new StatementSignedShortResultField());
                        break;
                    case MYSQL_TYPE_LONG:
                        if (bind.is_unsigned)   row.emplace_back(new StatementUnsignedLongResultField());
                        else                    row.emplace_back(new StatementSignedLongResultField());
                        break;
                    case MYSQL_TYPE_LONGLONG:
                        if (bind.is_unsigned)   row.emplace_back(new StatementUnsignedLongLongResultField());
                        else                    row.emplace_back(new StatementSignedLongLongResultField());
                        break;
                    case MYSQL_TYPE_FLOAT:
                        row.emplace_back(new StatementFloatResultField());
                        break;
                    case MYSQL_TYPE_DOUBLE:
                        row.emplace_back(new StatementDoubleResultField());
                        break;
                    case MYSQL_TYPE_VARCHAR:
                    case MYSQL_TYPE_VAR_STRING:
                    case MYSQL_TYPE_STRING:
                    case MYSQL_TYPE_TINY_BLOB:
                    case MYSQL_TYPE_MEDIUM_BLOB:
                    case MYSQL_TYPE_LONG_BLOB:
                    case MYSQL_TYPE_BLOB:
                        row.emplace_back(new StatementDynamicResultField());
                        break;
                    default:
                        // TODO: temporal fields
                        break;
                }

                // retrieve the freshly minted field
                auto &field = row.back();

                // if we have a fixed-size field, we can assign the data- and null-pointer
                if (!field->dynamic())
                {
                    // assign the data buffer and the null pointer to the bind structure
                    bind.buffer  = field->getValue();
                    bind.is_null = field->getNULL();
                }
                else
                {
                    // field is dynamic, cast to get access to properties
                    StatementDynamicResultField *dynamic = static_cast<StatementDynamicResultField*>(field.get());

                    // set the buffer to be a null pointer and give MySQL a pointer to store the length
                    bind.buffer  = nullptr;
                    bind.is_null = field->getNULL();
                    bind.length  = &dynamic->_size;
                    bind.buffer_length = 0;
                }
            }

            // bind the output parameters to the statement (this has to be done every time)
            if (mysql_stmt_bind_result(_statement, _bind.data())) throw Exception(mysql_stmt_error(_statement));

            // fetch the data into the buffers
            switch (mysql_stmt_fetch(_statement))
            {
                case 0:
                    // fetch successful, all data loaded
                    break;
                case 1:
                    // something went horribly wrong
                    throw Exception(mysql_stmt_error(_statement));
                case MYSQL_NO_DATA:
                    // this is also not allowed, since we only fetch
                    // as many rows as were indicated to be present
                    throw Exception("Result set corrupted");
                case MYSQL_DATA_TRUNCATED:
                    // some fields need more data, fetch it
                    // we should know the required size by now
                    for (size_t i = 0; i < _bind.size(); ++i)
                    {
                        // get bind property and field
                        auto &bind  = _bind[i];
                        auto &field = row[i];

                        // skip fixed-size fields and NULL fields
                        if (!field->dynamic() || field->isNULL()) continue;

                        // cast to a dynamic field
                        StatementDynamicResultField *dynamic = static_cast<StatementDynamicResultField*>(field.get());

                        // no need to allocate if it is an empty field
                        if (!dynamic->_size) continue;

                        // allocate memory and retrieve the field
                        dynamic->_value = static_cast<char*>(std::malloc(dynamic->_size));

                        // assign the buffer and indicate the size to MySQL
                        bind.buffer = dynamic->_value;
                        bind.buffer_length = dynamic->_size;

                        // fetch the field from MySQL
                        mysql_stmt_fetch_column(_statement, &bind, i, 0);
                    }

                    // done
                    break;
            }
        }

        // all done, wrap the data in a result container
        return std::make_shared<StatementResultImpl>(_fields, std::move(result));
    }
};

/**
 *  End namespace
 */
}}
