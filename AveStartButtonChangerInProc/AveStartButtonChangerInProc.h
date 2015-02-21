

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Nov 21 20:18:14 2009
 */
/* Compiler settings for .\AveStartButtonChangerInProc.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __AveStartButtonChangerInProc_h__
#define __AveStartButtonChangerInProc_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICAveStartButtonChangerObject_FWD_DEFINED__
#define __ICAveStartButtonChangerObject_FWD_DEFINED__
typedef interface ICAveStartButtonChangerObject ICAveStartButtonChangerObject;
#endif 	/* __ICAveStartButtonChangerObject_FWD_DEFINED__ */


#ifndef __CAveStartButtonChangerObject_FWD_DEFINED__
#define __CAveStartButtonChangerObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class CAveStartButtonChangerObject CAveStartButtonChangerObject;
#else
typedef struct CAveStartButtonChangerObject CAveStartButtonChangerObject;
#endif /* __cplusplus */

#endif 	/* __CAveStartButtonChangerObject_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ICAveStartButtonChangerObject_INTERFACE_DEFINED__
#define __ICAveStartButtonChangerObject_INTERFACE_DEFINED__

/* interface ICAveStartButtonChangerObject */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICAveStartButtonChangerObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CE65D9CE-FB92-4FB8-823A-B080FAC3C999")
    ICAveStartButtonChangerObject : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ICAveStartButtonChangerObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICAveStartButtonChangerObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICAveStartButtonChangerObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICAveStartButtonChangerObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICAveStartButtonChangerObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICAveStartButtonChangerObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICAveStartButtonChangerObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICAveStartButtonChangerObject * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ICAveStartButtonChangerObjectVtbl;

    interface ICAveStartButtonChangerObject
    {
        CONST_VTBL struct ICAveStartButtonChangerObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICAveStartButtonChangerObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICAveStartButtonChangerObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICAveStartButtonChangerObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICAveStartButtonChangerObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICAveStartButtonChangerObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICAveStartButtonChangerObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICAveStartButtonChangerObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICAveStartButtonChangerObject_INTERFACE_DEFINED__ */



#ifndef __AveStartButtonChangerInProcLib_LIBRARY_DEFINED__
#define __AveStartButtonChangerInProcLib_LIBRARY_DEFINED__

/* library AveStartButtonChangerInProcLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_AveStartButtonChangerInProcLib;

EXTERN_C const CLSID CLSID_CAveStartButtonChangerObject;

#ifdef __cplusplus

class DECLSPEC_UUID("F791A188-699D-4FD4-955A-EB59E89B1907")
CAveStartButtonChangerObject;
#endif
#endif /* __AveStartButtonChangerInProcLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


