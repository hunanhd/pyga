#pragma once

#include <boost/python.hpp>

using namespace boost;

/*
boost python��װ��һЩ����:
(1) �������ַ�����ʹ��const char*
(2) ����ֵ���ַ�������std::string
(3) �����ͷ���ֵ��һ����������(array��vector��list��),���ĳ���python::object(����һ��python����)
(4) �ж������ֵ��,����ֵʹ��python::tuple(��python�������tuple�Ƕ�Ӧ��)
*/
namespace ga
{
    extern int add( int a, int b );
    extern python::object test( const char* name, python::object datas );
	extern void printf(const char* msg);
} // namespace JL