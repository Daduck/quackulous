cmake_minimum_required(VERSION 3.24)

project(Quackulous VERSION 1.2.0 LANGUAGES C RC ASM_MASM)

if(NOT WIN32)
  message(FATAL_ERROR "The supported first-class CMake path currently targets Windows only.")
endif()

if(NOT MSVC)
  message(FATAL_ERROR "The supported first-class Windows build currently targets MSVC.")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR "The current MSVC bring-up targets x64 only. Use -A x64 with Visual Studio 2022.")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(QUACK_BUILD_CLIENT "Build the playable client." ON)
option(QUACK_BUILD_SERVER "Build the dedicated server." ON)
option(QUACK_BUILD_GAME_DLLS "Build native cgame/game/ui DLLs." ON)
option(QUACK_BUILD_RENDERER_OPENGL2 "Build the OpenGL 2 renderer DLL." ON)
option(QUACK_USE_RENDERER_DLOPEN "Load renderers as DLLs." ON)
option(QUACK_USE_OPENAL "Enable OpenAL sound support." ON)
option(QUACK_USE_OPENAL_DLOPEN "Load OpenAL dynamically at runtime." ON)
option(QUACK_USE_CURL "Enable libcurl download support." ON)
option(QUACK_USE_CURL_DLOPEN "Load libcurl dynamically at runtime." ON)
option(QUACK_USE_CODEC_OPUS "Enable Opus codec support." ON)
option(QUACK_USE_CODEC_VORBIS "Enable Vorbis codec support." OFF)
option(QUACK_USE_VOIP "Enable VoIP support." ON)
option(QUACK_USE_MUMBLE "Enable Mumble link support." ON)
option(QUACK_USE_INTERNAL_ZLIB "Use the vendored zlib sources." ON)
option(QUACK_USE_INTERNAL_JPEG "Use the vendored JPEG sources." ON)
option(QUACK_USE_INTERNAL_OGG "Use the vendored Ogg sources." ON)
option(QUACK_USE_INTERNAL_VORBIS "Use the vendored Vorbis sources." ON)
option(QUACK_USE_INTERNAL_OPUS "Use the vendored Opus sources." ON)
option(QUACK_USE_LOCAL_HEADERS "Use in-tree headers for SDL/OpenAL/cURL compatibility." OFF)

if(NOT QUACK_USE_RENDERER_DLOPEN)
  message(FATAL_ERROR "The initial MSVC bring-up currently requires QUACK_USE_RENDERER_DLOPEN=ON.")
endif()

if(NOT QUACK_USE_INTERNAL_ZLIB)
  message(FATAL_ERROR "The initial MSVC bring-up currently requires QUACK_USE_INTERNAL_ZLIB=ON.")
endif()

if(NOT QUACK_USE_INTERNAL_JPEG)
  message(FATAL_ERROR "The initial MSVC bring-up currently requires QUACK_USE_INTERNAL_JPEG=ON.")
endif()

if(QUACK_USE_CODEC_OPUS AND NOT QUACK_USE_INTERNAL_OPUS)
  message(FATAL_ERROR "The initial MSVC bring-up currently requires QUACK_USE_INTERNAL_OPUS=ON when Opus is enabled.")
endif()

if(QUACK_USE_CODEC_VORBIS AND NOT QUACK_USE_INTERNAL_VORBIS)
  message(FATAL_ERROR "The initial MSVC bring-up currently requires QUACK_USE_INTERNAL_VORBIS=ON when Vorbis is enabled.")
endif()

if((QUACK_USE_CODEC_OPUS OR QUACK_USE_CODEC_VORBIS) AND NOT QUACK_USE_INTERNAL_OGG)
  message(FATAL_ERROR "The initial MSVC bring-up currently requires QUACK_USE_INTERNAL_OGG=ON when Ogg-backed codecs are enabled.")
endif()

set(QUACK_ARCH_STRING "x86_64")
set(QUACK_RUNTIME_ROOT "${CMAKE_BINARY_DIR}/stage")
set(QUACK_BASEGAME "base")

find_package(SDL2 CONFIG REQUIRED)

include(CTest)
include(cmake/QuackulousSources.cmake)

quackulous_collect_sources()

