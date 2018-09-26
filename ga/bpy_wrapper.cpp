#include "bpy_wrapper.h"
#include "bpy_typemap.h"
#include "Interpreter.h"
//#include "PyFunctionManager.h"

#include "ga.h"

namespace ga
{

    int add( int a, int b )
    {
        PyGILStateLocker lock;

        return JLAdd(a,b);
    }

	python::object test( const char* name,  python::object datas )
	{
		PyGILStateLocker lock;
		std::vector<double> _datas; 
		seq_2_stl(datas, _datas );

		JLTest(_datas);

		python::list list_datas;
		for (std::vector<double>::iterator itr = _datas.begin(); itr != _datas.end(); itr++)
		{
			list_datas.append(*itr);
		}
		

		python::list list_tuple_datas;
		list_tuple_datas.append( python::make_tuple( "name", name ) );
		list_tuple_datas.append( python::make_tuple( "data", list_datas ) );

		python::dict d( list_tuple_datas );

		return d;
	}

	void printf(const char* msg)
	{
		JLPrint(msg);
	}

} // namespace ga
