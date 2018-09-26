#pragma once

#include "Singleton.h"
#include <map>

#include <boost/python.hpp>
using namespace boost;

typedef python::object PyFunc;

/*
PyFunctionManager类与bpy_export.h/AddCommand()函数配合实现了注册cad命令，该命令调用的是python函数
基本流程:
1、py脚本文件中定义一个main()函数(参见hello.py)
2、main()函数调用arx.add_command()函数注册命令到cad中(参见hello.py/RegCommands()函数)
   注:arx_add_command()函数是c++函数AddCommand()的lua封装版本
3、AddCommand()函数内部做了2件事:
	(1) 调用addCommand()方法动态的注册cad命令,所有的py命令都绑定一个函数: RunPyFunction()
	(2) 将命令名称 和 py函数名称 注册给PyFunctionManager单件对象(它的内部维护了一个map)
4、当用户在cad命令行中输入命令时,触发反应器EditorReactor::commandWillStart(),该反应器得到命令的名称之后,
	记录在PyFunctionManager对象中
5、py命令执行的时候,调用RunPyFunction()函数(所有的py命令都绑定同一个函数)
6、RunPyFunction()函数调用PyFunctionManager::runCmd()方法,runCmd()方法利用py解释器调用py函数

注意: 使用的时候,必须先执行lua脚本(执行runlua命令或其它方法),触发上述流程的第(1)步!!!
*/
class PyFunctionManager : public Singleton<PyFunctionManager>
{
public:
    static void Init();
    static void UnInit();

public:
    PyFunctionManager();
    ~PyFunctionManager();

    void add( const CString& cmd, PyFunc func );
    void remove( const CString& cmd );
    void setCmd( const CString& cmd );
    void runCmd();

private:
    CString m_cmd; // 当前命令名称
    typedef std::map<CString, PyFunc> CmdFuncMap;
    CmdFuncMap m_cfMap; // 记录 命令名称 ->py函数 的映射关系
};