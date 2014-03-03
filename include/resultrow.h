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
private:
    /**
     *  The result set to which this row belongs
     *  @var    std::shared_ptr
     */
    std::shared_ptr<ResultImpl> _result;

    /**
     *  The row returned from MYSQL
     *  @var    MYSQL_ROW
     */
    MYSQL_ROW _row;

    /**
     *  Create an iterator class
     */
    class iterator
    {
    private:
        /**
         *  Underlying map iterator
         */
        std::map<std::string, size_t>::const_iterator _iterator;

        /**
         *  The result row
         */
        const ResultRow *_row;
    public:
        /**
         *  Empty constructor
         */
        iterator() : _iterator(std::map<std::string, size_t>::const_iterator()), _row(NULL) {}

        /**
         *  Constructor
         *  @param  parent
         *  @param  index
         */
        iterator(std::map<std::string, size_t>::const_iterator&& iterator, const ResultRow *row) : _iterator(std::move(iterator)), _row(row) {}

        /**
         *  Copy constructor
         *
         *  @param  that    object to copy
         */
        iterator(const iterator& that) : _iterator(that._iterator), _row(that._row) {}

        /**
         *  Destructor
         */
        virtual ~iterator() {}

        /**
         *  Assign another iterator
         *
         *  @param  that    other iterator to copy
         */
        iterator& operator=(const iterator& that)
        {
            // copy iterator
            _iterator = that._iterator;
            _row = that._row;

            // allow chaining
            return *this;
        }

        /**
         *  Increment operator
         */
        iterator &operator++()
        {
            _iterator++;
            return *this;
        }

        /**
         *  Increment operator (postfix)
         */
        iterator operator++(int)
        {
            // make a copy of ourselves
            iterator copy(*this);

            // increment iterator
            _iterator++;

            // return the copy
            return copy;
        }

        /**
         *  Decrement operator
         */
        iterator &operator--()
        {
            _iterator--;
            return *this;
        }

        /**
         *  Decrement operator (postfix)
         */
        iterator operator--(int)
        {
            // make a copy of ourselves
            iterator copy(*this);

            // decrement iterator
            _iterator--;

            // return the copy
            return copy;
        }

        /**
         *  Compare with other operator
         *  @param  iterator
         */
        bool operator==(const iterator& iterator) const
        {
            return _iterator == iterator._iterator;
        }

        /**
         *  Compare with other operator
         */
        bool operator!=(const iterator& iterator) const
        {
            return _iterator != iterator._iterator;
        }

        /**
         *  Dereference
         *  @return ResultField
         */
        std::pair<std::string, ResultField> operator*() const
        {
            return std::make_pair<>(_iterator->first,_row->operator[](_iterator->first));
        }

        /**
         *  Call method on the dereferenced object
         */
        std::unique_ptr<std::pair<std::string, ResultField>> operator->() const
        {
            return std::unique_ptr<std::pair<std::string, ResultField>>(new std::pair<std::string, ResultField>(_iterator->first,_row->operator[](_iterator->first)));
        }
    };

public:
    /**
     *  Construct the row
     *
     *  @param  result  the result object with all the rows
     *  @param  row     the fields for this row
     */
    ResultRow(std::shared_ptr<ResultImpl> result, MYSQL_ROW row) :
        _result(result), _row(row) {}

    /**
     *  Destructor
     */
    virtual ~ResultRow() {}

    /**
     *  Get the number of fields in the row
     *  @return size_t
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
    const ResultField operator [] (size_t index) const;

    /**
     *  Retrieve a field by name
     *
     *  This function throws an exception if no field
     *  exists under the given key.
     *
     *  @param  key     field name
     */
    const ResultField operator [] (const std::string &key) const;

    /**
     *  Get iterator for first field
     *  @return const_iterator
     */
    iterator begin() const;

    /**
     *  Get iterator for field by the given field name
     *  @param  name
     */
    iterator find(const std::string& key) const;

    /**
     *  Get the iterator past the end
     */
    iterator end() const;
};

/**
 *  End namespace
 */
}}
