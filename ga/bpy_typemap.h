#pragma once

#include<boost/python.hpp> 

using namespace boost;

#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>

typedef std::vector<std::string> StringArray;

// 用于python list和stl map的内部转换
#define INTERNAL_KEY "__KEY__"
#define INTERNAL_VALUE "__VALUE__"

/*
* stl容器与python list之间的转换
*/
template<typename T>
void seq_2_stl( python::object seq, std::vector<T>& args )
{
    for ( int i = 0; i < python::len( seq ); ++i )
    {
        args.push_back( python::extract<T>( seq[i] ) );;
    }
}

template<typename T>
python::object stl_2_seq( const std::vector<T>& args )
{
    python::list seq;
    for( int i = 0; i < static_cast<int>( args.size() ); ++i )
    {
        seq.append( args[i] );
    }
    return seq;
}

template<typename T>
void seq_2_stl( python::object seq, std::list<T>& args )
{
    for ( int i = 0; i < python::len( seq ); ++i )
    {
        args.push_back( python::extract<T>( seq[i] ) );;
    }
}

template<typename T>
python::object stl_2_seq( const std::list<T>& args )
{
    python::list seq;
    typename std::list<T>::const_iterator itr = args.begin();
    for( ; itr != args.end(); ++itr )
    {
        seq.append( *itr ); //lua的seq下标都是从1开始的
    }
    return seq;
}

template<typename K, typename V>
void dict_2_stl( python::dict d, std::map<K, V>& args )
{
    python::list keys = d.keys();
    if( keys.count() == 0 ) return;

    for( int i = 0 ; i < static_cast<int>( keys.count() ) ; ++i )
    {
        K key = python::extract<K>( keys[i] );
        V value = d.get( key );
        args.insert( std::make_pair( key, value ) );
    }
}

template<typename K, typename V>
python::object stl_2_dict( const std::map<K, V>& args )
{
    typename map<K, V>::const_iterator itr = args.begin();
    python::list datas;
    for ( ; itr != args.end(); ++itr )
    {
        datas.append( python::make_tuple( itr->first, itr->second ) );
    }
    python::dict d( datas );
    return d;
}

/*
* CString与python list之间的转换
*/
// map偏特化(map<string, V>或map<CString, V>
//template<typename V>
//void dict_2_stl( python::dict d, std::map<CString, V>& args )
//{
//    python::list keys = d.keys();
//    if( keys.count() == 0 ) return;
//
//    for( int i = 0 ; i < static_cast<int>( keys.count() ) ; ++i )
//    {
//        std::string key = python::extract<std::string>( keys[i] );
//        V value = d.get( key );
//        args.insert( std::make_pair( CString( C2W( key.c_str() ) ), value ) );
//    }
//}

//!需要定义成(python::list& seq) 吗,NO
//template<typename V>
//python::object stl_2_dict( const std::map<CString, V>& args )
//{
//    typename map<CString, V>::const_iterator itr = args.begin();
//    python::list datas;
//    for ( ; itr != args.end(); ++itr )
//    {
//        std::string sKey( W2C( ( LPCTSTR )itr->first ) );
//        datas.append( python::make_tuple( sKey, itr->second ) );
//    }
//    python::dict d( datas );
//    return d;
//}

//// vector
//template<>
//extern void seq_2_stl<CString>( python::object seq, std::vector<CString>& args );
//template<>
//extern python::object stl_2_seq<CString>( const std::vector<CString>& args );
//// list
//template<>
//extern void seq_2_stl<CString>( python::object seq, std::list<CString>& args );
//template<>
//extern python::object stl_2_seq<CString>( const std::list<CString>& args );
//// map
//template<>
//extern void dict_2_stl<CString, CString>( python::dict seq, std::map<CString, CString>& args );
//template<>
//extern python::object stl_2_dict<CString, CString>( const std::map<CString, CString>& args );
