foreach(tool mac2unix ibm2unix untab)
    add_executable(${tool} ${tool}.c)
endforeach()

install(TARGETS mac2unix ibm2unix untab RUNTIME DESTINATION bin)
