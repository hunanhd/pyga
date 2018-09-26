#include "bpy_wrapper.h"
//using namespace ga;

/*
但是set函数有多个重载,有2种方法:
(1) 函数指针(boost python官网的方案)
参考: http://www.boost.org/doc/libs/1_61_0/libs/python/doc/html/tutorial/tutorial/functions.html#tutorial.functions.overloading
AcGePoint3d& (AcGePoint3d::*setXYZ)(double, double, double) = &AcGePoint3d::set;
(2) 再封装一个函数(thin wrapper),这种方案也不错,更灵活一些
http://stackoverflow.com/questions/1571054/boost-python-how-to-return-by-reference
void Foo_set_x(Foo& self, float value) {
self.get() = value;
}

class_<Foo>("Foo", init<float>())
...
.def("set", &Foo_set_x);

*/

// 模块名(ga)与生成pyd文件名(ga.pyd)要一样!!!
BOOST_PYTHON_MODULE( ga )
{
    // 注册add函数
	python::def( "add", ga::add );
	python::def( "test", ga::test);
	python::def( "printf", ga::printf);
}