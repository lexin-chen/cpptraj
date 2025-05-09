add_subdirectory(xdrfile)
if(NOT INSIDE_AMBER)
  if(readline_INTERNAL)
    add_subdirectory(readline)
  endif()
  if(arpack_INTERNAL)
    add_subdirectory(arpack)
  endif()
  if(tng_io_INTERNAL)
    add_subdirectory(tng)
  endif()
  add_definitions(-DBUILDTYPE="GitHub")
else()
  add_definitions(-DBUILDTYPE="AmberTools")
endif()

#---------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Parse cpptrajfiles

# read each non-empty line into an element of a list
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/cpptrajfiles CPPTRAJFILES_CONTENTS)

# get rid of backslashes
string(REPLACE "\\" "" CPPTRAJFILES_CONTENTS "${CPPTRAJFILES_CONTENTS}")

# name of list that we are curreently appending to
set(LIST_NAME "")

foreach(LINE ${CPPTRAJFILES_CONTENTS})

	# ignore comment lines
	if(NOT "${LINE}" MATCHES "^#")

		# extract the name of the source file mentioned in the line (a string after whitespace or an equals sign)
		string(REGEX MATCH "[^ :=]+\.(o|cpp|c|LIBCPPTRAJ\.o)" SOURCE_FILE_NAME "${LINE}")
		
		# get name of variable that the following list is being set to
		# must exclude parentheses so that we don't match dereferences of other variables
		string(REGEX MATCH "[^$\(\)]+=" VARIABLE_NAME "${LINE}")
		
		# if we are starting a new source list, update LIST_NAME accordingly
		if(NOT "${VARIABLE_NAME}" STREQUAL "")
			string(REPLACE "=" "" VARIABLE_NAME "${VARIABLE_NAME}")
			set(LIST_NAME ${VARIABLE_NAME})
		endif()
		
		# did we get a new source file?
		if(NOT "${SOURCE_FILE_NAME}" STREQUAL "")
			
			if("${LIST_NAME}" STREQUAL "")
				message(FATAL_ERROR "cpptrajfiles parser error: got source files before any source lists!")
			endif()
			
			# get rid of LIBCPPTRAJ.o suffix if it exists
			string(REPLACE "LIBCPPTRAJ.o" "cpp" SOURCE_FILE_NAME "${SOURCE_FILE_NAME}")
			
			# also convert .o to .cpp (used in some variables)
			string(REPLACE ".o" ".cpp" SOURCE_FILE_NAME "${SOURCE_FILE_NAME}")

			
			list(APPEND ${LIST_NAME} ${SOURCE_FILE_NAME})
		endif()
			
		#message("\"${LINE}\" - SFN: \"${SOURCE_FILE_NAME}\" - VN: \"${VARIABLE_NAME}\"")
	endif()
endforeach()

# The above loop will create the folowing variables:
# COMMON_SOURCES - all C++ source files used for both cpptraj and libcpptraj, that are compiled the same way for both
# CSOURCES - all C source files used for cpptraj and libcpptraj
# SOURCES - C++ sources for cpptraj only
# LIBCPPTRAJ_OBJECTS - C++ sources for libcpptraj that should be compiled with the LIBCPPTRAJ definition 
# LIBCPPTRAJ_CORE_OBJECTS - C++ sources which contain "core" functionality.
# LIBCPPTRAJ_FILE_OBJECTS - C++ sources which contain basic file-related functionality. Requires core library
# LIBCPPTRAJ_TRAJ_OBJECTS - C++ sources which contain trajectory file functionality. Requires core and file libraries, as well as libxdrfile.
# LIBCPPTRAJ_PARM_OBJECTS - C++ sources which contain parameter file functionality. Requires core and file libraries.

# pub_fft.F90 is not in the source lists
set(PUBFFT_FORTRAN_SOURCE pub_fft.F90)

#---------------------------------------------------------------------------------------------------------------------------------------------------------------------

# with icc, cpptraj needs -fp-model source in order to produce floating point results that match gcc
set(FP_SOURCE_FLAG "")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	check_cxx_compiler_flag("-fp-model source" FP_MODEL_SOURCE_WORKS)

	if(FP_MODEL_SOURCE_WORKS)
		set(FP_SOURCE_FLAG "-fp-model source")
	endif()
endif()

