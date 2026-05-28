set(RIBDUMP_SOURCES
    ribdump.c
)

add_executable(ribdump ${RIBDUMP_SOURCES})
target_include_directories(ribdump PRIVATE .)

# Utilities mentioned in Makefile.unix
set(UTILS
    bin2hex
    bin2oct
    dec2hex
    dbl2oct
    flt2oct
    hex2bin
    hex2dec
    oct2bin
    str2oct
)

foreach(UTIL ${UTILS})
    add_executable(${UTIL} ${UTIL}.c)
endforeach()
