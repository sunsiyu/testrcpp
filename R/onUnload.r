.onUnload <- function (libpath){
  library.dynam.unload("testrcpp", libpath)
}