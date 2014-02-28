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
     *  Map of field names to their field
     */
    std::map<std::string, ResultField> _namedFields;

    /**
     *  List of fields without name
     */
    std::vector<ResultField> _indexedFields;
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
    size_t size() const;

    /**
     *  Retrieve a field by index
     *
     *  This function throws an exception if no field
     *  exists under the given index (i.e. index
     *  is not smaller than size()).
     *
     *  @param  index   field index
     */
    const ResultField& operator [] (size_t index) const;

    /**
     *  Retrieve a field by name
     *
     *  This function throws an exception if no field
     *  exists under the given key.
     *
     *  @param  key     field name
     */
    const ResultField& operator [] (const std::string& key) const;

    /**
     *  Get iterator for first field
     */
    std::map<std::string, ResultField>::const_iterator begin() const;

    /**
     *  Get iterator for field by the given name
     */
    std::map<std::string, ResultField>::const_iterator find(const std::string& key) const;

    /**
     *  Get the iterator past the end
     */
    std::map<std::string, ResultField>::const_iterator end() const;
};

/**
 *  End namespace
 */
}}
