# AMSKY01 Weather Station Driver
set(AMSKY01_VERSION_MAJOR 1)
set(AMSKY01_VERSION_MINOR 2)

# Source files
set(AMSKY01_SOURCES
    amsky01.cpp
)

# Add executable
add_executable(indi_amsky01 ${AMSKY01_SOURCES})

# Set include directories
target_include_directories(indi_amsky01 PRIVATE
    /usr/include/libindi
)

# Link libraries
target_link_libraries(indi_amsky01 
    indidriver
    indiclient
    pthread
)

# Install
install(TARGETS indi_amsky01 RUNTIME DESTINATION /usr/bin)

# Install XML file
install(FILES indi_amsky01.xml DESTINATION /usr/share/indi)
