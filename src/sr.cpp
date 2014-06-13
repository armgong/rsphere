#include <sector.h>
#include <sys/time.h>
#include <iostream>

using namespace std;

int sr(const char* input,const char* outputdir,const char* outputp,const char* opfile,const char* opfunc)
{

   string  inputs(input);
   string  outputdirs(outputdir);
   string  outputs(outputp);
   string  opfiles(opfile);
   string  opfuncs(opfunc);

   Sector client;
   if (Utility::login(client) < 0)
      return -1;

   vector<string> files;
   files.insert(files.end(), inputs);

   SphereStream s;
   if (s.init(files) < 0)
   {
      cout << "unable to locate input data files. quit.\n";
      Utility::logout(client);
      return -1;
   }

   SphereStream temp;
   temp.setOutputPath(outputdirs, outputs);
   temp.init(256);

   SphereProcess* myproc = client.createSphereProcess();

   if (myproc->loadOperator(opfile) < 0)
   {
      cout << "cannot find  op so\n";
      Utility::logout(client);
      return -1;
   }

   timeval t;
   gettimeofday(&t, 0);
   cout << "start run task" << t.tv_sec << endl;

   int result = myproc->run(s, temp, opfuncs, 0);
   if (result < 0)
   {
      Utility::print_error(result);
      Utility::logout(client);
      return -1;
   }

   myproc->waitForCompletion();
   timeval t3;
   gettimeofday(&t3, 0);
   cout << "COMPLETED " <<t3.tv_sec-t.tv_sec<<" Sec"<< endl;
   myproc->close();
   client.releaseSphereProcess(myproc);

   Utility::logout(client);

   return 0;
}
