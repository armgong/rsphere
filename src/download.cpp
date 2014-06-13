/*****************************************************************************
Copyright 2005 - 2011 The Board of Trustees of the University of Illinois.

Licensed under the Apache License, Version 2.0 (the "License"); you may not
use this file except in compliance with the License. You may obtain a copy of
the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.
*****************************************************************************/

/*****************************************************************************
written by
   Yunhong Gu, last updated 01/12/2010
modify by 
   Yu Gong For R client
*****************************************************************************/

#ifdef WIN32
   #include <windows.h>
#else
   #include <unistd.h>
   #include <sys/ioctl.h>
   #include <sys/time.h>
   #include <sys/stat.h>
   #include <sys/types.h>
#endif

#include <osportable.h>
#include <fstream>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <sector.h>

using namespace std;


int download(const char* file, const char* dest, Sector& client, bool encryption)
{
   #ifndef WIN32
      timeval t1, t2;
   #else
      DWORD t1, t2;
   #endif

   #ifndef WIN32
      gettimeofday(&t1, 0);
   #else
      t1 = GetTickCount();
   #endif

   SNode attr;
   if (client.stat(file, attr) < 0)
   {
      cerr << "ERROR: cannot locate file " << file << endl;
      return -1;
   }

   if (attr.m_bIsDir)
   {
      ::mkdir((string(dest) + "/" + file).c_str(), S_IRWXU);
      return 1;
   }

   const long long int size = attr.m_llSize;
   cout << "downloading " << file << " of " << size << " bytes" << endl;

   SectorFile* f = client.createSectorFile();

   int mode = SF_MODE::READ;
   if (encryption)
      mode |= SF_MODE::SECURE;

   if (f->open(file, mode) < 0)
   {
      cerr << "unable to locate file " << file << endl;
      return -1;
   }

   int sn = strlen(file) - 1;
   for (; sn >= 0; sn --)
   {
      if (file[sn] == '/')
         break;
   }
   string localpath;
   if (dest[strlen(dest) - 1] != '/')
      localpath = string(dest) + string("/") + string(file + sn + 1);
   else
      localpath = string(dest) + string(file + sn + 1);

   int64_t result = f->download(localpath.c_str(), true);

   f->close();
   client.releaseSectorFile(f);

   if (result >= 0)
   {
      float throughput = 0.0;
      #ifndef WIN32
         gettimeofday(&t2, 0);
         float span = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0;
      #else
         float span = (GetTickCount() - t1) / 1000.0;
      #endif
      if (span > 0.0)
         throughput = result * 8.0 / 1000000.0 / span;

      cout << "Downloading accomplished! " << "AVG speed " << throughput << " Mb/s." << endl << endl ;

      return 0;
   }

   cerr << "error happened during downloading " << file << endl;
   Utility::print_error(result);

   return -1;
}

int _sector_download(const char* srcfile, const char* dest)
{


   Sector client;
   if (Utility::login(client) < 0)
      return -1;
   if (download(srcfile,dest, client, false) < 0)
    {
     int64_t availdisk = 0;
     LocalFS::get_dir_space(dest, availdisk);
     if (availdisk <= 0)
      {
       cerr << "insufficient local disk space. quit.\n";
               Utility::logout(client);
               return -1;
      }
   }
   Utility::logout(client);
   return 0;
}
