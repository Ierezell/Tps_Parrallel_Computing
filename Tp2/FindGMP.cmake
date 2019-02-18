if (GMP_INCLUDE_DIR AND GMP_LIBRARIES)
	# Force search at every time, in case configuration changes
	unset(GMP_INCLUDE_DIR CACHE)
	unset(GMP_LIBRARIES CACHE)
endif (GMP_INCLUDE_DIR AND GMP_LIBRARIES)

find_path(GMP_INCLUDE_DIR NAMES gmp.h)
if(STBIN)
	find_library(GMP_LIBRARIES NAMES libgmp.a gmp)
else(STBIN)
	find_library(GMP_LIBRARIES NAMES libgmp.so gmp)
endif(STBIN)

if(GMP_INCLUDE_DIR AND GMP_LIBRARIES)
   set(GMP_FOUND TRUE)
endif(GMP_INCLUDE_DIR AND GMP_LIBRARIES)

if(GMP_FOUND)
	message(STATUS "Configured GMP: ${GMP_LIBRARIES}")
else(GMP_FOUND)
	message(STATUS "Could NOT find GMP")
endif(GMP_FOUND)

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARIES)
