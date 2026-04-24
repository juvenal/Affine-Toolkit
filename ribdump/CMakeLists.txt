foreach(tool
    ribdump
    oct2bin bin2oct
    hex2bin bin2hex
    dbl2oct flt2oct
    dec2hex hex2dec
    str2oct)
    add_executable(${tool} ${tool}.c)
endforeach()

install(TARGETS ribdump oct2bin bin2oct hex2bin bin2hex dbl2oct flt2oct dec2hex hex2dec str2oct
    RUNTIME DESTINATION bin)
