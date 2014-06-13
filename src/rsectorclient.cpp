#include <R.h>
#include <Rinternals.h>
#define rlength length
#undef length
#include <sector.h>
#include <iostream>
#include "sysinfo.hpp"
#include "ls.hpp"
#include "download.hpp"
#include "upload.hpp"
#include "sr.hpp"
#include "mr.hpp"
using namespace std;
extern "C"
{
SEXP sector_info()
{
    cout << SectorVersionString << endl;
    Sector client;
    if (Utility::login(client) < 0)
      return R_NilValue ;
    int result = 0;
    SysStat sys;
    result = client.sysinfo(sys);
    if (result >= 0)
     print_sysinfo(sys, false);
   Utility::logout(client);

 return R_NilValue;
}

SEXP sector_ls(SEXP file)
{
 const char* s= CHAR(STRING_ELT(file, 0));
 int ret=_sector_ls(s);
 return R_NilValue;
}

SEXP sector_upload(SEXP nums,SEXP srcfile,SEXP dest)
{
 const char* s= CHAR(STRING_ELT(srcfile, 0));
 const char* t= CHAR(STRING_ELT(dest, 0));
 int replica_num=(int)REAL(nums)[0];
 int ret=_sector_upload(replica_num,s,t);

 return R_NilValue;
}

SEXP sector_download(SEXP srcfile,SEXP dest)
{
 const char* s= CHAR(STRING_ELT(srcfile, 0));
 const char* t= CHAR(STRING_ELT(dest, 0));
 int ret=_sector_download(s,t);
 
 return R_NilValue;
}

SEXP SphereMRun(SEXP inputd,SEXP outputd,SEXP outpu,SEXP opfil,SEXP opfun)
{
 const char* inputdir= CHAR(STRING_ELT(inputd, 0));
 const char* outputdir= CHAR(STRING_ELT(outputd, 0));
 const char* output= CHAR(STRING_ELT(outpu, 0));
 const char* opfile= CHAR(STRING_ELT(opfil, 0));
 const char* opfunc= CHAR(STRING_ELT(opfun, 0));
 int ret=mr(inputdir,outputdir,output,opfile,opfunc);
 return R_NilValue;
}

SEXP SphereRun(SEXP inputd,SEXP outputd,SEXP outpu,SEXP opfil,SEXP opfun)
{
 const char* inputdir= CHAR(STRING_ELT(inputd, 0));
 const char* outputdir= CHAR(STRING_ELT(outputd, 0));
 const char* output= CHAR(STRING_ELT(outpu, 0));
 const char* opfile= CHAR(STRING_ELT(opfil, 0));
 const char* opfunc= CHAR(STRING_ELT(opfun, 0));
 int ret=sr(inputdir,outputdir,output,opfile,opfunc);
  return R_NilValue;
}

}
