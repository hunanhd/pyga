#pragma once

#include <assert.h>

//从soui里拿过来的
template <typename T>
class Singleton
{
protected:
    static    T* ms_Singleton;

public:
    Singleton( void )
    {
        assert( !ms_Singleton );
        ms_Singleton = static_cast<T*>( this );
    }
    virtual ~Singleton( void )
    {
        assert( ms_Singleton );
        ms_Singleton = 0;
    }
    static T& getSingleton( void )
    {
        assert( ms_Singleton );
        return ( *ms_Singleton );
    }
    static T* getSingletonPtr( void )
    {
        return ( ms_Singleton );
    }

private:
    Singleton<T>& operator=( const Singleton<T>& )
    {
        return *this;
    }
    Singleton( const Singleton<T>& ) {}
};