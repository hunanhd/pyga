#ifndef BASE_PYOBJECTBASE_H
#define BASE_PYOBJECTBASE_H

// needed header
#undef slots
#include <Python.h>
#define slots
#include <iostream>
#include <bitset>
#include <typeinfo>

#include "Exception.h"
#include "CXX/Objects.hxx"

/** Python static class macro for definition
 * sets up a static function entry in a class inheriting
 * from PyObjectBase. Its a pure convenience macro. You can also do
 * it by hand if you want. It looks like that:
 * \code
 * static PyObject* X (PyObject *self,PyObject *args,PyObject *kwd);
 * \endcode
 * @param SFUNC is the static method name (use what you want)
 * @see PYFUNCIMP_S
 * @see FCPythonExport
 */
#define PYFUNCDEF_S(SFUNC)   static PyObject* SFUNC (PyObject *self,PyObject *args,PyObject *kwd)


/** Python static class macro for implementation
 * used to set up a implementation for PYFUNCDEF_S definition.
 * Its a pure convenience macro. You can also do
 * it by hand if you want. It looks like that:
 * \code
 * PyObject* CLASS::SFUNC (PyObject *self,PyObject *args,PyObject *kwd)
 * \endcode
 * see PYFUNCDEF_S for details
 * @param CLASS is the class in which the macro take place.
 * @param SFUNC is the object method get implemented
 * @see PYFUNCDEF_S
 * @see FCPythonExport
 */
#define PYFUNCIMP_S(CLASS,SFUNC) PyObject* CLASS::SFUNC (PyObject *self,PyObject *args,PyObject *kwd)

/**
 * Union to convert from PyTypeObject to PyObject pointer.
 */
union PyType_Object
{
    PyTypeObject* t;
    PyObject* o;
};


/*------------------------------
 * Basic defines
------------------------------*/
//typedef const char * version;     // define "version"


inline int PyStrEq( const char* A, const char* B ) // define "streq"
{
    return strcmp( A, B ) == 0;
}


inline void PyAssert( int expr, char* msg )       // C++ assert
{
    if ( !expr )
    {
        fprintf( stderr, "%s\n", msg );
        exit( -1 );
    };
}

/*------------------------------
 * Python defines
------------------------------*/

/// some basic python macros
#define Py_NEWARGS 1
/// return with no return value if nothing happens
#define Py_Return Py_INCREF(Py_None); return Py_None;
/// returns an error
#define Py_Error(E, M)   {PyErr_SetString(E, M); return NULL;}
/// checks on a condition and returns an error on failure
#define Py_Try(F) {if (!(F)) return NULL;}
/// assert which returns with an error on failure
#define Py_Assert(A,E,M) {if (!(A)) {PyErr_SetString(E, M); return NULL;}}


/// Define the PyParent Object
typedef PyTypeObject* PyParentObject;


/// This must be the first line of each PyC++ class
#define Py_Header                                           \
public:                                                     \
    static PyTypeObject   Type;                             \
    static PyMethodDef    Methods[];                        \
    virtual PyTypeObject *GetType(void) {return &Type;}

/** This defines the _getattr_up macro
 *  which allows attribute and method calls
 *  to be properly passed up the hierarchy.
 */
#define _getattr_up(Parent)                                 \
{                                                           \
    PyObject *rvalue = Py_FindMethod(Methods, this, attr);  \
    if (rvalue == NULL)                                     \
    {                                                       \
        PyErr_Clear();                                      \
        return Parent::_getattr(attr);                      \
    }                                                       \
    else                                                    \
        return rvalue;                                      \
}

/*------------------------------
 * PyObjectBase
------------------------------*/

