/**
 *  ResultRow.h
 *
 *  Class with result data for a single MySQL row
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
 *  Result row class
 */
class ResultRow
{
    /**
     *  The result set
     */
    std::shared_ptr<ResultImpl> _result;

    /**
     *  The field data
     */
    MYSQL_ROW _fields;
public:
    /**
     *  Construct the row
     *
     *  @param  result  the result object with all the rows
     *  @param  fields  the fields for this row
     */
    ResultRow(std::shared_ptr<ResultImpl> result, MYSQL_ROW fields);

    /**
     *  Get the number of fields in the row
     */
    size_t size();

    /**
     *  Retrieve a field by index
     *
     *  This function throws an exception if no field
     *  exists under the given index (i.e. index
     *  is not smaller than size()).
     *
     *  @param  index   field index
     */
    std::string operator [] (size_t index) const;

    /**
     *  Retrieve a field by name
     *
     *  This function throws an exception if no field
     *  exists under the given key.
     *
     *  @param  key     field name
     */
    std::string operator [] (const std::string& key) const;
};

/**
 *  End namespace
 */
}}
