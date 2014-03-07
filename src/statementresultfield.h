/**
 *  StatementResultField.h
 *
 *  Class representing a result field in
 *  a result set from a prepared statement
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Result field class
 */
class StatementResultField : public ResultFieldImpl
{
private:
    /**
     *  Are we NULL?
     */
    my_bool _null;

    /**
     *  Get access to the data pointer for MySQL to fill
     */
    virtual void *getValue() = 0;

    /**
     *  Get access to the NULL pointer for MySQL to fill
     */
    my_bool *getNULL()
    {
        return &_null;
    }
protected:
    /**
     *  Is this a dynamically sized field?
     */
    virtual bool dynamic()
    {
        // most fields are not
        return false;
    }
public:
    /**
     *  Constructor
     */
    StatementResultField() : _null(false) {}

    /**
     *  Destructor
     */
    virtual ~StatementResultField() {};

    /**
     *  Is this a NULL field?
     */
    bool isNULL() const
    {
        return _null;
    }

    /**
     *  Cast to a number
     */
    virtual operator int8_t()   const override = 0;
    virtual operator uint16_t() const override = 0;
    virtual operator int16_t()  const override = 0;
    virtual operator uint32_t() const override = 0;
    virtual operator int32_t()  const override = 0;
    virtual operator uint64_t() const override = 0;
    virtual operator int64_t()  const override = 0;
    virtual operator float()    const override = 0;
    virtual operator double()   const override = 0;

    /**
     *  Cast to a string
     */
    virtual operator std::string() const override = 0;

    // friends and families
    friend class StatementResultInfo;
};

/**
 *  End namespace
 */
}}
