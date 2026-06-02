set(RDROBJS
    readrib.c
    streamio.c
    paraml.c
    seterror.c
    vcache.c
    readfunc.c
    hintfunc.c
    ribfunc.c
    declare.c
    ribtbl.c
    hinttbl.c
)

set(TABLEOBJS ritable.c)
set(ERROROBJS error.c)

# libribrdr
add_affine_library(ribrdr 
    ${RDROBJS} 
    ${TABLEOBJS} 
    ${ERROROBJS}
)
affine_target_link_libraries(ribrdr PUBLIC ribhash sribw ZLIB::ZLIB)

# libribtable
add_affine_library(ribtable ${TABLEOBJS})

# libriberror
add_affine_library(riberror ${ERROROBJS})

# libsribrdr
add_affine_library(sribrdr ${RDROBJS})
affine_target_link_libraries(sribrdr PUBLIC ribhash sribw ZLIB::ZLIB)
