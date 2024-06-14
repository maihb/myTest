#ifndef __IDCARDUNITE_H_
#define __IDCARDUNITE_H_

#define IDCARD_READ_OK 0
#define ERROR_IDCARD_MODULE_UNLOAD 99

#include <string>
using namespace std;

typedef int (*pIdcardModuleInit)(const char *pszAppPath, const char *pszModulePath);
typedef void (*pIdcardModuleRelease)();
typedef int (*pIdcardReadInfo)(int timeout);
typedef void (*pIdcardReadStop)();
typedef const char *(*pIdcardGetInfo)();
typedef const char *(*pIdcardGetLastErrorInfo)();

typedef int (*pSetInfo)(const char *key, const char *value);
typedef const char *(*pGetInfo)(const char *key);

class IDCardUnite
{
private:
    int loadDll(const char *name);
    void unLoadDll();

    /* data */
    string _strAppPath;
    string _strModulePath;
    string _strModuleName;
    string _strCardInfo;
    string _strErrorInfo;
    int _iErrorCode;
    bool _bInit;
    void *_hModule;

    pIdcardModuleInit IdcardModuleInit;
    pIdcardModuleRelease IdcardModuleRelease;
    pIdcardReadInfo IdcardReadInfo;
    pIdcardReadStop IdcardReadStop;
    pIdcardGetInfo IdcardGetInfo;
    pIdcardGetLastErrorInfo IdcardGetLastErrorInfo;

public:
    IDCardUnite(/* args */);
    ~IDCardUnite();
    pSetInfo setInfo;
    pGetInfo getInfo;

    int Init(const string strPwd, const char *name);
    void UnInit();

    int IDCardRead(int timeout);
    void IDCardStop();

    string StrCardInfo() const { return _strCardInfo; }
    string StrErrorInfo() const { return _strErrorInfo; }
    int ErrorCode() const { return _iErrorCode; }
};

#endif /*__IDCARDUNITE_H_*/