/** The PyObjectBase class, exports the class as a python type
 *  PyObjectBase is the base class for all C++ classes which
 *  need to get exported into the python namespace. This class is
 *  very important because nearly all important classes in FreeCAD
 *  are visible in python for macro recording and automation purpose.
 *  The class App::Document is a good expample for an exported class.
 *  There are some convenience macros to make it easier to inherit
 *  from this class and defining new methods exported to python.
 *  PYFUNCDEF_D defines a new exported method.
 *  PYFUNCIMP_D defines the implementation of the new exported method.
 *  In the implementation you can use Py_Return, Py_Error, Py_Try and Py_Assert.
 *  PYMETHODEDEF makes the entry in the python method table.
 *  @see Document
 *  @see PYFUNCDEF_D
 *  @see PYFUNCIMP_D
 *  @see PYMETHODEDEF
 *  @see Py_Return
 *  @see Py_Error
 *  @see Py_Try
 *  @see Py_Assert
 */
class GA_EXPORT_API PyObjectBase : public PyObject
{
    /** Py_Header struct from python.h.
     *  Every PyObjectBase object is also a python object. So you can use
     *  every Python C-Library function also on a PyObjectBase object
     */
    Py_Header

protected:
    /// destructor
    virtual ~PyObjectBase();

public:
    /** Constructor
     *  Sets the Type of the object (for inheritance) and decrease the
     *  the reference count of the PyObject.
     */
    PyObjectBase( void*, PyTypeObject* T );
    /// Wrapper for the Python destructor
    static void PyDestructor( PyObject* P ) // python wrapper
    {
        delete ( ( PyObjectBase* ) P );
    }
    /// incref method wrapper (see python extending manual)
    PyObjectBase* IncRef( void )
    {
        Py_INCREF( this );
        return this;
    }
    /// decref method wrapper (see python extending manual)
    PyObjectBase* DecRef( void )
    {
        Py_DECREF( this );
        return this;
    }

    /** GetAttribute implementation
     *  This method implements the retrieval of object attributes.
     *  If you want to implement attributes in your class, reimplement
     *  this method.
     *  You have to call the method of the base class.
     *  Note: if you reimplement _gettattr() in a inheriting class you
     *  need to call the method of the base class! Otherwise even the
     *  methods of the object will disappear!
     */
    virtual PyObject* _getattr( char* attr );
    /// static wrapper for pythons _getattr()
    static  PyObject* __getattr( PyObject* PyObj, char* attr );

    /** SetAttribute implementation
     *  This method implements the setting of object attributes.
     *  If you want to implement attributes in your class, reimplement
     *  this method.
     *  You have to call the method of the base class.
     */
    virtual int _setattr( char* attr, PyObject* value );  // _setattr method
    /// static wrapper for pythons _setattr(). // This should be the entry in Type.
    static  int __setattr( PyObject* PyObj, char* attr, PyObject* value );

    /** _repr method
    * Override this method to return a string object with some
    * information about the object.
    * \code
    * PyObject *MeshFeaturePy::_repr(void)
    * {
    *   std::stringstream a;
    *   a << "MeshFeature: [ ";
    *   a << "some really important info about the object!";
    *   a << "]" << std::endl;
    *   return Py_BuildValue("s", a.str().c_str());
    * }
    * \endcode
    */
    virtual PyObject* _repr( void );
    /// python wrapper for the _repr() function
    static  PyObject* __repr( PyObject* PyObj )
    {
        if ( !( ( PyObjectBase* ) PyObj )->isValid() )
        {
            PyErr_Format( PyExc_ReferenceError, "Cannot print representation of deleted object" );
            return NULL;
        }
        return ( ( PyObjectBase* ) PyObj )->_repr();
    }

    /** PyInit method
    * Override this method to initialize a newly created
    * instance of the class (Constuctor)
    */
    virtual int PyInit( PyObject* /*args*/, PyObject* /*kwd*/ )
    {
        return 0;
    }
    /// python wrapper for the _repr() function
    static  int __PyInit( PyObject* self, PyObject* args, PyObject* kwd )
    {
        return ( ( PyObjectBase* ) self )->PyInit( args, kwd );
    }

    void setInvalid()
    {
        // first bit is not set, i.e. invalid
        StatusBits.reset( 0 );
        _pcTwinPointer = 0;
    }

