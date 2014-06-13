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
   Yunhong Gu, last updated 03/20/2011

modify by Yu Gong For R client,Last update 2014/06/06
*****************************************************************************/

#ifndef WIN32
   #include <arpa/inet.h>
   #include <sys/socket.h>
   #include <netdb.h>
#else
   #include <winsock2.h>
   #include <ws2tcpip.h>
   #include <time.h>
#endif
#include <sector.h>
#include <iostream>
#include <cstring>

using namespace std;

string sectorformat(const int64_t& val)
{
   string fmt_val = "";

   int64_t left = val;
   while (left > 0)
   {
      int section = left % 1000;
      left = left / 1000;

      char buf[8];
      if (left > 0)
         sprintf(buf, "%03d", section);
      else
         sprintf(buf, "%d", section);

      if (fmt_val.c_str()[0] == 0)
         fmt_val = buf;
      else
         fmt_val = string(buf) + "," + fmt_val;
   }

   // nothing left, assign 0
   if (fmt_val.c_str()[0] == 0)
      fmt_val = "0";

   return fmt_val;
}

string sectorformat(const string& str, const int len)
{
   string fmt_str = str;

   for (int i = fmt_str.length(); i < len; ++ i)
      fmt_str += " ";

   return fmt_str;
}

string toString(const int64_t& val)
{
   char buf[64];
   sprintf(buf, "%lld", (long long)val);

   return buf;
}

string sectorformat(const int64_t& val, const int len)
{
   return sectorformat(toString(val), len);
}

string sectorformatStatus(const int status, const int len)
{
   string info = "Unknown";

   switch (status)
   {
   case 0:
      info = "Down";
      break;
   case 1:
      info = "Normal";
      break;
   case 2:
      info = "DiskFull";
      break;
   case 3:
      return "Error";
      break;
   }

   if (len <= 0)
      return info;

   if (info.length() > unsigned(len))
      return info.substr(0, len);
   else
      info.append(len - info.length(), ' ');

   return info;
}

string sectorformatSize(const int64_t& size)
{
   if (size <= 0)
      return "0";

   double k = 1000.;
   double m = 1000 * k;
   double g = 1000 * m;
   double t = 1000 * g;
   double p = 1000 * t;

   char sizestr[64];

   if (size < k)
      sprintf(sizestr, "%lld B", (long long int)size);
   else if (size < m)
      sprintf(sizestr, "%.3f KB", size / k);
   else if (size < g)
      sprintf(sizestr, "%.3f MB", size / m);
   else if (size < t)
      sprintf(sizestr, "%.3f GB", size / g);
   else if (size < p)
      sprintf(sizestr, "%.3f TB", size / t);
   else
      sprintf(sizestr, "%.3f PB", size / p);

   return sizestr;
}

string getDNSName(const string& ip)
{
   struct addrinfo hints, *peer;

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags = AI_PASSIVE;
   hints.ai_family = AF_INET;

   if (0 != getaddrinfo(ip.c_str(), NULL, &hints, &peer))
      return ip;

   char clienthost[NI_MAXHOST];
   if (getnameinfo(peer->ai_addr, peer->ai_addrlen, clienthost, sizeof(clienthost), NULL, 0, NI_NAMEREQD) < 0)
      return ip;

   freeaddrinfo(peer);

   return clienthost;
}

// TODO: create a new util class that provides the sectorformat routines above
// move the print function as a member function of sysstat

void print_sysinfo(const SysStat& s, bool address = false)
{
   cout << "Sector System Insectorformation:" << endl;
   cout << "Available Disk Size:         " << sectorformatSize(s.m_llAvailDiskSpace)<< endl;
   cout << "Total File Size:             " << sectorformatSize(s.m_llTotalFileSize) << endl;
   cout << "Total Number of Files:       " << s.m_llTotalFileNum << " (" << s.m_llUnderReplicated << " under replicated)" << endl;
   cout << "Total Number of Slave Nodes: " << s.m_llTotalSlaves;

   vector<int> slave_count(4);
   fill(slave_count.begin(), slave_count.end(), 0);
   for (vector<SysStat::SlaveStat>::const_iterator i = s.m_vSlaveList.begin(); i != s.m_vSlaveList.end(); ++ i)
   {
      if ((i->m_iStatus >= 0) && (i->m_iStatus < 4))
         slave_count[i->m_iStatus] ++;
   }
   cout << " (" << slave_count[1] << " Normal " << slave_count[0] << " Down " << slave_count[2] << " DiskFull " << slave_count[3] << " Error)\n";


   int total_cluster = 0;
   for (vector<SysStat::ClusterStat>::const_iterator i = s.m_vCluster.begin(); i != s.m_vCluster.end(); ++ i)
   {
      if (i->m_iTotalNodes > 0)
         ++ total_cluster;
   }

   cout << sectorformat("SLAVE_ID", 11)
        << sectorformat("Address", 24)
        << sectorformat("STATUS", 10)
        << sectorformat("AvailDisk", 12)
        << sectorformat("TotalFile", 12)
        << sectorformat("Memory", 12)
        << endl;

   for (vector<SysStat::SlaveStat>::const_iterator i = s.m_vSlaveList.begin(); i != s.m_vSlaveList.end(); ++ i)
   {
      cout << sectorformat(i->m_iID, 11)
           << sectorformat(i->m_strIP + ":" + toString(i->m_iPort) , 24)
           << sectorformatStatus(i->m_iStatus, 10)
           << sectorformat(sectorformatSize(i->m_llAvailDiskSpace), 12)
           << sectorformat(sectorformatSize(i->m_llTotalFileSize), 12)
           << sectorformat(sectorformatSize(i->m_llCurrMemUsed), 12)
           << endl;

      if (!address)
         continue;

      cout << sectorformat("", 10)
           << getDNSName(i->m_strIP) << ":"
           << i->m_strDataDir << endl;
   }
}

