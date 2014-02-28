/**
 *  Exception.h
 *
 *  Exception thrown by the REACT-CPP-MYSQL library
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Class definition
 */
class Exception : public React::Exception
{
public:
    /**
     *  Constructor
     *
     *  @param  message
     */
    Exception(const char *message) : React::Exception(message) {}

    /**
     *  Destructor
     */
    virtual ~Exception() {}
};

/**
 *  End namespace
 */
}}
