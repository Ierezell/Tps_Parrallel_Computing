# find_package( GMP QUIET )

# if(GMP_FOUND)

#   if (GMPXX_INCLUDE_DIR AND GMPXX_LIBRARIES)
#     # Already in cache, be silent
#     set(GMPXX_FIND_QUIETLY TRUE)
#   endif()

#   find_path(GMPXX_INCLUDE_DIR NAMES gmpxx.h
#             PATHS ${GMP_INCLUDE_DIR_SEARCH}
#             DOC "The directory containing the GMPXX include files"
#            )

#   find_library(GMPXX_LIBRARIES NAMES gmpxx
#                PATHS ${GMP_LIBRARIES_DIR_SEARCH}
#                DOC "Path to the GMPXX library"
#                )



#   find_package_handle_standard_args(GMPXX "DEFAULT_MSG" GMPXX_LIBRARIES GMPXX_INCLUDE_DIR )

# endif()


if (GMPXX_INCLUDE_DIR AND GMPXX_LIBRARIES)
	# Force search at every time, in case configuration changes
	unset(GMPXX_INCLUDE_DIR CACHE)
	unset(GMPXX_LIBRARIES CACHE)
endif (GMPXX_INCLUDE_DIR AND GMPXX_LIBRARIES)

find_path(GMPXX_INCLUDE_DIR NAMES gmpxx.h)
if(STBIN)
	find_library(GMPXX_LIBRARIES NAMES libgmpxx.a gmp)
else(STBIN)
	find_library(GMPXX_LIBRARIES NAMES libgmpxx.so gmp)
endif(STBIN)

if(GMPXX_INCLUDE_DIR AND GMPXX_LIBRARIES)
   set(GMP_FOUND TRUE)
endif(GMPXX_INCLUDE_DIR AND GMPXX_LIBRARIES)

if(GMP_FOUND)
	message(STATUS "Configured GMP: ${GMPXX_LIBRARIES}")
else(GMP_FOUND)
	message(STATUS "Could NOT find GMP")
endif(GMP_FOUND)

mark_as_advanced(GMPXX_INCLUDE_DIR GMPXX_LIBRARIES)
