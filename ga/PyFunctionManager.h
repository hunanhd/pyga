#pragma once

#include "Singleton.h"
#include <map>

#include <boost/python.hpp>
using namespace boost;

typedef python::object PyFunc;

/*
PyFunctionManager����bpy_export.h/AddCommand()�������ʵ����ע��cad�����������õ���python����
��������:
1��py�ű��ļ��ж���һ��main()����(�μ�hello.py)
2��main()��������arx.add_command()����ע�����cad��(�μ�hello.py/RegCommands()����)
   ע:arx_add_command()������c++����AddCommand()��lua��װ�汾
3��AddCommand()�����ڲ�����2����:
	(1) ����addCommand()������̬��ע��cad����,���е�py�����һ������: RunPyFunction()
	(2) ���������� �� py�������� ע���PyFunctionManager��������(�����ڲ�ά����һ��map)
4�����û���cad����������������ʱ,������Ӧ��EditorReactor::commandWillStart(),�÷�Ӧ���õ����������֮��,
	��¼��PyFunctionManager������
5��py����ִ�е�ʱ��,����RunPyFunction()����(���е�py�����ͬһ������)
6��RunPyFunction()��������PyFunctionManager::runCmd()����,runCmd()��������py����������py����

ע��: ʹ�õ�ʱ��,������ִ��lua�ű�(ִ��runlua�������������),�����������̵ĵ�(1)��!!!
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
    CString m_cmd; // ��ǰ��������
    typedef std::map<CString, PyFunc> CmdFuncMap;
    CmdFuncMap m_cfMap; // ��¼ �������� ->py���� ��ӳ���ϵ
};