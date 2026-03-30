//#ifndef CREATELINK_H
//#define CREATELINK_H
//#include "windows.h"
//#include "winnls.h"
//#include "shobjidl.h"
//#include "objbase.h"
//#include "objidl.h"
//#include "shlguid.h"

//HRESULT CreateLink(LPCWSTR lpszPathObj,LPCWSTR lpszArgsObj, LPCSTR lpszPathLink, LPCWSTR pszIconfile)
//{
//    HRESULT hres;
//    IShellLink* psl;

//    CoInitialize(NULL);

//    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
//    // has already been called.
//    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
//    if (SUCCEEDED(hres))
//    {
//        IPersistFile* ppf;

//        // Set the path to the shortcut target and add the description.
//        psl->SetPath(lpszPathObj);
//        psl->SetArguments(lpszArgsObj);
////        psl->SetDescription(lpszDesc);
//        psl->SetIconLocation(pszIconfile,0);

//        // Query IShellLink for the IPersistFile interface, used for saving the
//        // shortcut in persistent storage.
//        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

//        if (SUCCEEDED(hres))
//        {
//            WCHAR wsz[MAX_PATH];

//            // Ensure that the string is Unicode.
//            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);

//            // Add code here to check return value from MultiByteWideChar
//            // for success.

//            // Save the link by calling IPersistFile::Save.
//            hres = ppf->Save(wsz, TRUE);
//            ppf->Release();
//        }
//        psl->Release();
//    }
//    CoUninitialize();
//    return hres;
//}
//#endif // CREATELINK_H
