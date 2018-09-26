#include "bpy_wrapper.h"
//using namespace ga;

/*
����set�����ж������,��2�ַ���:
(1) ����ָ��(boost python�����ķ���)
�ο�: http://www.boost.org/doc/libs/1_61_0/libs/python/doc/html/tutorial/tutorial/functions.html#tutorial.functions.overloading
AcGePoint3d& (AcGePoint3d::*setXYZ)(double, double, double) = &AcGePoint3d::set;
(2) �ٷ�װһ������(thin wrapper),���ַ���Ҳ����,�����һЩ
http://stackoverflow.com/questions/1571054/boost-python-how-to-return-by-reference
void Foo_set_x(Foo& self, float value) {
self.get() = value;
}

class_<Foo>("Foo", init<float>())
...
.def("set", &Foo_set_x);

*/

// ģ����(ga)������pyd�ļ���(ga.pyd)Ҫһ��!!!
BOOST_PYTHON_MODULE( ga )
{
    // ע��add����
	python::def( "add", ga::add );
	python::def( "test", ga::test);
	python::def( "printf", ga::printf);
}