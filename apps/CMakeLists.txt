# apps/CMakeLists.txt

# Create the executable stat_app if the option is enabled
if (BUILD_LIB_DATAFRAME)
    add_executable(stat_app
        stat_app.cpp
    )

    # Link the dataframe shared library to stat_app
    target_link_libraries(stat_app PRIVATE DataFrame)
endif()

# Create the executable interpol_app if the option is enabled
if (BUILD_LIB_INTERPOLATION)
    add_executable(interpol_app
        interpol_app.cpp
    )

    # Link the dataframe shared library to stat_app
    target_link_libraries(interpol_app PRIVATE Interpolation)
endif()