    bool isValid()
    {
        return StatusBits.test( 0 );
    }

    void setConst()
    {
        // second bit is set, i.e. immutable
        StatusBits.set( 1 );
    }

    bool isConst()
    {
        return StatusBits.test( 1 );
    }

    void setAttributeOf( const char* attr, const PyObjectBase* par );
    void startNotify();

    typedef void* PointerType ;

protected:
    std::bitset<32> StatusBits;
    /// pointer to the handled class
    void* _pcTwinPointer;
    PyObjectBase* parent;
    char* attribute;
};


/** Python dynamic class macro for definition
 * sets up a static/dynamic function entry in a class inheriting
 * from PyObjectBase. Its a pure convenience macro. You can also do
 * it by hand if you want. It looks like that:
 * \code
 * PyObject *PyGetGrp(PyObject *args);
 * static PyObject *sPyGetGrp(PyObject *self, PyObject *args, PyObject *kwd)
 *        {return ((FCPyParametrGrp*)self)->PyGetGrp(args);};
 * \endcode
 * first the method is defined which have the functionality then the
 * static wrapper is used to provide a callback for python. The call
 * is simply mapped to the method.
 * @param CLASS is the class in which the macro take place.
 * @param DFUNC is the object method get defined and called
 * sDFUNC is the static method name (use what you want)
 * @see PYFUNCIMP_D
 * @see PyObjectBase
 */
#define PYFUNCDEF_D(CLASS,DFUNC)	PyObject * DFUNC (PyObject *args);  \
static PyObject * s##DFUNC (PyObject *self, PyObject *args, PyObject * /*kwd*/){return (( CLASS *)self)-> DFUNC (args);};

/** Python dynamic class macro for implementation
 * used to set up an implementation for PYFUNCDEF_D definition.
 * Its a pure convenience macro. You can also do
 * it by hand if you want. It looks like that:
 * \code
 * PyObject *FCPyParametrGrp::PyGetGrp(PyObject *args)
 * \endcode
 * see PYFUNCDEF_D for details * @param CLASS is the class in which the macro take place.
 * @param DFUNC is the object method get defined and called
 * @see PYFUNCDEF_D
 * @see PyObjectBase
 */
#define PYFUNCIMP_D(CLASS,DFUNC) PyObject* CLASS::DFUNC (PyObject *args)



/** Python dynamic class macro for the method list
 * used to fill the method list of a class derived from PyObjectBase.
 * Its a pure convenience macro. You can also do
 * it by hand if you want. It looks like that:
 * \code
 * PyMethodDef DocTypeStdPy::Methods[] = {
 * 	{"AddFeature",    (PyCFunction) sAddFeature,    Py_NEWARGS},
 * 	{"RemoveFeature", (PyCFunction) sRemoveFeature, Py_NEWARGS},
 *	{NULL, NULL}
 * };
 * \endcode
 * instead of:
 * \code
 * PyMethodDef DocTypeStdPy::Methods[] = {
 *	PYMETHODEDEF(AddFeature)
 *	PYMETHODEDEF(RemoveFeature)
 *	{NULL, NULL}
 * };
 * \endcode
 * see PYFUNCDEF_D for details
 * @param FUNC is the object method get defined
 * @see PYFUNCDEF_D
 * @see PyObjectBase
 */
#define PYMETHODEDEF(FUNC)	{"" #FUNC "",(PyCFunction) s##FUNC,Py_NEWARGS},

GA_EXPORT_API extern PyObject* BaseExceptionFreeCADError;
#define PY_FCERROR (BaseExceptionFreeCADError ? \
 BaseExceptionFreeCADError : PyExc_RuntimeError)