function(quackulous_generate_glsl_fallback out_var)
  set(outputs)
  foreach(shader IN LISTS ARGN)
    get_filename_component(shader_name "${shader}" NAME_WE)
    set(out_file "${CMAKE_CURRENT_BINARY_DIR}/generated/renderergl2/glsl/${shader_name}.c")
    if(IS_ABSOLUTE "${shader}")
      set(shader_path "${shader}")
    else()
      set(shader_path "${CMAKE_CURRENT_SOURCE_DIR}/${shader}")
    endif()
    file(READ "${shader_path}" shader_source)
    string(REPLACE "\\" "\\\\" shader_source "${shader_source}")
    string(REPLACE "\"" "\\\"" shader_source "${shader_source}")
    string(REPLACE "\r\n" "\n" shader_source "${shader_source}")
    string(REPLACE "\r" "\n" shader_source "${shader_source}")
    string(REPLACE "\n" "\\n\"\n\"" shader_source "${shader_source}")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated/renderergl2/glsl")
    file(WRITE "${out_file}" "const char *fallbackShader_${shader_name} =\n\"${shader_source}\";\n")
    list(APPEND outputs "${out_file}")
  endforeach()

  set(${out_var} "${outputs}" PARENT_SCOPE)
endfunction()

quackulous_generate_glsl_fallback(QUACK_RENDERER_GL2_SHADER_SOURCES ${QUACK_RENDERER_GL2_SHADER_INPUTS})

set(QUACK_COMMON_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/SDL2/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/AL"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/client"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/server"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/qcommon"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/sdl"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/sys"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/renderercommon"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/renderergl1"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/renderergl2"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/game"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cgame"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/null"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/jpeg-8c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/libcurl-7.35.0"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/libogg-1.3.1/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/libvorbis-1.3.4/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/opus-1.1/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/opus-1.1/celt"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/libvorbis-1.3.4/lib"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/opus-1.1/silk"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/opus-1.1/silk/float"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/opusfile-0.5/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib"
)

list(REMOVE_DUPLICATES QUACK_COMMON_INCLUDE_DIRS)

set(QUACK_COMMON_DEFINES
  ARCH_STRING="${QUACK_ARCH_STRING}"
  PRODUCT_VERSION="${PROJECT_VERSION}"
  USE_ICON
  WINVER=0x0601
  _CRT_NONSTDC_NO_DEPRECATE
  _CRT_SECURE_NO_WARNINGS
)

if(QUACK_USE_LOCAL_HEADERS)
  list(APPEND QUACK_COMMON_DEFINES USE_LOCAL_HEADERS)
endif()

if(QUACK_USE_INTERNAL_ZLIB)
  list(APPEND QUACK_COMMON_DEFINES NO_GZIP)
endif()

if(QUACK_USE_INTERNAL_JPEG)
  list(APPEND QUACK_COMMON_DEFINES USE_INTERNAL_JPEG)
endif()

if(QUACK_USE_RENDERER_DLOPEN)
  list(APPEND QUACK_COMMON_DEFINES USE_RENDERER_DLOPEN)
endif()

function(quackulous_target_defaults target)
  target_include_directories("${target}" PRIVATE ${QUACK_COMMON_INCLUDE_DIRS})
  target_compile_definitions("${target}" PRIVATE ${QUACK_COMMON_DEFINES})
  target_compile_options("${target}" PRIVATE
    $<$<COMPILE_LANGUAGE:C>:/W3>
    $<$<COMPILE_LANGUAGE:C>:/wd4113>
    $<$<COMPILE_LANGUAGE:C>:/wd4244>
    $<$<COMPILE_LANGUAGE:C>:/wd4996>
  )
  target_link_options("${target}" PRIVATE /DEBUG:FULL)
endfunction()

function(quackulous_copy_runtime_assets target)
  add_custom_command(TARGET "${target}" POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${target}>/${QUACK_BASEGAME}"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/assets"
      "$<TARGET_FILE_DIR:${target}>/${QUACK_BASEGAME}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/misc/server.cfg"
      "$<TARGET_FILE_DIR:${target}>/server.cfg"
    VERBATIM
  )
  # Copy game data pk3 files if the gamedata/base directory exists.
  # These are not committed to the repo; populate gamedata/base/ from your
  # Tremulous installation once and they will be deployed to every build config.
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gamedata/base")
    file(GLOB _gamedata_pk3s "${CMAKE_CURRENT_SOURCE_DIR}/gamedata/base/*.pk3")
    if(_gamedata_pk3s)
      add_custom_command(TARGET "${target}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy
          ${_gamedata_pk3s}
          "$<TARGET_FILE_DIR:${target}>/${QUACK_BASEGAME}"
        VERBATIM
      )
    endif()
  endif()
endfunction()

function(quackulous_copy_runtime_dlls target)
  add_custom_command(TARGET "${target}" POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      $<TARGET_RUNTIME_DLLS:${target}>
      $<TARGET_FILE_DIR:${target}>
    COMMAND_EXPAND_LISTS
    VERBATIM
  )
