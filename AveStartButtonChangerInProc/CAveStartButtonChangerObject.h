// CAveStartButtonChangerObject.h : Declaration of the CCAveStartButtonChangerObject

#pragma once
#include "resource.h"       // main symbols

#include "AveStartButtonChangerInProc.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CCAveStartButtonChangerObject

class ATL_NO_VTABLE CCAveStartButtonChangerObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCAveStartButtonChangerObject, &CLSID_CAveStartButtonChangerObject>,
	public IDispatchImpl<ICAveStartButtonChangerObject, &IID_ICAveStartButtonChangerObject, &LIBID_AveStartButtonChangerInProcLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CCAveStartButtonChangerObject()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CAVESTARTBUTTONCHANGEROBJECT)


BEGIN_COM_MAP(CCAveStartButtonChangerObject)
	COM_INTERFACE_ENTRY(ICAveStartButtonChangerObject)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

};

OBJECT_ENTRY_AUTO(__uuidof(CAveStartButtonChangerObject), CCAveStartButtonChangerObject)
