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
   Yunhong Gu, last updated 03/15/2011
modify by 
   Yu Gong for R client
*****************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <osportable.h>
#include <sector.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#ifndef WIN32
   #include <sys/types.h>
   #include <sys/stat.h>
#endif

using namespace std;


int upload(const char* file, const char* dst, Sector& client, const int rep_num, const string& ip, const string& cid, const bool secure)
{
   //check if file already exists

   SNode s;
   if (LocalFS::stat(file, s) < 0)
   {
      cout << "cannot locate source file " << file << endl;
      return -1;
   }

   SNode attr;
   if (client.stat(dst, attr) >= 0)
   {
      if (attr.m_llSize == s.m_llSize)
      {
         cout << "destination file " << dst << " exists on Sector FS. skip.\n";
         return 0;
      }
   }

   cout << "uploading " << file << " of " << s.m_llSize << " bytes" << endl;

   timeval t1, t2;
   gettimeofday(&t1, 0);

   SectorFile* f = client.createSectorFile();

   SF_OPT option;
   option.m_llReservedSize = s.m_llSize;
   if (option.m_llReservedSize <= 0)
      option.m_llReservedSize = 1;
   option.m_iReplicaNum = rep_num;
   option.m_strHintIP = ip;
   option.m_strCluster = cid;

   int mode = SF_MODE::WRITE | SF_MODE::TRUNC;
   if (secure)
      mode |= SF_MODE::SECURE;

   int r = f->open(dst, mode, &option);
   if (r < 0)
   {
      cerr << "unable to open file " << dst << endl;
      Utility::print_error(r);
      return -1;
   }

   int64_t result = f->upload(file);

   f->close();
   client.releaseSectorFile(f);

   if (result >= 0)
   {
      gettimeofday(&t2, 0);
      float throughput = s.m_llSize * 8.0 / 1000000.0 / ((t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0);

      cout << "Uploading accomplished! " << "AVG speed " << throughput << " Mb/s." << endl << endl ;
   }
   else
   {
      cout << "Uploading failed! Please retry. " << endl << endl;
      Utility::print_error(result);
      return result;
   }

   return 0;
}

int _sector_upload(int replica_num,const char* srcfile,const char* dest)
{

   string ip = "";
   string cluster = "";
   Sector client;
   if (Utility::login(client) < 0)
      return -1;
   int result = upload(srcfile,dest,client, replica_num, ip, cluster,false);
   if (result < 0)
   {
    client.remove(dest);
    Utility::logout(client);
    return -1;
   }
   Utility::logout(client);
   return 0;
}