endfunction()

if(QUACK_BUILD_CLIENT)
  add_executable(quackulous-client WIN32
    ${QUACK_CLIENT_SOURCES}
    ${QUACK_CLIENT_ZLIB_SOURCES}
    ${QUACK_CLIENT_OGG_SOURCES}
    ${QUACK_CLIENT_OPUS_SOURCES}
    ${QUACK_CLIENT_OPUSFILE_SOURCES}
    ${QUACK_CLIENT_VORBIS_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sys/win_resource.rc"
  )
  quackulous_target_defaults(quackulous-client)
  target_link_libraries(quackulous-client PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    gdi32
    ole32
    psapi
    winmm
    ws2_32
  )
  if(QUACK_USE_OPENAL)
    target_compile_definitions(quackulous-client PRIVATE USE_OPENAL)
    if(QUACK_USE_OPENAL_DLOPEN)
      target_compile_definitions(quackulous-client PRIVATE USE_OPENAL_DLOPEN)
    endif()
  endif()
  if(QUACK_USE_CURL)
    target_compile_definitions(quackulous-client PRIVATE USE_CURL)
    if(QUACK_USE_CURL_DLOPEN)
      target_compile_definitions(quackulous-client PRIVATE USE_CURL_DLOPEN)
    endif()
  endif()
  if(QUACK_USE_VOIP)
    target_compile_definitions(quackulous-client PRIVATE USE_VOIP)
  endif()
  if(QUACK_USE_CODEC_OPUS)
    target_compile_definitions(quackulous-client PRIVATE
      USE_CODEC_OPUS
      OPUS_BUILD
      HAVE_LRINTF
      FLOATING_POINT
      USE_ALLOCA
    )
  endif()
  if(QUACK_USE_CODEC_VORBIS)
    target_compile_definitions(quackulous-client PRIVATE USE_CODEC_VORBIS)
  endif()
  if(QUACK_USE_MUMBLE)
    target_compile_definitions(quackulous-client PRIVATE USE_MUMBLE)
  endif()
  set_target_properties(quackulous-client PROPERTIES
    OUTPUT_NAME "tremulous.${QUACK_ARCH_STRING}"
    RUNTIME_OUTPUT_DIRECTORY "${QUACK_RUNTIME_ROOT}/$<CONFIG>"
    VS_DEBUGGER_WORKING_DIRECTORY "${QUACK_RUNTIME_ROOT}/$(Configuration)"
    VS_DEBUGGER_COMMAND_ARGUMENTS "+set sv_pure 0"
  )
  quackulous_copy_runtime_assets(quackulous-client)
  quackulous_copy_runtime_dlls(quackulous-client)
  # OpenAL is loaded at runtime via LoadLibrary (USE_OPENAL_DLOPEN), so it is
  # invisible to TARGET_RUNTIME_DLLS.  Stage it explicitly from the vcpkg tree.
  if(QUACK_USE_OPENAL AND QUACK_USE_OPENAL_DLOPEN AND DEFINED VCPKG_INSTALLED_DIR)
    set(_oal_dll_debug   "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin/OpenAL32.dll")
    set(_oal_dll_release "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/OpenAL32.dll")
    add_custom_command(TARGET quackulous-client POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${_oal_dll_debug},${_oal_dll_release}>"
        "$<TARGET_FILE_DIR:quackulous-client>"
      COMMENT "Staging OpenAL32.dll"
      VERBATIM
    )
  endif()
endif()

if(QUACK_BUILD_SERVER)
  add_executable(quackulous-dedicated
    ${QUACK_DEDICATED_SOURCES}
    ${QUACK_DEDICATED_ZLIB_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sys/win_resource.rc"
  )
  quackulous_target_defaults(quackulous-dedicated)
  target_compile_definitions(quackulous-dedicated PRIVATE DEDICATED)
  if(QUACK_USE_VOIP)
    target_compile_definitions(quackulous-dedicated PRIVATE USE_VOIP)
  endif()
  target_link_libraries(quackulous-dedicated PRIVATE
    psapi
    winmm
    ws2_32
  )
  set_target_properties(quackulous-dedicated PROPERTIES
    OUTPUT_NAME "tremded.${QUACK_ARCH_STRING}"
    RUNTIME_OUTPUT_DIRECTORY "${QUACK_RUNTIME_ROOT}/$<CONFIG>"
    VS_DEBUGGER_WORKING_DIRECTORY "${QUACK_RUNTIME_ROOT}/$(Configuration)"
  )
  quackulous_copy_runtime_assets(quackulous-dedicated)
endif()

if(QUACK_BUILD_CLIENT)
  add_library(quackulous-renderer-opengl1 SHARED
    ${QUACK_RENDERER_GL1_SOURCES}
    ${QUACK_RENDERER_JPEG_SOURCES}
  )
  quackulous_target_defaults(quackulous-renderer-opengl1)
  target_link_libraries(quackulous-renderer-opengl1 PRIVATE
    SDL2::SDL2
    gdi32
    ole32
    opengl32
    psapi
    winmm
    ws2_32
  )
  set_target_properties(quackulous-renderer-opengl1 PROPERTIES
    OUTPUT_NAME "renderer_opengl1_${QUACK_ARCH_STRING}"
    RUNTIME_OUTPUT_DIRECTORY "${QUACK_RUNTIME_ROOT}/$<CONFIG>"
  )
endif()

if(QUACK_BUILD_CLIENT AND QUACK_BUILD_RENDERER_OPENGL2)
  add_library(quackulous-renderer-opengl2 SHARED
    ${QUACK_RENDERER_GL2_SOURCES}
    ${QUACK_RENDERER_GL2_SHADER_SOURCES}
    ${QUACK_RENDERER_JPEG_SOURCES}
  )
  quackulous_target_defaults(quackulous-renderer-opengl2)
  target_include_directories(quackulous-renderer-opengl2 PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}/generated/renderergl2/glsl"
  )
  target_link_libraries(quackulous-renderer-opengl2 PRIVATE
    SDL2::SDL2
    gdi32
    ole32
    opengl32
    psapi
    winmm
    ws2_32
  )
  target_compile_definitions(quackulous-renderer-opengl2 PRIVATE RENDERER_OPENGL2)
  set_target_properties(quackulous-renderer-opengl2 PROPERTIES
    OUTPUT_NAME "renderer_opengl2_${QUACK_ARCH_STRING}"
    RUNTIME_OUTPUT_DIRECTORY "${QUACK_RUNTIME_ROOT}/$<CONFIG>"
  )
endif()

if(QUACK_BUILD_GAME_DLLS)
  add_library(quackulous-cgame SHARED ${QUACK_CGAME_SOURCES})
  quackulous_target_defaults(quackulous-cgame)
  target_compile_definitions(quackulous-cgame PRIVATE CGAME)
  set_target_properties(quackulous-cgame PROPERTIES
    OUTPUT_NAME "cgame${QUACK_ARCH_STRING}"
    RUNTIME_OUTPUT_DIRECTORY "${QUACK_RUNTIME_ROOT}/$<CONFIG>/${QUACK_BASEGAME}"
  )

  add_library(quackulous-game SHARED ${QUACK_GAME_DLL_SOURCES})
  quackulous_target_defaults(quackulous-game)
  target_compile_definitions(quackulous-game PRIVATE GAME)
  set_target_properties(quackulous-game PROPERTIES
    OUTPUT_NAME "game${QUACK_ARCH_STRING}"
    RUNTIME_OUTPUT_DIRECTORY "${QUACK_RUNTIME_ROOT}/$<CONFIG>/${QUACK_BASEGAME}"
  )

  add_library(quackulous-ui SHARED ${QUACK_UI_SOURCES})
  quackulous_target_defaults(quackulous-ui)
  target_compile_definitions(quackulous-ui PRIVATE UI)
  set_target_properties(quackulous-ui PROPERTIES
    OUTPUT_NAME "ui${QUACK_ARCH_STRING}"
    RUNTIME_OUTPUT_DIRECTORY "${QUACK_RUNTIME_ROOT}/$<CONFIG>/${QUACK_BASEGAME}"
  )
endif()

if(QUACK_BUILD_CLIENT AND BUILD_TESTING)
  add_test(
    NAME client_version
    COMMAND "$<TARGET_FILE:quackulous-client>" --version
  )
  set_tests_properties(client_version PROPERTIES LABELS "smoke")

  add_test(
    NAME runtime_layout
    COMMAND powershell
      -ExecutionPolicy Bypass
      -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/smoke-runtime.ps1"
      -RuntimeRoot "$<TARGET_FILE_DIR:quackulous-client>"
      -Arch "${QUACK_ARCH_STRING}"
      -RequireServer "$<IF:$<BOOL:${QUACK_BUILD_SERVER}>,1,0>"
      -RequireOpenGL2 "$<IF:$<BOOL:${QUACK_BUILD_RENDERER_OPENGL2}>,1,0>"
  )
  set_tests_properties(runtime_layout PROPERTIES LABELS "smoke")

  add_test(
    NAME client_startup_smoke
    COMMAND powershell
      -ExecutionPolicy Bypass
      -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/startup-smoke.ps1"
      -RuntimeRoot "$<TARGET_FILE_DIR:quackulous-client>"
  )
  set_tests_properties(client_startup_smoke PROPERTIES
    LABELS "smoke;interactive"
    TIMEOUT 90
  )

  add_test(
    NAME client_renderer_bootstrap
    COMMAND powershell
      -ExecutionPolicy Bypass
      -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/client-module-smoke.ps1"
      -RuntimeRoot "$<TARGET_FILE_DIR:quackulous-client>"
      -Module renderer
  )
  set_tests_properties(client_renderer_bootstrap PROPERTIES
    LABELS "smoke;bringup;interactive"
    TIMEOUT 90
  )

  if(QUACK_BUILD_RENDERER_OPENGL2)
    add_test(
      NAME client_renderer_opengl2_bootstrap
      COMMAND powershell
        -ExecutionPolicy Bypass
        -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/client-module-smoke.ps1"
        -RuntimeRoot "$<TARGET_FILE_DIR:quackulous-client>"
        -Module renderer-opengl2
    )
    set_tests_properties(client_renderer_opengl2_bootstrap PROPERTIES
      LABELS "smoke;bringup;interactive"
      TIMEOUT 90
    )

    add_test(
      NAME client_display_ultrawide_windowed_opengl2
      COMMAND powershell
        -ExecutionPolicy Bypass
        -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/client-module-smoke.ps1"
        -RuntimeRoot "$<TARGET_FILE_DIR:quackulous-client>"
        -Module renderer-opengl2
        -CustomWidth 3440
        -CustomHeight 1440
        -Fullscreen 0
    )
    set_tests_properties(client_display_ultrawide_windowed_opengl2 PROPERTIES
      LABELS "smoke;display;interactive"
      TIMEOUT 90
    )

    add_test(
      NAME client_display_fullscreen_opengl2
      COMMAND powershell
        -ExecutionPolicy Bypass
        -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/client-module-smoke.ps1"
        -RuntimeRoot "$<TARGET_FILE_DIR:quackulous-client>"
        -Module renderer-opengl2
        -CustomWidth 1920
        -CustomHeight 1080
        -Fullscreen 1
        -ModeRegex "MODE: -1, [0-9]+ x [0-9]+ fullscreen"
    )
    set_tests_properties(client_display_fullscreen_opengl2 PROPERTIES
      LABELS "smoke;display;interactive"
      TIMEOUT 90
    )
  endif()

  add_test(
    NAME client_filesystem_network_bootstrap
    COMMAND powershell
      -ExecutionPolicy Bypass
      -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/client-module-smoke.ps1"
      -RuntimeRoot "$<TARGET_FILE_DIR:quackulous-client>"
      -Module filesystem-network
  )
  set_tests_properties(client_filesystem_network_bootstrap PROPERTIES
    LABELS "smoke;bringup;interactive"
    TIMEOUT 90
  )

  add_test(
    NAME client_audio_bootstrap
    COMMAND powershell
      -ExecutionPolicy Bypass
      -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/client-module-smoke.ps1"
      -RuntimeRoot "$<TARGET_FILE_DIR:quackulous-client>"
      -Module audio
  )
  set_tests_properties(client_audio_bootstrap PROPERTIES
    LABELS "smoke;bringup;interactive"
    TIMEOUT 90
  )
endif()

if(QUACK_BUILD_SERVER AND BUILD_TESTING)
  add_test(
    NAME dedicated_version
    COMMAND "$<TARGET_FILE:quackulous-dedicated>" --version
  )
  set_tests_properties(dedicated_version PROPERTIES LABELS "server")
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION "${QUACK_BASEGAME}")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/misc/server.cfg" DESTINATION ".")

if(QUACK_BUILD_CLIENT)
  install(TARGETS quackulous-client quackulous-renderer-opengl1
    RUNTIME DESTINATION "."
  )
  if(QUACK_BUILD_RENDERER_OPENGL2)
    install(TARGETS quackulous-renderer-opengl2
      RUNTIME DESTINATION "."
    )
  endif()
endif()

if(QUACK_BUILD_SERVER)
  install(TARGETS quackulous-dedicated
    RUNTIME DESTINATION "."
  )
endif()

if(QUACK_BUILD_GAME_DLLS)
  install(TARGETS quackulous-cgame quackulous-game quackulous-ui
    RUNTIME DESTINATION "${QUACK_BASEGAME}"
  )
endif()