# all sources should use optimized compile flags
set_property(SOURCE ${PUBFFT_FORTRAN_SOURCE} PROPERTY COMPILE_FLAGS "${OPT_FFLAGS_SPC}")
set_property(SOURCE ${COMMON_SOURCES} ${SOURCES} ${LIBCPPTRAJ_OBJECTS} PROPERTY COMPILE_FLAGS "${OPT_CXXFLAGS_SPC} ${FP_SOURCE_FLAG}")
set_property(SOURCE ${CSOURCES} PROPERTY COMPILE_FLAGS "${OPT_CFLAGS_SPC}")

include_directories(${AMBERTOOLS_INC_DIR})

if(fftw_ENABLED)	
	set_property(SOURCE PubFFT.cpp PROPERTY COMPILE_DEFINITIONS FFTW_FFT)
endif()

#---------------------------------------------------------------------------------------------------------------------------------------------------------------------
#add the common object library

#concatenate all the source files
set(CPPTRAJ_COMMON_SOURCES ${COMMON_SOURCES} ${CSOURCES})

if(fftw_DISABLED AND mkl_DISABLED)
	# we only need pubfft if we don't have FFTW/MKL
	list(APPEND CPPTRAJ_COMMON_SOURCES ${PUBFFT_FORTRAN_SOURCE})
endif()

add_library(cpptraj_common_obj OBJECT ${CPPTRAJ_COMMON_SOURCES})
add_subdirectory(Cluster)
add_subdirectory(Structure)
add_subdirectory(Energy)
make_pic_if_needed(cpptraj_common_obj)

#normally this would be applied by target_link_libraries, but since we use that intermediary object library, we have to apply it manually

# NOTE: there is a CMake bug where if we were to set these as a directory-scope includes, the CUDA build would fail on some platforms with old versions of CMake
# it turns out that CMake's cuda library passes the include paths after the first one from each of these generator expressions to nvcc without the -I flag
# This causes the error "A single input file is required for a non-link phase when an outputfile is specified"
target_include_directories(cpptraj_common_obj PRIVATE $<TARGET_PROPERTY:xdrfile,INTERFACE_INCLUDE_DIRECTORIES> $<TARGET_PROPERTY:netcdf,INTERFACE_INCLUDE_DIRECTORIES>)
 
#---------------------------------------------------------------------------------------------------------------------------------------------------------------------
# cpptraj executable

add_executable(cpptraj $<TARGET_OBJECTS:cpptraj_common_obj> ${SOURCES})

target_link_libraries(cpptraj netcdf netlib xdrfile)

install(TARGETS cpptraj DESTINATION ${BINDIR})
#------------------------------------------------------------------------------------------
# libcpptraj library

add_library(libcpptraj $<TARGET_OBJECTS:cpptraj_common_obj> ${LIBCPPTRAJ_OBJECTS})
set_property(TARGET libcpptraj PROPERTY COMPILE_DEFINITIONS LIBCPPTRAJ)

target_link_libraries(libcpptraj netlib netcdf xdrfile)
remove_prefix(libcpptraj)
install_libraries(libcpptraj)

#tell others where to find the cpptraj includes
target_include_directories(libcpptraj INTERFACE .)

#------------------------------------------------------------------------------------------
# Static libraries
# (not installed, used by other targets inside Amber which only need a subset of cpptraj functionality)
add_library(cpptraj_core STATIC ${LIBCPPTRAJ_CORE_OBJECTS})
add_library(cpptraj_file STATIC ${LIBCPPTRAJ_FILE_OBJECTS})
add_library(cpptraj_traj STATIC ${LIBCPPTRAJ_TRAJ_OBJECTS})
add_library(cpptraj_parm STATIC ${LIBCPPTRAJ_PARM_OBJECTS})

# all libraries include the current dir as an interface directory
targets_include_directories(cpptraj_core cpptraj_file cpptraj_traj cpptraj_parm DIRECTORIES INTERFACE .)

