cmake_minimum_required(VERSION 3.25)
project(LD54)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    add_executable(LD54 src/main.cpp)

    target_link_options(LD54 PRIVATE -static-libstdc++ -mwindows -static -lstdc++ -lpthread)
    target_include_directories(LD54 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/include")
    target_link_libraries(LD54 PRIVATE ${CMAKE_SOURCE_DIR}/vendor/lib/SDL2.dll)
    add_custom_command(TARGET LD54 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/LD54-windows"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/LD54-windows/assets"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_CURRENT_BINARY_DIR}/LD54-windows/assets"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/vendor/lib/SDL2.dll" "{CMAKE_CURRENT_BINARY_DIR}/LD54-windows"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/LD54.exe" "${CMAKE_CURRENT_BINARY_DIR}/LD54-windows"
            )
elseif(APPLE)
    add_executable(LD54 MACOSX_BUNDLE src/main.cpp)
    set_target_properties(LD54 PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist)
    target_include_directories(LD54 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/include")
    target_link_libraries(LD54 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/lib/SDL2.framework")
    add_custom_command(TARGET LD54 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/LD54.app/Contents/Frameworks
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/vendor/lib/SDL2.framework ${CMAKE_CURRENT_BINARY_DIR}/LD54.app/Contents/Frameworks/SDL2.framework
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/LD54.app/Contents/Resources
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/LD54.app/Contents/Resources
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/LD54.sh ${CMAKE_CURRENT_BINARY_DIR}/LD54.app/Contents/MacOS

            COMMAND install_name_tool -change ${CMAKE_SOURCE_DIR}/vendor/lib/SDL2.framework/Versions/A/SDL2 @executable_path/../Frameworks/SDL2.framework/Versions/A/SDL2 ${CMAKE_CURRENT_BINARY_DIR}/LD54.app/Contents/MacOS/LD54
            )
endif()