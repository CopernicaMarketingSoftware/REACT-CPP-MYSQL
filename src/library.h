/**
 *  Library.h
 *
 *  Provides the Library class, that makes sure the
 *  initialized mysql library stays available until
 *  program termination.
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace React { namespace MySQL {

/**
 *  Library class
 */
class Library
{
public:
    /**
     *  Constructor
     */
    Library()
    {
        // initialize the library
        mysql_library_init(0, nullptr, nullptr);
    }

    /**
     *  Destructor
     */
    ~Library()
    {
        // finish the library on exit
        mysql_library_end();
    }
};

/**
 *  End namespace
 */
}}
