#include "Exception.h"

Exception::Exception( void )
{
    _sErrMsg = "PyScript Exception";
}

Exception::Exception( const Exception& inst )
    : _sErrMsg( inst._sErrMsg )
{
}


Exception::Exception( const char* sMessage )
    : _sErrMsg( sMessage )
{
}

Exception::Exception( const std::string& sMessage )
    : _sErrMsg( sMessage )
{
}

Exception& Exception::operator=( const Exception& inst )
{
    _sErrMsg = inst._sErrMsg;
    return *this;
}

const char* Exception::what( void ) const throw()
{
    return _sErrMsg.c_str();
}

void Exception::ReportException ( void ) const
{
    //COleDateTime dt = COleDateTime::GetCurrentTime();
    //acutPrintf( _T( "\nException (%s): %s" ), dt.Format( _T( "%Y%m%d%H%M%S" ) ), C2W( what() ) );
}