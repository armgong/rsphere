.onLoad <- function(libname, pkgname)
{
   library.dynam("rsphere",pkgname,libname,local=FALSE)

}

.onUnload <- function(libpath)
 {
 library.dynam.unload("rsphere", libpath)
}
