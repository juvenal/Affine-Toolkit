# All pixNtiff tools require libtiff (checked in root CMakeLists.txt)
include_directories(${TIFF_INCLUDE_DIRS})

# ── Support object library ─────────────────────────────────────────────────────
# bitmap.c is compiled twice: once with LIBTIFF defined and once without
add_library(bitmap_tiff OBJECT bitmap.c)
target_compile_definitions(bitmap_tiff PRIVATE LIBTIFF)

add_library(bitmap_plain OBJECT bitmap.c)

# ── Helper macro: build one executable from named sources + support objects ───
macro(pix_exe name)
    add_executable(${name} ${name}.c ${ARGN})
    target_link_libraries(${name} PRIVATE ${TIFF_LIBRARIES} m)
endmacro()

# TIFF conversion tools
pix_exe(pix2tiff rpix.c wtiff.c $<TARGET_OBJECTS:bitmap_plain>)
pix_exe(tiff2pix rtiff.c wpix.c $<TARGET_OBJECTS:bitmap_tiff>)
pix_exe(tif2tif  rtiff.c wtiff.c $<TARGET_OBJECTS:bitmap_tiff>)

# TGA conversion tools
pix_exe(tga2tga  rtga.c wtga.c $<TARGET_OBJECTS:bitmap_plain>)
pix_exe(tga2tiff rtga.c wtiff.c $<TARGET_OBJECTS:bitmap_plain>)
pix_exe(tiff2tga rtiff.c wtga.c $<TARGET_OBJECTS:bitmap_tiff>)

# IFF conversion tools
pix_exe(iff2iff  riff.c wiff.c $<TARGET_OBJECTS:bitmap_plain>)
pix_exe(iff2tiff riff.c wtiff.c $<TARGET_OBJECTS:bitmap_plain>)
pix_exe(tiff2iff rtiff.c wiff.c $<TARGET_OBJECTS:bitmap_tiff>)

# Z (RLE/compressed PIX) tools
pix_exe(z2tiff   rz.c wtiff.c $<TARGET_OBJECTS:bitmap_plain>)

# Info tools
pix_exe(pixinfo  rpix.c $<TARGET_OBJECTS:bitmap_plain>)
pix_exe(tgainfo  rtga.c $<TARGET_OBJECTS:bitmap_plain>)
pix_exe(iffinfo  riff.c $<TARGET_OBJECTS:bitmap_plain>)
pix_exe(cininfo  $<TARGET_OBJECTS:bitmap_plain>)

# Floating-point TIFF tools
pix_exe(tiff2fp  rtiff.c $<TARGET_OBJECTS:bitmap_tiff>)
pix_exe(fp2tiff  wtiff.c $<TARGET_OBJECTS:bitmap_plain>)
pix_exe(normfptiff $<TARGET_OBJECTS:bitmap_plain>)
pix_exe(iff2fp   riff.c $<TARGET_OBJECTS:bitmap_plain>)

# Image processing tools
pix_exe(tiffedge  rtiff.c wtiff.c $<TARGET_OBJECTS:bitmap_tiff>)
pix_exe(tifftile  rtiff.c wtiff.c $<TARGET_OBJECTS:bitmap_tiff>)
pix_exe(tiffinvert rtiff.c wtiff.c $<TARGET_OBJECTS:bitmap_tiff>)
pix_exe(tifforient rtiff.c wtiff.c orient.c $<TARGET_OBJECTS:bitmap_tiff>)
pix_exe(busybits  $<TARGET_OBJECTS:bitmap_plain>)

# X11 / OpenGL display tools
find_package(X11)
if(X11_FOUND)
    pix_exe(tiff2X rtiff.c wX.c $<TARGET_OBJECTS:bitmap_tiff>)
    target_link_libraries(tiff2X PRIVATE ${X11_LIBRARIES})

    pix_exe(showbits rtiff.c wX.c $<TARGET_OBJECTS:bitmap_tiff>)
    target_link_libraries(showbits PRIVATE ${X11_LIBRARIES})
endif()

# Installation
set(PIX_TOOLS pix2tiff tiff2pix tif2tif tga2tga tga2tiff tiff2tga
    iff2iff iff2tiff tiff2iff z2tiff pixinfo tgainfo iffinfo cininfo
    tiff2fp fp2tiff normfptiff iff2fp tiffedge tifftile tiffinvert
    tifforient busybits)
if(X11_FOUND)
    list(APPEND PIX_TOOLS tiff2X showbits)
endif()

install(TARGETS ${PIX_TOOLS} RUNTIME DESTINATION bin)
