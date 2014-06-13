sectorinfo <- function()
{
  invisible(.Call("sector_info",PACKAGE="rsphere"))
}
sectorls<- function(file)
{
  invisible(.Call("sector_ls",file,PACKAGE="rsphere"))
}
sectordownload<-function(srcfile,dest)
{
  invisible(.Call("sector_download",srcfile,dest,PACKAGE="rsphere"))
}
sectorupload<-function(replica_num,srcfile,dest)
{
invisible(.Call("sector_upload",replica_num,srcfile,dest,PACKAGE="rsphere"))
}

spheremrun<-function(inputd,outputd,output,opfile,opfun)
{
invisible(.Call("SphereMRun",inputd,outputd,output,opfile,opfun,PACKAGE="rsphere"))
}

sphererun<-function(inputd,outputd,output,opfile,opfun)
{
invisible(.Call("SphereRun",inputd,outputd,output,opfile,opfun,PACKAGE="rsphere"))
}




