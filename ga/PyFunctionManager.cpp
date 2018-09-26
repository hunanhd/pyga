
#include "PyFunctionManager.h"

// 实例化静态成员变量
template<> PyFunctionManager* Singleton<PyFunctionManager>::ms_Singleton = 0;

PyFunctionManager::PyFunctionManager()
{

}

PyFunctionManager::~PyFunctionManager()
{

}

void PyFunctionManager::add( const CString& cmd, PyFunc func )
{
    // 命令变成大写
    CString upper_cmd = cmd;
    upper_cmd.MakeUpper();
    m_cfMap[upper_cmd] = func;
}

void PyFunctionManager::remove( const CString& cmd )
{
    // 命令变成大写
    CString upper_cmd = cmd;
    upper_cmd.MakeUpper();

    CmdFuncMap::iterator itr = m_cfMap.find( upper_cmd );
    if( itr != m_cfMap.end() )
    {
        m_cfMap.erase( itr );
    }
}

void PyFunctionManager::setCmd( const CString& cmd )
{
    // 命令变成大写
    CString upper_cmd = cmd;
    upper_cmd.MakeUpper();

    this->m_cmd = upper_cmd;
}

void PyFunctionManager::runCmd()
{
    CmdFuncMap::iterator itr = m_cfMap.find( this->m_cmd );
    if( itr != m_cfMap.end() )
    {
        python::object func = itr->second;
        func(); // 调用函数
    }
}

void PyFunctionManager::Init()
{
    PyFunctionManager* lfm = new PyFunctionManager();
}

void PyFunctionManager::UnInit()
{
    delete PyFunctionManager::getSingletonPtr();
}
