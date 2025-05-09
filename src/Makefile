# Cpptraj standalone Makefile
include ../config.h

include cpptrajfiles

include cpptrajheaders

# Object files from subdirectories go here
include Cluster/clusterfiles
CLUSTER_SOURCEFILES=$(CLUSTER_SOURCES:%.cpp=Cluster/%.cpp)
include Structure/structurefiles
STRUCTURE_SOURCEFILES=$(STRUCTURE_SOURCES:%.cpp=Structure/%.cpp)
include Energy/energyfiles
ENERGY_SOURCEFILES=$(ENERGY_SOURCES:%.cpp=Energy/%.cpp)

# All object files
OBJECTS=$(SOURCES:.cpp=.o) $(CSOURCES:.c=.o) $(CLUSTER_SOURCEFILES:.cpp=.o) $(STRUCTURE_SOURCEFILES:.cpp=.o) $(ENERGY_SOURCEFILES:.cpp=.o)

# General rules
.cpp.o:
	$(VB)echo CXX $<
	$(VB)$(CXX) $(DIRECTIVES) $(CPPTRAJ_INC) $(CXXFLAGS) -c -o $@ $<

.c.o:
	$(VB)echo CC $<
	$(VB)$(CC) $(DIRECTIVES) $(CPPTRAJ_INC) $(CFLAGS) -c -o $@ $<

DEL_FILE = /bin/rm -f

# For debugging
showsources:
	@echo $(CLUSTER_SOURCEFILES)
	@echo $(STRUCTURE_SOURCEFILES)
	@echo $(ENERGY_SOURCEFILES)

showobjects:
	@echo $(OBJECTS)

# Default target: cpptraj only
all: cpptraj$(SFX)$(EXE)

# Install all targets
install: $(INSTALL_TARGETS)

# Cpptraj ------------------------------
install_cpptraj: cpptraj$(SFX)$(EXE)
	$(VB)echo Install cpptraj$(SFX)$(EXE) to $(CPPTRAJBIN)
	$(VB)$(INSTALL_PROGRAM) cpptraj$(SFX)$(EXE) $(CPPTRAJBIN)

cpptraj$(SFX)$(EXE): $(OBJECTS) $(FFT_TARGET) $(READLINE_TARGET) $(CUDA_TARGET) $(XDRFILE_TARGET) $(ARPACK_TARGET) $(TNGFILE_TARGET)
	$(VB)echo Link cpptraj$(SFX)$(EXE)
	$(VB)$(CXX) -o cpptraj$(SFX)$(EXE) $(OBJECTS) $(CUDA_TARGET) $(FFT_TARGET) $(READLINE_LIB) $(CPPTRAJ_LIB) $(LDFLAGS)

# libcpptraj ---------------------------
# Rule to make libcpptraj-specific objects
%.LIBCPPTRAJ.o : %.cpp
	$(VB)echo [LIBCPPTRAJ] CXX $<
	$(VB)$(CXX) $(DIRECTIVES) -DLIBCPPTRAJ $(CPPTRAJ_INC) $(CXXFLAGS) -c -o $@ $<

libcpptraj: $(LIBCPPTRAJ_TARGET)

$(CPPTRAJLIB)/libcpptraj$(SHARED_SUFFIX): $(LIBCPPTRAJ_OBJECTS) $(FFT_TARGET) $(CUDA_TARGET) $(XDRFILE_TARGET) $(ARPACK_TARGET) $(TNGFILE_TARGET) install_headers
	$(VB)echo Install $(CPPTRAJLIB)/libcpptraj$(SHARED_SUFFIX)
	$(VB)$(CXX) -shared -o $(CPPTRAJLIB)/libcpptraj$(SHARED_SUFFIX) $(LIBCPPTRAJ_OBJECTS) $(FFT_TARGET) $(CUDA_TARGET) $(CPPTRAJ_LIB) $(LDFLAGS)

# Data directory -----------------------
install_dat:
	-$(INSTALL_DATA) ../dat/ATOMTYPE_CHECK.TAB $(CPPTRAJDAT)/
	-$(INSTALL_DATA) ../dat/BONDTYPE_CHECK.TAB $(CPPTRAJDAT)/
	-$(INSTALL_DATA) ../dat/Karplus.txt        $(CPPTRAJDAT)/
	-$(INSTALL_DATA) ../dat/README             $(CPPTRAJDAT)/
	-$(INSTALL_DATA) ../dat/Carbohydrate_PDB_Glycam_Names.txt $(CPPTRAJDAT)/
	-$(INSTALL_DATA) ../dat/PDB_ResidueNames.txt $(CPPTRAJDAT)/

# Headers ------------------------------
$(CPPTRAJINC):
	$(VB)-mkdir -p $(CPPTRAJINC)

install_headers: $(CPPTRAJINC)
	$(VB) echo Installing CPPTRAJ headers to $(CPPTRAJINC)
	$(VB)-for file in $(CPPTRAJ_HEADERS) ; do \
	  $(INSTALL) -D $${file} $(CPPTRAJINC)/$${file} ; \
	done

# Static libraries ---------------------
#$(CPPTRAJLIB)/libcpptraj.a: $(LIBCPPTRAJ_OBJECTS) $(FFT_TARGET) $(CUDA_TARGET)
#	ar rv $(CPPTRAJLIB)/libcpptraj.a $(LIBCPPTRAJ_OBJECTS) $(FFT_TARGET) $(CUDA_TARGET)

