#ifndef BASE_EXCEPTION_H
#define BASE_EXCEPTION_H

#include "dlimexp.h"

/* À´Ô´ÓÚfreecad */

#include <exception>
#include <string>

class GA_EXPORT_API Exception
{
public:
    Exception( const char* sMessage );
    Exception( const std::string& sMessage );
    Exception( void );
    Exception( const Exception& inst );
    virtual ~Exception() throw() {}

    Exception& operator=( const Exception& inst );
    virtual const char* what( void ) const throw();
    virtual void ReportException ( void ) const;
    inline void setMessage( const char* sMessage );
    inline void setMessage( const std::string& sMessage );

protected:
    std::string _sErrMsg;
};

inline void Exception::setMessage( const char* sMessage )
{
    _sErrMsg = sMessage;
}

inline void Exception::setMessage( const std::string& sMessage )
{
    _sErrMsg = sMessage;
}

#endif // BASE_EXCEPTION_H

