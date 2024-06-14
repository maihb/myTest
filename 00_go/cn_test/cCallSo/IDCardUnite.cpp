#include <dlfcn.h>
#include <stdio.h>
#include "IDCardUnite.h"

IDCardUnite::IDCardUnite(/* args */)
{
    _bInit = false;
    _iErrorCode = ERROR_IDCARD_MODULE_UNLOAD;
    _hModule = NULL;

    IdcardReadInfo = NULL;
    IdcardModuleInit = NULL;
    IdcardModuleRelease = NULL;
    IdcardGetInfo = NULL;
    IdcardReadStop = NULL;
    IdcardGetLastErrorInfo = NULL;
    getInfo = NULL;
}

IDCardUnite::~IDCardUnite()
{
}

int IDCardUnite::loadDll(const char *name)
{
    // string strDllName = _strDllPath + "/libs/" + _strModuleName;

    // string strDllName = _strModulePath + "/" + _strModuleName;

    if (!_hModule)
    {
        _hModule = dlopen(name, RTLD_LAZY);
        if (_hModule == NULL)
        {
            printf("dlopen err, dll name=%s", name);
            return ERROR_IDCARD_MODULE_UNLOAD;
        }

        setInfo = (pSetInfo)dlsym(_hModule, "setInfo");
        if (!setInfo)
        {
            printf("dlsym setInfo err!\n");
            unLoadDll();
            return ERROR_IDCARD_MODULE_UNLOAD;
        }
        getInfo = (pGetInfo)dlsym(_hModule, "getInfo");
        if (!getInfo)
        {
            printf("dlsym getInfo err!\n");
            unLoadDll();
            return ERROR_IDCARD_MODULE_UNLOAD;
        }
    }

    return IDCARD_READ_OK;
}

void IDCardUnite::unLoadDll()
{
    if (_hModule)
    {
        int iret = dlclose(_hModule);
        // printf("IDCardUnite::unLoadDll iret=%d, pSetInfo=%p pGetInfo=%p\n", iret, setInfo, getInfo);
        _hModule = NULL;
    }
}

int IDCardUnite::Init(const string strPwd, const char *name)
{
    _strAppPath = strPwd;
    _iErrorCode = loadDll(name);
    if (_iErrorCode == IDCARD_READ_OK)
    {
        //_iErrorCode = IdcardModuleInit(_strAppPath.c_str(), _strModulePath.c_str());
    }
    if (_iErrorCode != IDCARD_READ_OK)
    {
        printf("Init err = %d", _iErrorCode);
    }

    return _iErrorCode;
}

void IDCardUnite::UnInit()
{
    if (_hModule)
    {
        // IdcardModuleRelease();
        unLoadDll();
    }
}

int IDCardUnite::IDCardRead(int timeout)
{
    _iErrorCode = IdcardReadInfo(timeout);

    if (_iErrorCode == IDCARD_READ_OK)
    {
        _strCardInfo = IdcardGetInfo();
        return _iErrorCode;
    }

    _strErrorInfo = IdcardGetLastErrorInfo();
    printf("IDCardRead errcode = %d; err info = %s", _iErrorCode, _strErrorInfo.c_str());

    return _iErrorCode;
}

void IDCardUnite::IDCardStop()
{
    if (!_hModule)
    {
        return;
    }

    IdcardReadStop();
}