$(CPPTRAJLIB)/libcpptraj_core.a: $(LIBCPPTRAJ_CORE_OBJECTS)
	ar rv $(CPPTRAJLIB)/libcpptraj_core.a $(LIBCPPTRAJ_CORE_OBJECTS)

$(CPPTRAJLIB)/libcpptraj_file.a: $(LIBCPPTRAJ_FILE_OBJECTS)
	ar rv $(CPPTRAJLIB)/libcpptraj_file.a $(LIBCPPTRAJ_FILE_OBJECTS)

$(CPPTRAJLIB)/libcpptraj_traj.a: $(LIBCPPTRAJ_TRAJ_OBJECTS) $(XDRFILE_OBJECTS) $(TNGFILE_OBJECTS) $(VMDPLUGIN_OBJECTS)
	ar rv $(CPPTRAJLIB)/libcpptraj_traj.a $(LIBCPPTRAJ_TRAJ_OBJECTS) $(XDRFILE_OBJECTS) $(TNGFILE_OBJECTS) $(VMDPLUGIN_OBJECTS)

$(CPPTRAJLIB)/libcpptraj_parm.a: $(LIBCPPTRAJ_PARM_OBJECTS)
	ar rv $(CPPTRAJLIB)/libcpptraj_parm.a $(LIBCPPTRAJ_PARM_OBJECTS)

libstatic: $(CPPTRAJLIB)/libcpptraj_core.a $(CPPTRAJLIB)/libcpptraj_traj.a \
           $(CPPTRAJLIB)/libcpptraj_file.a $(CPPTRAJLIB)/libcpptraj_parm.a

nolibcpptraj:
	@echo ""
	@echo "Error: Cannot build libcpptraj; re-configure with '-shared'"
	@echo ""
	@exit 1

# Internal object/library targets
pub_fft.o:  pub_fft.F90
	$(VB)echo FC pub_fft.F90
	$(VB)$(FC) -c $(FFLAGS) -o $@ pub_fft.F90

readline/libreadline.a:
	cd readline && $(MAKE) all

xdrfile/libxdrfile.a:
	cd xdrfile && $(MAKE) all

arpack/libarpack.a:
	cd arpack && $(MAKE) install

tng/libtng_io.a:
	cd tng && $(MAKE) all

noreadline:
	@echo ""
	@echo "Skipping bundled READLINE build"
	@echo ""

noxdrfile:
	@echo ""
	@echo "Skipping bundled XDRFILE build"
	@echo ""

notngfile:
	@echo ""
	@echo "Skipping bundled TNG build"
	@echo ""

noarpack:
	@echo ""
	@echo "Skipping bundled ARPACK build"
	@echo ""

cuda_kernels/libcpptraj_cuda.a::
	cd cuda_kernels && $(MAKE) all

# Dependency targets
findDepend: FindDepend.cpp FindDepend.o 
	$(CXX) -o findDepend FindDepend.o

depend: findDepend
	./findDepend $(SOURCES) $(CSOURCES) $(CLUSTER_SOURCEFILES) $(STRUCTURE_SOURCEFILES) $(ENERGY_SOURCEFILES) > cpptrajdepend

dependclean:
	$(DEL_FILE) FindDepend.o findDepend

# Clean/uninstall targets
clean:
	$(DEL_FILE) $(OBJECTS) $(FFT_TARGET) cpptraj$(SFX) *.LIBCPPTRAJ.o libcpptraj$(SHARED_SUFFIX)
	cd readline && $(MAKE) clean
	cd xdrfile && $(MAKE) clean
	cd arpack && $(MAKE) clean
	cd cuda_kernels && $(MAKE) clean
	cd Cluster && $(MAKE) clean
	cd Structure && $(MAKE) clean
	cd Energy && $(MAKE) clean
	cd tng && $(MAKE) clean

uninstall_lib:
	$(DEL_FILE) $(CPPTRAJLIB)/*.a
	$(DEL_FILE) $(CPPTRAJLIB)/*.la
	$(DEL_FILE) $(CPPTRAJLIB)/*.so*
	$(DEL_FILE) $(CPPTRAJLIB)/*.settings
	$(DEL_FILE) -r $(CPPTRAJLIB)/cmake
	$(DEL_FILE) -r $(CPPTRAJLIB)/pkgconfig

uninstall_inc:
	$(DEL_FILE) $(CPPTRAJINC)/*.h
	$(DEL_FILE) $(CPPTRAJINC)/*.f
	$(DEL_FILE) $(CPPTRAJINC)/*.f03

uninstall: uninstall_lib uninstall_inc
	$(DEL_FILE) $(CPPTRAJBIN)/cpptraj$(SFX)$(EXE)
	$(DEL_FILE) $(CPPTRAJLIB)/libcpptraj$(SHARED_SUFFIX)
	cd readline && $(MAKE) uninstall
	cd xdrfile && $(MAKE) uninstall
	cd cuda_kernels && $(MAKE) uninstall
	cd Cluster && $(MAKE) uninstall
	cd Structure && $(MAKE) uninstall
	cd Energy && $(MAKE) uninstall

# Header dependencies
include cpptrajdepend