# cpptraj_traj needs xdrfile and netcdf
target_include_directories(cpptraj_traj PRIVATE $<TARGET_PROPERTY:xdrfile,INTERFACE_INCLUDE_DIRECTORIES> $<TARGET_PROPERTY:netcdf,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(cpptraj_traj xdrfile netcdf)

# all libs need netlib
targets_link_libraries(cpptraj_core cpptraj_file cpptraj_traj cpptraj_parm LIBRARIES netlib)

#------------------------------------------------------------------------------------------
# DLL exports/imports

if(SHARED)
	# CMake automatically sets up the libcpptraj_EXPORTS definition for libcpptraj.
	# We just have to apply it to the common obj as well.
	target_compile_definitions(cpptraj_common_obj PRIVATE libcpptraj_EXPORTS)

	# now set up the definition for other people to use
	target_compile_definitions(libcpptraj INTERFACE CPPTRAJ_USE_DLL)
endif()

#------------------------------------------------------------------------------------------
# Header installation

#if(INSTALL_HEADERS)
	# grab all .h files from the main directory.
	file(GLOB CPPTRAJ_HEADERS "*.h")
	list(REMOVE_ITEM CPPTRAJ_HEADERS "SymbolExporting.h")
	
	# also grab xdrfile headers since some of them are used by cpptraj headers
	#file(GLOB XDRFILE_HEADERS "xdrfile/*.h")
        file(GLOB CLUSTER_HEADERS "Cluster/*.h")
        file(GLOB STRUCTURE_HEADERS "Structure/*.h")
        file(GLOB ENERGY_HEADERS "Energy/*.h")

        if(INSIDE_AMBER)
	  install(FILES ${CPPTRAJ_HEADERS} DESTINATION ${INCDIR}/cpptraj)
	  #install(FILES ${XDRFILE_HEADERS} DESTINATION ${INCDIR}/cpptraj/xdrfile)
          install(FILES ${CLUSTER_HEADERS} DESTINATION ${INCDIR}/cpptraj/Cluster)
          install(FILES ${STRUCTURE_HEADERS} DESTINATION ${INCDIR}/cpptraj/Structure)
          install(FILES ${ENERGY_HEADERS} DESTINATION ${INCDIR}/cpptraj/Energy)
        else()
	  install(FILES ${CPPTRAJ_HEADERS} DESTINATION ${INCDIR})
	  #install(FILES ${XDRFILE_HEADERS} DESTINATION ${INCDIR}/xdrfile)
          install(FILES ${CLUSTER_HEADERS} DESTINATION ${INCDIR}/Cluster)
          install(FILES ${STRUCTURE_HEADERS} DESTINATION ${INCDIR}/Structure)
          install(FILES ${ENERGY_HEADERS} DESTINATION ${INCDIR}/Energy)
        endif()
	
	# configure SymbolExporting.h specially for the current install type
	if(SHARED)
		set(CPPTRAJ_IS_SHARED 1)
	else()
		set(CPPTRAJ_IS_SHARED 0)
	endif()
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/SymbolExporting-installversion.h.in ${CMAKE_CURRENT_BINARY_DIR}/SymbolExporting-installversion.h @ONLY)
	
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SymbolExporting-installversion.h DESTINATION ${INCDIR}/cpptraj RENAME SymbolExporting.h)
	
#endif()

# Check for C++11 support
set(HAS_C11_SUPPORT FALSE)
foreach(cxxfeat ${CMAKE_CXX_COMPILE_FEATURES})
  if(${cxxfeat} STREQUAL cxx_std_11)
    # C++11 support found
    message(STATUS "Found C++11 support: ${cxxfeat}")
    add_definitions(-DC11_SUPPORT)
    set(HAS_C11_SUPPORT TRUE)
    set_property(TARGET cpptraj PROPERTY CXX_STANDARD 11)
    break()
  endif()
endforeach()

#---------------------------------------------------------------------------------------------------------------------------------------------------------------------
#Deal with external libraries

# NOTE: you CANNOT set any directory-scope include directories that use generator expressions here.
# These expressions get propagated down into the cuda_kernels subdir, and trigger an undocumented bug in old versions of CMake
# where they get passed to nvcc without a -I prefix, breaking everything.  So please don't do it.

if(libbz2_ENABLED)
	add_definitions(-DHASBZ2)
	include_directories(${BZIP2_INCLUDE_DIR})
	targets_link_libraries(cpptraj cpptraj_file libcpptraj LIBRARIES BZip2::BZip2)
endif()

if(zlib_ENABLED)
	add_definitions(-DHASGZ)
	include_directories(${ZLIB_INCLUDE_DIRS})
	targets_link_libraries(cpptraj cpptraj_file libcpptraj LIBRARIES ZLIB::ZLIB)
endif()

