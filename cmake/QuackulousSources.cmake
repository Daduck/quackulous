function(quackulous_collect_sources)
  file(GLOB client_core CONFIGURE_DEPENDS "src/client/*.c")
  file(GLOB server_core CONFIGURE_DEPENDS "src/server/*.c")
  file(GLOB renderer_common CONFIGURE_DEPENDS "src/renderercommon/*.c")
  file(GLOB renderer_gl1 CONFIGURE_DEPENDS "src/renderergl1/*.c")
  file(GLOB renderer_gl2 CONFIGURE_DEPENDS "src/renderergl2/*.c")
  file(GLOB game_bg CONFIGURE_DEPENDS "src/game/bg_*.c")
  file(GLOB game_dll CONFIGURE_DEPENDS "src/game/g_*.c")
  file(GLOB cgame_core CONFIGURE_DEPENDS "src/cgame/*.c")
  file(GLOB ui_core CONFIGURE_DEPENDS "src/ui/*.c")
  file(GLOB jpeg_sources CONFIGURE_DEPENDS "src/jpeg-8c/*.c")
  file(GLOB zlib_sources CONFIGURE_DEPENDS "src/zlib/*.c")
  file(GLOB ogg_sources CONFIGURE_DEPENDS "src/libogg-1.3.1/src/*.c")
  file(GLOB vorbis_sources CONFIGURE_DEPENDS "src/libvorbis-1.3.4/lib/*.c")
  file(GLOB opus_sources CONFIGURE_DEPENDS
    "src/opus-1.1/src/*.c"
    "src/opus-1.1/celt/*.c"
    "src/opus-1.1/silk/*.c"
    "src/opus-1.1/silk/float/*.c"
  )
  file(GLOB opusfile_sources CONFIGURE_DEPENDS "src/opusfile-0.5/src/*.c")
  file(GLOB renderer_gl2_glsl CONFIGURE_DEPENDS "src/renderergl2/glsl/*.glsl")

  list(REMOVE_ITEM client_core
    "${CMAKE_CURRENT_SOURCE_DIR}/src/client/libmumblelink.c"
  )
  list(REMOVE_ITEM vorbis_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/src/libvorbis-1.3.4/lib/vorbisenc.c"
  )
  list(REMOVE_ITEM opus_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/src/opus-1.1/celt/opus_custom_demo.c"
  )

  set(qcommon_engine
    src/qcommon/cm_load.c
    src/qcommon/cm_patch.c
    src/qcommon/cm_polylib.c
    src/qcommon/cm_test.c
    src/qcommon/cm_trace.c
    src/qcommon/cmd.c
    src/qcommon/common.c
    src/qcommon/cvar.c
    src/qcommon/files.c
    src/qcommon/huffman.c
    src/qcommon/ioapi.c
    src/qcommon/md4.c
    src/qcommon/md5.c
    src/qcommon/msg.c
    src/qcommon/net_chan.c
    src/qcommon/net_ip.c
    src/qcommon/parse.c
    src/qcommon/puff.c
    src/qcommon/q_math.c
    src/qcommon/q_shared.c
    src/qcommon/unzip.c
    src/qcommon/vm.c
    src/qcommon/vm_interpreted.c
    src/qcommon/vm_x86.c
  )

  set(client_platform
    src/asm/ftola.c
    src/asm/snapvector.c
    src/asm/vm_x86_64.asm
    src/sdl/sdl_input.c
    src/sdl/sdl_snd.c
    src/sys/con_log.c
    src/sys/con_passive.c
    src/sys/sys_main.c
    src/sys/sys_win32.c
  )

  set(dedicated_platform
    src/asm/ftola.c
    src/asm/snapvector.c
    src/asm/vm_x86_64.asm
    src/null/null_client.c
    src/null/null_input.c
    src/null/null_snddma.c
    src/sys/con_log.c
    src/sys/con_win32.c
    src/sys/sys_main.c
    src/sys/sys_win32.c
  )

  set(renderer_platform
    src/sdl/sdl_gamma.c
    src/sdl/sdl_glimp.c
  )

  set(renderer_dlopen_common
    src/qcommon/puff.c
    src/qcommon/q_math.c
    src/qcommon/q_shared.c
    src/renderergl1/tr_subs.c
  )

  set(client_sources
    ${client_core}
    ${server_core}
    ${qcommon_engine}
    ${client_platform}
  )

  if(QUACK_USE_MUMBLE)
    list(APPEND client_sources src/client/libmumblelink.c)
  endif()

  set(dedicated_sources
    ${server_core}
    ${qcommon_engine}
    ${dedicated_platform}
  )

  set(renderer_gl1_sources
    ${renderer_common}
    ${renderer_gl1}
    ${renderer_platform}
    ${renderer_dlopen_common}
  )

  set(renderer_gl2_sources
    ${renderer_common}
    ${renderer_gl2}
    ${renderer_platform}
    ${renderer_dlopen_common}
  )

  set(cgame_sources
    ${cgame_core}
    ${game_bg}
    src/qcommon/q_math.c
    src/qcommon/q_shared.c
    src/ui/ui_shared.c
  )

  set(game_dll_sources
    ${game_dll}
    ${game_bg}
    src/qcommon/q_math.c
    src/qcommon/q_shared.c
  )

  set(ui_sources
    ${ui_core}
    src/game/bg_lib.c
    src/game/bg_misc.c
    src/qcommon/q_math.c
    src/qcommon/q_shared.c
  )

  set(client_ogg_sources)
  if(QUACK_USE_CODEC_OPUS OR QUACK_USE_CODEC_VORBIS)
    set(client_ogg_sources ${ogg_sources})
  endif()

  set(client_opus_sources)
  set(client_opusfile_sources)
  if(QUACK_USE_CODEC_OPUS)
    set(client_opus_sources ${opus_sources})
    set(client_opusfile_sources ${opusfile_sources})
  endif()

  set(client_vorbis_sources)
  if(QUACK_USE_CODEC_VORBIS)
    set(client_vorbis_sources ${vorbis_sources})
  endif()

  set(QUACK_CLIENT_SOURCES "${client_sources}" PARENT_SCOPE)
  set(QUACK_CLIENT_ZLIB_SOURCES "${zlib_sources}" PARENT_SCOPE)
  set(QUACK_CLIENT_OGG_SOURCES "${client_ogg_sources}" PARENT_SCOPE)
  set(QUACK_CLIENT_OPUS_SOURCES "${client_opus_sources}" PARENT_SCOPE)
  set(QUACK_CLIENT_OPUSFILE_SOURCES "${client_opusfile_sources}" PARENT_SCOPE)
  set(QUACK_CLIENT_VORBIS_SOURCES "${client_vorbis_sources}" PARENT_SCOPE)
  set(QUACK_DEDICATED_SOURCES "${dedicated_sources}" PARENT_SCOPE)
  set(QUACK_DEDICATED_ZLIB_SOURCES "${zlib_sources}" PARENT_SCOPE)
  set(QUACK_RENDERER_GL1_SOURCES "${renderer_gl1_sources}" PARENT_SCOPE)
  set(QUACK_RENDERER_GL2_SOURCES "${renderer_gl2_sources}" PARENT_SCOPE)
  set(QUACK_RENDERER_GL2_SHADER_INPUTS "${renderer_gl2_glsl}" PARENT_SCOPE)
  set(QUACK_RENDERER_JPEG_SOURCES "${jpeg_sources}" PARENT_SCOPE)
  set(QUACK_CGAME_SOURCES "${cgame_sources}" PARENT_SCOPE)
  set(QUACK_GAME_DLL_SOURCES "${game_dll_sources}" PARENT_SCOPE)
  set(QUACK_UI_SOURCES "${ui_sources}" PARENT_SCOPE)
endfunction()
