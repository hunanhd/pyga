
#include "PyFunctionManager.h"

// ʵ������̬��Ա����
template<> PyFunctionManager* Singleton<PyFunctionManager>::ms_Singleton = 0;

PyFunctionManager::PyFunctionManager()
{

}

PyFunctionManager::~PyFunctionManager()
{

}

void PyFunctionManager::add( const CString& cmd, PyFunc func )
{
    // �����ɴ�д
    CString upper_cmd = cmd;
    upper_cmd.MakeUpper();
    m_cfMap[upper_cmd] = func;
}

void PyFunctionManager::remove( const CString& cmd )
{
    // �����ɴ�д
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
    // �����ɴ�д
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
        func(); // ���ú���
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
