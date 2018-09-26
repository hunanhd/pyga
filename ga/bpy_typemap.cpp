#include "bpy_typemap.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///* Ãÿ ‚¥¶¿ÌCString */
//
//template<>
//void seq_2_stl<CString>( python::object seq, std::vector<CString>& args )
//{
//    for ( int i = 0; i < python::len( seq ); ++i )
//    {
//        std::string temp = python::extract<std::string>( seq[i] );
//        args.push_back( C2W( temp.c_str() ) );
//    }
//}
//
//template<>
//python::object stl_2_seq<CString>( const std::vector<CString>& args )
//{
//    python::list seq;
//    for( int i = 0; i < static_cast<int>( args.size() ); ++i )
//    {
//        std::string temp( W2C( ( LPCTSTR )args[i] ) );
//        seq.append( temp );
//    }
//    return seq;
//}
//
//template<>
//void seq_2_stl<CString>( python::object seq, std::list<CString>& args )
//{
//    for ( int i = 0; i < python::len( seq ); ++i )
//    {
//        std::string temp = python::extract<std::string>( seq[i] );
//        args.push_back( C2W( temp.c_str() ) );
//    }
//}
//
//template<>
//python::object stl_2_seq<CString>( const std::list<CString>& args )
//{
//    python::list seq;
//    typedef std::list<CString> StringList;
//    StringList::const_iterator itr = args.begin();
//    for( ; itr != args.end(); ++itr )
//    {
//        std::string temp( W2C( ( LPCTSTR )*itr ) );
//        seq.append( temp );
//    }
//    return seq;
//}
//
//template<>
//void dict_2_stl<CString, CString>( python::dict d, std::map<CString, CString>& args )
//{
//    python::list keys = d.keys();
//    if( python::len( keys ) == 0 ) return;
//
//    for( int i = 0 ; i < static_cast<int>( python::len( keys ) ) ; ++i )
//    {
//        std::string sKey = python::extract<std::string>( keys[i] );
//        std::string sValue = python::extract<std::string>( d.get( keys[i] ) );
//        args.insert( std::make_pair( CString( C2W( sKey.c_str() ) ), CString( C2W( sValue.c_str() ) ) ) );
//    }
//}
//
//template<>
//python::object stl_2_dict<CString, CString>( const std::map<CString, CString>& args )
//{
//    std::map<CString, CString>::const_iterator itr = args.begin();
//    python::list datas;
//    for ( ; itr != args.end(); ++itr )
//    {
//        std::string sKey( W2C( ( LPCTSTR )itr->first ) );
//        std::string sValue( W2C( ( LPCTSTR )itr->second ) );
//        datas.append( python::make_tuple( sKey, sValue ) );
//    }
//    python::dict d( datas );
//    return d;
//}