if(fftw_ENABLED)
        add_definitions(-DHAVE_FFTWD=1)
        add_definitions(-DFFTW_FFT)
        # LibPME not yet supported on windows
        if(NOT MSVC AND HAS_C11_SUPPORT)
          add_definitions(-DLIBPME)
        endif()
	target_include_directories(cpptraj_common_obj PRIVATE $<TARGET_PROPERTY:fftw,INTERFACE_INCLUDE_DIRECTORIES>)	
	targets_link_libraries(cpptraj libcpptraj LIBRARIES fftw)
elseif(mkl_ENABLED)
  # Use FFTW from MKL
  add_definitions(-DHAVE_FFTWD=1)
  add_definitions(-DFFTW_FFT)
  # LibPME not yet supported on windows
  if(NOT MSVC AND HAS_C11_SUPPORT)
    add_definitions(-DLIBPME)
  endif()
endif()

#readline
if(readline_ENABLED)
	targets_link_libraries(cpptraj libcpptraj LIBRARIES readline)
else()
	target_compile_definitions(cpptraj PRIVATE NO_READLINE)
	target_compile_definitions(libcpptraj PRIVATE NO_READLINE)
endif()

if(tng_io_ENABLED)
	add_definitions(-DHAS_TNGFILE)
	include_directories($<TARGET_PROPERTY:tng_io,INTERFACE_INCLUDE_DIRECTORIES>)
	targets_link_libraries(cpptraj cpptraj_file libcpptraj LIBRARIES tng_io)
endif()

# libsander
if(INSIDE_AMBER AND ("${AMBER_TOOLS}" MATCHES "sander" AND BUILD_SANDER_API))
	#add the sander-specific definitions and libraries
	target_compile_definitions(cpptraj PRIVATE USE_SANDERLIB)
	target_link_libraries(cpptraj libsander)
endif()

# arpack
if(arpack_DISABLED)
	add_definitions(-DNO_ARPACK)
else()
	targets_link_libraries(cpptraj libcpptraj LIBRARIES arpack)
endif()

# --------------------------------------------------------------------
# Parallel Versions
# --------------------------------------------------------------------

if(MPI)
	make_mpi_version(cpptraj_common_obj cpptraj_common_obj_mpi LANGUAGES CXX)
	make_mpi_version(cpptraj cpptraj.MPI LANGUAGES CXX SWAP_SOURCES $<TARGET_OBJECTS:cpptraj_common_obj> TO $<TARGET_OBJECTS:cpptraj_common_obj_mpi> INSTALL)
	
	set_property(TARGET cpptraj.MPI cpptraj_common_obj_mpi APPEND PROPERTY COMPILE_DEFINITIONS MPI) # since we use CXX mpi, we have to define this manually
			
	if(pnetcdf_ENABLED)
		targets_link_libraries(cpptraj.MPI LIBRARIES pnetcdf)
		target_include_directories(cpptraj_common_obj_mpi PUBLIC $<TARGET_PROPERTY:pnetcdf,INTERFACE_INCLUDE_DIRECTORIES>)
		set_property(TARGET cpptraj.MPI cpptraj_common_obj_mpi APPEND PROPERTY COMPILE_DEFINITIONS HAS_PNETCDF) 		
	endif()
endif()

if(OPENMP)
	make_openmp_version(cpptraj_common_obj cpptraj_common_obj_openmp LANGUAGES CXX)
	make_openmp_version(cpptraj cpptraj.OMP LANGUAGES CXX SWAP_SOURCES $<TARGET_OBJECTS:cpptraj_common_obj> TO $<TARGET_OBJECTS:cpptraj_common_obj_openmp> INSTALL)
	make_openmp_version(libcpptraj libcpptraj_omp LANGUAGES CXX SWAP_SOURCES $<TARGET_OBJECTS:cpptraj_common_obj> TO $<TARGET_OBJECTS:cpptraj_common_obj_openmp> INSTALL)
endif()

if(BUILD_PARALLEL_COMBINATIONS AND (MPI AND OPENMP))
	make_openmp_version(cpptraj_common_obj_mpi cpptraj_common_obj_mpi_openmp LANGUAGES CXX)
	make_openmp_version(cpptraj.MPI cpptraj.MPI.OMP LANGUAGES CXX SWAP_SOURCES $<TARGET_OBJECTS:cpptraj_common_obj_mpi> TO $<TARGET_OBJECTS:cpptraj_common_obj_mpi_openmp> INSTALL)
endif()
	
