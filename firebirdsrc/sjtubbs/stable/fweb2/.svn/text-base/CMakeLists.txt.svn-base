PROJECT(FWEB2 C CXX)
SET(FWEB2NAME fweb2)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmakescript)
INCLUDE(fweb2macro)

IF(DEFINED FWEB_INSTALL_PREFIX)
	SET(CMAKE_INSTALL_PREFIX ${FWEB_INSTALL_PREFIX})
ELSE(DEFINED FWEB_INSTALL_PREFIX)
	SET(CMAKE_INSTALL_PREFIX /home/bbs/bbshome/httpd)
ENDIF(DEFINED FWEB_INSTALL_PREFIX)


IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(${CMAKE_C_FLAGS} STREQUAL " ")
	SET(CMAKE_C_FLAGS " -Wall ")
	CHECK_C_ACCEPTS_FLAG("-Wextra" c_opt_wextra)
	CHECK_C_ACCEPTS_FLAG("-Wno-unused-parameter" c_opt_no_param)
	CHECK_C_ACCEPTS_FLAG("-pipe" c_opt_pipe)
	CHECK_C_ACCEPTS_FLAG("-fno-strict-aliasing" c_opt_no_aliasing)
	IF(c_opt_pipe)
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe")
	ENDIF(c_opt_pipe)
	IF(c_opt_wextra)
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
	ENDIF(c_opt_wextra)
	IF(c_opt_no_param)
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")
	ENDIF(c_opt_no_param)
	IF(c_opt_no_aliasing)
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
	ENDIF(c_opt_no_aliasing)
ENDIF(${CMAKE_C_FLAGS} STREQUAL " ")

IF(${CMAKE_CXX_FLAGS} STREQUAL " ")
	SET(CMAKE_CXX_FLAGS " -Wall ")
	CHECK_CXX_ACCEPTS_FLAG("-Wextra" cxx_opt_wextra)
	CHECK_CXX_ACCEPTS_FLAG("-Wno-unused-parameter" cxx_opt_no_param)
	CHECK_CXX_ACCEPTS_FLAG("-pipe" cxx_opt_pipe)
	IF(cxx_opt_pipe)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")
	ENDIF(cxx_opt_pipe)
	IF(cxx_opt_wextra)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
	ENDIF(cxx_opt_wextra)
	IF(cxx_opt_no_param)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
	ENDIF(cxx_opt_no_param)
ENDIF(${CMAKE_CXX_FLAGS} STREQUAL " ")

SET(FWEB_CONF_PATH ${CMAKE_INSTALL_PREFIX}/fweb.ini)


SET(FWEB_SRC 
	fweb.c fwebhttp.c cgic.c mempool.c fwebio.c
	dispatcher.c fweblog.c module.c fwebtpl.cc config_reader.c
	)
ADD_EXECUTABLE(${FWEB2NAME} ${FWEB_SRC})
CHECK_REQURIED_INCLUDE("fcgiapp.h" HAVE_FCGI)
LINK_LIBRARY_OR_ERROR(fcgi FCGX_Init HAVE_LIB_FCGI)
CHECK_REQURIED_INCLUDE_CXX("google/template.h" HAVE_CTPL)
LINK_LIBRARY_OR_ERROR(ctemplate_nothreads _init HAVE_LIB_CTPL)


SET(CMAKE_REQUIRED_INCLUDES "${PROJECT_SOURCE_DIR}/../fb2k/include")
CHECK_REQURIED_INCLUDE("bbs.h" HAVE_BBSH)
SET(CMAKE_REQURIED_INCLUDES "")

ADD_SRC_FILE_PROP(fweb.c COMPILE_FLAGS "'-DFWEB_CONF_PATH=\"${FWEB_CONF_PATH}\"'")
SET_SRC_FILE_G_LOG_DOMAIN("" "${FWEB_SRC}" )

pkg_check_modules(GLIB REQUIRED glib-2.0 gmodule-2.0)

FOREACH(GLIB_DIR ${GLIB_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(AFTER ${GLIB_DIR})
ENDFOREACH(GLIB_DIR)
FOREACH(GLIB_CFLAG ${GLIB_CFLAGS_OTHERS})
	ADD_TARGET_PROP(${FWEB2NAME} COMPILE_FLAGS ${GLIB_CFLAG})
ENDFOREACH(GLIB_CFLAG)
FOREACH(GLIB_LDFLAG ${GLIB_LDFLAGS})
	ADD_TARGET_PROP(${FWEB2NAME} LINK_FLAGS ${GLIB_LDFLAG})
ENDFOREACH(GLIB_LDFLAG)

CHECK_CXX_ACCEPTS_FLAG("-Wl,--fatal-warnings" cxx_opt_wlfatal)
IF(cxx_opt_wlfatal)
	ADD_TARGET_PROP(${FWEB2NAME} LINK_FLAGS "-Wl,--fatal-warnings")
ENDIF(cxx_opt_wlfatal)


INCLUDE_DIRECTORIES(AFTER  ../fb2k/include /usr/local/include)
LINK_DIRECTORIES( /usr/local/lib )
INSTALL(TARGETS ${FWEB2NAME} RUNTIME DESTINATION .)
INSTALL(FILES fweb.ini DESTINATION . RENAME fweb.ini.default)

SUBDIRS(tpl)
SUBDIRS(modules bbswww ntwww)
