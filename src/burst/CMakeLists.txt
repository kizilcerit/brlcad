if(HAVE_TERMLIB)

  set(BURST_INCLUDE_DIRS
    ${TERMIO_INCLUDE_DIRS}
    ${FB_INCLUDE_DIRS}
    ${RT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
  LIST(REMOVE_DUPLICATES BURST_INCLUDE_DIRS)
  include_directories(${BURST_INCLUDE_DIRS})

  set(burst_SOURCES
    burst.c
    error.c
    fb.c
    glob.c
    grid.c
    gridrotate.c
    Hm.c
    HmGetc.c
    HmGlob.c
    idents.c
    paint.c
    plot.c
    prnt.c
    Sc.c
    trie.c
    ui.c
    )

  BRLCAD_ADDEXEC(burst "${burst_SOURCES}" "librt;libfb;libtermio;${TERMLIB_LIBRARY};${M_LIBRARY}")

  set(burst_noinst_HEADERS
    ascii.h
    burst.h
    extern.h
    Hm.h
    Mm.h
    Sc.h
    trie.h
    )
  CMAKEFILES(${burst_noinst_HEADERS} burst.format)
endif(HAVE_TERMLIB)

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8
