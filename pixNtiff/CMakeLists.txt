find_package(SDL2 REQUIRED)

# Common sources for many utilities
set(COMMON_SRCS bitmap.c)

# Helper for creating utilities with common objects
macro(add_pixutil name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} TIFF::TIFF JPEG::JPEG ${LZMA_LIBRARY} ${ZSTD_LIBRARY} ZLIB::ZLIB m)
    target_include_directories(${name} PRIVATE . ${CMAKE_SOURCE_DIR}/include)
endmacro()

# Utilities
add_pixutil(pix2tiff pix2tiff.c rpix.c wtiff.c ${COMMON_SRCS})
add_pixutil(tiff2pix tiff2pix.c rtiff.c wpix.c orient.c ${COMMON_SRCS})
add_pixutil(tif2tif tif2tif.c rtiff.c wtiff.c ${COMMON_SRCS})
add_pixutil(tifftile tifftile.c rtiff.c wtiff.c orient.c ${COMMON_SRCS})
add_pixutil(tiffinvert tiffinvert.c rtiff.c wtiff.c orient.c ${COMMON_SRCS})
add_pixutil(tifforient tifforient.c rtiff.c wtiff.c orient.c ${COMMON_SRCS})

# IFF utilities (don't necessarily need TIFF but use bitmap.c)
add_executable(iff2iff iff2iff.c riff.c wiff.c ${COMMON_SRCS})
target_link_libraries(iff2iff m)

add_pixutil(iff2tiff iff2tiff.c riff.c wtiff.c orient.c ${COMMON_SRCS})
add_pixutil(tiff2iff tiff2iff.c rtiff.c wiff.c orient.c ${COMMON_SRCS})

add_executable(pixinfo pixinfo.c)
add_executable(cininfo cininfo.c)
add_executable(iffinfo iffinfo.c)

# showbits needs SDL2
if(SDL2_FOUND)
    add_executable(showbits showbits.c rpix.c rtiff.c rtga.c riff.c orient.c ${COMMON_SRCS})
    target_link_libraries(showbits TIFF::TIFF JPEG::JPEG ${LZMA_LIBRARY} ${ZSTD_LIBRARY} ZLIB::ZLIB SDL2::SDL2 m)
    target_include_directories(showbits PRIVATE . ${CMAKE_SOURCE_DIR}/include)
endif()

add_pixutil(tiff2X tiff2X.c rtiff.c wX.c orient.c ${COMMON_SRCS})

add_executable(tgainfo tgainfo.c)

add_pixutil(tga2tiff tga2tiff.c rtga.c wtiff.c orient.c ${COMMON_SRCS})
add_pixutil(tiff2tga tiff2tga.c rtiff.c wtga.c orient.c ${COMMON_SRCS})

# tga2tga uses bitmapm.o (bitmap.c compiled without LIBTIFF)
add_executable(tga2tga tga2tga.c rtga.c wtga.c bitmap.c)
target_compile_definitions(tga2tga PRIVATE BITMAP_NO_TIFF) # Assuming this flag exists to disable TIFF in bitmap.c

add_pixutil(z2tiff z2tiff.c rz.c wtiff.c ${COMMON_SRCS})
add_pixutil(tiff2fp tiff2fp.c rtiff.c orient.c ${COMMON_SRCS})

add_executable(iff2fp iff2fp.c riff.c orient.c ${COMMON_SRCS})
target_link_libraries(iff2fp m)

add_pixutil(fp2tiff fp2tiff.c wtiff.c ${COMMON_SRCS})
add_pixutil(normfptiff normfptiff.c rtiff.c wtiff.c ${COMMON_SRCS})
add_pixutil(tiffedge tiffedge.c rtiff.c wtiff.c orient.c ${COMMON_SRCS})
add_pixutil(busybits busybits.c wtiff.c ${COMMON_SRCS})