# CUDA
if(CUDA)
	add_subdirectory(cuda_kernels)
	
	include_directories(${CUDA_INCLUDE_DIRS})
	
	copy_target(cpptraj_common_obj cpptraj_common_obj_cuda)
	target_compile_definitions(cpptraj_common_obj_cuda PRIVATE CUDA)
	
	copy_target(cpptraj cpptraj.cuda SWAP_SOURCES $<TARGET_OBJECTS:cpptraj_common_obj> TO $<TARGET_OBJECTS:cpptraj_common_obj_cuda>)
	copy_target(libcpptraj libcpptraj_cuda SWAP_SOURCES $<TARGET_OBJECTS:cpptraj_common_obj> TO $<TARGET_OBJECTS:cpptraj_common_obj_cuda>)
	
	target_compile_definitions(cpptraj.cuda PRIVATE CUDA)
	target_compile_definitions(libcpptraj_cuda PRIVATE CUDA)
	
	targets_link_libraries(cpptraj.cuda libcpptraj_cuda LIBRARIES cpptraj_cuda_routines)
	
	install(TARGETS cpptraj.cuda DESTINATION ${BINDIR} COMPONENT CUDA)

	if(readline_ENABLED)
	  targets_link_libraries(cpptraj.cuda libcpptraj_cuda LIBRARIES readline)
        endif()
        if(arpack_ENABLED)
	  targets_link_libraries(cpptraj.cuda libcpptraj_cuda LIBRARIES arpack)
        endif()
        if(tng_io_ENABLED)
	  targets_link_libraries(cpptraj.cuda libcpptraj_cuda LIBRARIES tng_io)
        endif()


	if(BUILD_PARALLEL_COMBINATIONS AND MPI)
		make_mpi_version(cpptraj_common_obj_cuda cpptraj_common_obj_mpi_cuda LANGUAGES CXX)
		make_mpi_version(cpptraj.cuda cpptraj.MPI.cuda LANGUAGES CXX SWAP_SOURCES $<TARGET_OBJECTS:cpptraj_common_obj_cuda> TO $<TARGET_OBJECTS:cpptraj_common_obj_mpi_cuda> INSTALL)
		
		set_property(TARGET cpptraj.MPI.cuda cpptraj_common_obj_mpi_cuda APPEND PROPERTY COMPILE_DEFINITIONS MPI) # since we use CXX mpi, we have to define this manually
				
		if(pnetcdf_ENABLED)
			targets_link_libraries(cpptraj.MPI.cuda LIBRARIES pnetcdf)
			target_include_directories(cpptraj_common_obj_mpi_cuda PUBLIC $<TARGET_PROPERTY:pnetcdf,INTERFACE_INCLUDE_DIRECTORIES>)
			set_property(TARGET cpptraj.MPI.cuda cpptraj_common_obj_mpi_cuda APPEND PROPERTY COMPILE_DEFINITIONS HAS_PNETCDF) 		
		endif()
	endif()

	if(BUILD_PARALLEL_COMBINATIONS AND OPENMP)
		make_openmp_version(cpptraj_common_obj_cuda cpptraj_common_obj_openmp_cuda LANGUAGES CXX)
		make_openmp_version(cpptraj.cuda cpptraj.OMP.cuda LANGUAGES CXX SWAP_SOURCES $<TARGET_OBJECTS:cpptraj_common_obj_cuda> TO $<TARGET_OBJECTS:cpptraj_common_obj_openmp_cuda> INSTALL)
		make_openmp_version(libcpptraj_cuda libcpptraj_omp_cuda LANGUAGES CXX SWAP_SOURCES $<TARGET_OBJECTS:cpptraj_common_obj_cuda> TO $<TARGET_OBJECTS:cpptraj_common_obj_openmp_cuda> INSTALL)
	endif()

	if(BUILD_PARALLEL_COMBINATIONS AND (MPI AND OPENMP))
		# THE ULTIMATE CHIMERA!!!!! Muahahahaha!
		make_openmp_version(cpptraj_common_obj_mpi_cuda cpptraj_common_obj_mpi_openmp_cuda LANGUAGES CXX)
		make_openmp_version(cpptraj.MPI.cuda cpptraj.MPI.OMP.cuda LANGUAGES CXX SWAP_SOURCES $<TARGET_OBJECTS:cpptraj_common_obj_mpi_cuda> TO $<TARGET_OBJECTS:cpptraj_common_obj_mpi_openmp_cuda> INSTALL)
	endif()
		
		
	
endif()

	
