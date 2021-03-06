set(RTBIN_INCLUDE_DIRS
  ${OPTICAL_INCLUDE_DIRS}
  ${FB_INCLUDE_DIRS}
  ${RT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
LIST(REMOVE_DUPLICATES RTBIN_INCLUDE_DIRS)
include_directories(${RTBIN_INCLUDE_DIRS})

set(RT_COMMON_SRCS
  do.c
  main.c
  opt.c
  worker.c
  )
if(NOT MSVC)
  set(RT_COMMON_SRCS ${RT_COMMON_SRCS} heatgraph.c)
endif(NOT MSVC)

set(rtexample_SRCS rtexample.c)
BRLCAD_ADDDATA(rtexample.c sample_applications)

set(reshoot_SRCS reshoot.c)

set(rtwalk_SRCS rtwalk.c)

set(rtshot_SRCS rtshot.c)

set(rtregis_SRCS
  read-rtlog.c
  rtregis.c
  )

set(rtscale_SRCS
  read-rtlog.c
  rtscale.c
  )

set(rt_SRCS
  ${RT_COMMON_SRCS}
  scanline.c
  view.c
  )

set(rt_bot_faces_SRCS
  ${RT_COMMON_SRCS}
  view_bot_faces.c
  )

set(rtarea_SRCS
  ${RT_COMMON_SRCS}
  viewarea.c
  )

set(rtcell_SRCS
  ${RT_COMMON_SRCS}
  viewcell.c
  )

set(rtcheck_SRCS
  ${RT_COMMON_SRCS}
  viewcheck.c
  )

set(rtedge_SRCS
  ${RT_COMMON_SRCS}
  viewedge.c
  )

set(rtfrac_SRCS
  ${RT_COMMON_SRCS}
  viewfrac.c
  )

set(rtg3_SRCS
  ${RT_COMMON_SRCS}
  viewg3.c
  )

set(rthide_SRCS
  ${RT_COMMON_SRCS}
  viewhide.c
  )

set(rtpp_SRCS
  ${RT_COMMON_SRCS}
  viewpp.c
  )

set(rtrad_SRCS
  ${RT_COMMON_SRCS}
  viewrad.c
  )

set(rtrange_SRCS
  ${RT_COMMON_SRCS}
  viewrange.c
  )

set(rtray_SRCS
  ${RT_COMMON_SRCS}
  viewray.c
  )

set(rtsil_SRCS
  ${RT_COMMON_SRCS}
  viewsil.c
  )

set(rtweight_SRCS
  ${RT_COMMON_SRCS}
  viewweight.c
  )

set(rtxray_SRCS
  ${RT_COMMON_SRCS}
  viewxray.c
  )

set(LIBREMRT_SOURCES
  do.c
  opt.c
  scanline.c
  sh_tcl.c
  view.c
  worker.c
  )
if(NOT MSVC)
  set(LIBREMRT_SOURCES ${LIBREMRT_SOURCES} heatgraph.c)
endif(NOT MSVC)

BRLCAD_ADDEXEC(rtexample "${rtexample_SRCS}" "librt;libfb;liboptical;libicv" NO_INSTALL)

BRLCAD_ADDEXEC(reshoot "${reshoot_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv")

BRLCAD_ADDEXEC(rtwalk "${rtwalk_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtshot "${rtshot_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtregis "${rtregis_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv")

BRLCAD_ADDEXEC(rtscale "${rtscale_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rt "${rt_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rt_bot_faces "${rt_bot_faces_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtarea "${rtarea_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtcell "${rtcell_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtcheck "${rtcheck_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtedge "${rtedge_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtfrac "${rtfrac_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtg3 "${rtg3_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rthide "${rthide_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtpp "${rtpp_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtrad "${rtrad_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtrange "${rtrange_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtray "${rtray_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtsil "${rtsil_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtweight "${rtweight_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")

BRLCAD_ADDEXEC(rtxray "${rtxray_SRCS}" "${CMAKE_THREAD_LIBS_INIT};librt;libfb;liboptical;libicv;${M_LIBRARY}")


add_library(libremrt STATIC ${LIBREMRT_SOURCES})
if(M_LIBRARY)
  target_link_libraries(libremrt ${M_LIBRARY})
endif(M_LIBRARY)
set_target_properties(libremrt PROPERTIES FOLDER "BRL-CAD Static Libraries")

add_library(librttherm STATIC ${RT_COMMON_SRCS})
set_target_properties(librttherm PROPERTIES LINKER_LANGUAGE C)
set_target_properties(librttherm PROPERTIES FOLDER "BRL-CAD Static Libraries")

set(rt_ignore_files
  ext.h
  libfb-dummy.c
  rad.h
  rtuif.h
  scanline.h
  viewdir.c
  viewdummy.c
  viewscat.c
  )
CMAKEFILES(${rt_ignore_files})

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8
