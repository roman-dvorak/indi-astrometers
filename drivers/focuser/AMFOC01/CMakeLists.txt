# AMFOC01 Driver
set(AMFOC01_VERSION_MAJOR 1)
set(AMFOC01_VERSION_MINOR 0)

# Source files
set(AMFOC01_SOURCES
    amfoc01.cpp
)

# Add executable
add_executable(indi_amfoc01 ${AMFOC01_SOURCES})

# Set include directories
target_include_directories(indi_amfoc01 PRIVATE
    /usr/include/libindi
)

# Link libraries directly
target_link_libraries(indi_amfoc01 
    indidriver
    indiclient
    XISF
    pthread
)

# Install
install(TARGETS indi_amfoc01 RUNTIME DESTINATION bin)

# Install XML file
install(FILES indi_amfoc01.xml DESTINATION /usr/share/indi)
