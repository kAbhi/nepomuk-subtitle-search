project(nepomuk-subtitle-loader)

find_package(KDE4 REQUIRED)
find_package(NepomukCore REQUIRED)
find_package(Soprano REQUIRED)

include(SopranoAddOntology)
include (KDE4Defaults)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${SOPRANO_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${NEPOMUK_CORE_INCLUDE_DIR}
  )

set( SRCS
  srtparser.cpp
  subtitleloader.cpp
  main.cpp
)

soprano_add_ontology( SRCS "${CMAKE_SOURCE_DIR}/ontology/nsbo.trig" 
                      "NSBO" "Nepomuk2::Vocabulary" "trig" )

kde4_add_executable(nepomuk-subtitle-loader ${SRCS})

target_link_libraries( nepomuk-subtitle-loader
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  ${NEPOMUK_CORE_LIBRARY}
  ${SOPRANO_LIBRARIES}
  )

install(TARGETS nepomuk-subtitle-loader
        DESTINATION ${BIN_INSTALL_DIR})

