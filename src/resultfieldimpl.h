/**
 *  ResultFieldImpl.h
 *
 *  Class representing a result field
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
class ResultFieldImpl
{
public:
    /**
     *  Is this a NULL field?
     */
    virtual bool isNULL() const = 0;

    /**
     *  Cast to a number
     */
    virtual operator int8_t()   const = 0;
    virtual operator uint16_t() const = 0;
    virtual operator int16_t()  const = 0;
    virtual operator uint32_t() const = 0;
    virtual operator int32_t()  const = 0;
    virtual operator uint64_t() const = 0;
    virtual operator int64_t()  const = 0;
    virtual operator float()    const = 0;
    virtual operator double()   const = 0;

    /**
     *  Cast to a string
     */
    virtual operator std::string() const = 0;

    /**
     *  Cast to a time structure
     */
    virtual operator std::tm() const = 0;
};

/**
 *  End namespace
 */
}}
