#pragma once

#include <boost/python.hpp>

using namespace boost;

/*
boost python封装的一些技巧:
(1) 参数的字符串都使用const char*
(2) 返回值是字符串的用std::string
(3) 参数和返回值是一个线性容器(array、vector、list等),都改成用python::object(代表一个python序列)
(4) 有多个返回值的,返回值使用python::tuple(与python语言里的tuple是对应的)
*/
namespace ga
{
    extern int add( int a, int b );
    extern python::object test( const char* name, python::object datas );
	extern void printf(const char* msg);
} // namespace JL