/** Exception handling for python callback functions
 * Is a convenience macro to manage the exception handling of python callback
 * functions defined in classes inheriting PyObjectBase and using PYMETHODEDEF .
 * You can automate this:
 * \code
 * PYFUNCIMP_D(DocTypeStdPy,AddFeature)
 * {
 *   char *pstr;
 *   if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
 *      return NULL;                             // NULL triggers exception
 *
 *   try {
 *     Feature *pcFtr = _pcDocTypeStd->AddFeature(pstr);
 *   }catch(...)                                                        \
 *   {                                                                 \
 * 	 	Py_Error(BaseExceptionFreeCADError,"Unknown C++ exception");          \
 *   }catch(FCException e) ..... // and so on....                                                               \
 * }
 * \endcode
 * with that:
 * \code
 * PYFUNCIMP_D(DocTypeStdPy,AddFeature)
 * {
 *   char *pstr;
 *   if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C
 *      return NULL;                             // NULL triggers exception
 *
 *  PY_TRY {
 *    Feature *pcFtr = _pcDocTypeStd->AddFeature(pstr);
 *  }PY_CATCH;
 * }
 * \endcode
 * this catch maps all of the FreeCAD standard exception to a clear output for the
 * Python exception.
 * @see PYMETHODEDEF
 * @see PyObjectBase
 */
#define PY_TRY	try

#ifndef DONT_CATCH_CXX_EXCEPTIONS
/// see docu of PY_TRY
#  define PY_CATCH catch(Exception &e)                        \
    {                                                               \
        std::string str;                                            \
        str += "base exception thrown (";                        \
        str += e.what();                                            \
        str += ")";                                                 \
        e.ReportException();                                        \
        Py_Error(BaseExceptionFreeCADError,str.c_str());      \
    }                                                               \
    catch(std::exception &e)                                        \
    {                                                               \
        std::string str;                                            \
        str += "std exception thrown (";                            \
        str += e.what();                                            \
        str += ")";                                                 \
        acutPrintf(C2W(str.c_str()));                         \
        Py_Error(BaseExceptionFreeCADError, str.c_str());      \
    }                                                               \
    catch(const Py::Exception&)                                     \
    {                                                               \
        return NULL;                                                \
    }                                                               \
    catch(const char *e)                                            \
    {                                                               \
        Py_Error(BaseExceptionFreeCADError, e);                \
    }                                                               \
    catch(...)                                                      \
    {                                                               \
        Py_Error(BaseExceptionFreeCADError,"Unknown C++ exception"); \
    }

#else
/// see docu of PY_TRY
#  define PY_CATCH catch(Exception &e)                        \
    {                                                               \
        std::string str;                                            \
        str += "base exception thrown (";                        \
        str += e.what();                                            \
        str += ")";                                                 \
        e.ReportException();                                        \
        Py_Error(BaseExceptionFreeCADError,str.c_str());      \
    }                                                               \
    catch(std::exception &e)                                        \
    {                                                               \
        std::string str;                                            \
        str += "std exception thrown (";                            \
        str += e.what();                                            \
        str += ")";                                                 \
        acutPrintf(C2W(str.c_str()));                         \
        Py_Error(BaseExceptionFreeCADError,str.c_str());      \
    }                                                               \
    catch(const Py::Exception&)                                     \
    {                                                               \
        return NULL;                                                \
    }                                                               \
    catch(const char *e)                                            \
    {                                                               \
        Py_Error(BaseExceptionFreeCADError,e);                \
    }

#endif  // DONT_CATCH_CXX_EXCEPTIONS

/** Python helper class
 *  This class encapsulate the Decoding of UTF8 to a python object.
 *  Including exception handling.
 */
inline PyObject* PyAsUnicodeObject( const char* str )
{
    // Returns a new reference, don't increment it!
    PyObject* p = PyUnicode_DecodeUTF8( str, strlen( str ), 0 );
    if( !p )
        throw Exception( "UTF8 conversion failure at PyAsUnicodeString()" );
    return p;
}

inline PyObject* PyAsUnicodeObject( const std::string& str )
{
    return PyAsUnicodeObject( str.c_str() );
}


#endif // BASE_PYOBJECTBASE_H