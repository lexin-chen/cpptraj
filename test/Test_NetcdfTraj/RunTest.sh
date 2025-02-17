#!/bin/bash

. ../MasterTest.sh

# Clean
CleanFiles ptraj_netcdf.in trajectory.netcdf trajectory_test.mdcrd \
            trajectory_nc4.mdcrd trajectory.nc4 \
            compress.mdcrd compress.nc4 \
            icompress.mdcrd icompress.nc4 \
            velfrc.compress.nc4 trpzip2.*.crd \
            velfrc.icompress.nc4

TESTNAME='NetCDF tests'
Requires netcdf pnetcdf maxthreads 10

# Convert MDCRD to NETCDF and back again.
INPUT="ptraj_netcdf.in"
TOP="../tz2.truncoct.parm7"

cat > $INPUT <<EOF
trajin ../tz2.truncoct.crd
trajout trajectory.netcdf netcdf
EOF
RunCpptraj "Convert mdcrd -> NetCDF"
cat > $INPUT <<EOF
trajin trajectory.netcdf
trajout trajectory_test.mdcrd title "trajectory generated by ptraj"
EOF
RunCpptraj "Convert NetCDF -> mdcrd"
DoTest ../tz2.truncoct.crd trajectory_test.mdcrd

# ----- NetCDF4/HDF5 tests -----------------------
UNITNAME='Convert mdcrd <-> NetCDF4/HDF5'
CheckFor hdf5 maxthreads 1
if [ $? -eq 0 ] ; then
  cat > $INPUT <<EOF
trajin ../tz2.truncoct.crd
trajout trajectory.nc4 netcdf hdf5
EOF
  RunCpptraj "Convert mdcrd -> NetCDF4/HDF5"
  cat > $INPUT <<EOF
trajin trajectory.nc4
trajout trajectory_nc4.mdcrd title "trajectory generated by ptraj"
EOF
  RunCpptraj "Convert NetCDF4/HDF5 -> mdcrd"
  DoTest ../tz2.truncoct.crd trajectory_nc4.mdcrd
fi

UNITNAME='Convert mdcrd <-> NetCDF4/HDF5 with compression'
CheckFor hdf5 maxthreads 1
if [ $? -eq 0 ] ; then
  cat > $INPUT <<EOF
trajin ../tz2.truncoct.crd
trajout compress.nc4 netcdf hdf5 compress
EOF
  RunCpptraj "Convert mdcrd -> NetCDF4/HDF5 with compression"
  cat > $INPUT <<EOF
trajin compress.nc4
trajout compress.mdcrd title "trajectory generated by ptraj"
EOF
  RunCpptraj "Convert compressed NetCDF4/HDF5 -> mdcrd"
  DoTest ../tz2.truncoct.crd compress.mdcrd
fi

UNITNAME='Convert mdcrd <-> NetCDF4/HDF5 with lossy compression'
CheckFor hdf5 maxthreads 1
if [ $? -eq 0 ] ; then
  cat > $INPUT <<EOF
trajin ../tz2.truncoct.crd
trajout icompress.nc4 netcdf hdf5 icompress
EOF
  RunCpptraj "Convert mdcrd -> NetCDF4/HDF5 with lossy compression"
  cat > $INPUT <<EOF
trajin icompress.nc4
trajout icompress.mdcrd title "trajectory generated by ptraj"
EOF
  RunCpptraj "Convert lossy compressed NetCDF4/HDF5 -> mdcrd"
  # NOTE: An absolute tolerance of 0.0001 (which is greater than the
  #       possible precision of MDCRD format) is used to allow for
  #       the fact that a -0.000 will be converted to 0.000 by the
  #       lossy compression.
  DoTest ../tz2.truncoct.crd icompress.mdcrd -a 0.0001
fi

TOP=''
INPUT="-i ptraj_netcdf.in"
UNITNAME='Test compression of velocity/force info'
CheckFor hdf5 maxthreads 1
if [ $? -eq 0 ] ; then
  cat > ptraj_netcdf.in <<EOF
parm ../trpzip2.ff14SB.mbondi3.parm7
#debug 2
trajin ../trpzip2.ff14SB.mbondi3.nc
trajout velfrc.compress.nc4 netcdf hdf5 compress
EOF
  RunCpptraj "Compress velocity/force info"
  cat > ptraj_netcdf.in <<EOF
parm ../trpzip2.ff14SB.mbondi3.parm7
set TRJ = velfrc.compress.nc4
trajin \$TRJ
trajout trpzip2.pos.crd
run
clear trajin
trajin \$TRJ usevelascoords
trajout trpzip2.vel.crd
run
clear trajin
trajin \$TRJ usefrcascoords
trajout trpzip2.frc.crd
run
EOF
  RunCpptraj "Decompress velocity/force info"
  DoTest ../Test_VelFrc/trpzip2.pos.crd.save trpzip2.pos.crd
  DoTest ../Test_VelFrc/trpzip2.vel.crd.save trpzip2.vel.crd
  DoTest ../Test_VelFrc/trpzip2.frc.crd.save trpzip2.frc.crd
fi

UNITNAME='Test lossy compression of velocity/force info'
CheckFor hdf5 maxthreads 1
if [ $? -eq 0 ] ; then
  cat > ptraj_netcdf.in <<EOF
parm ../trpzip2.ff14SB.mbondi3.parm7
#debug 2
trajin ../trpzip2.ff14SB.mbondi3.nc
trajout velfrc.icompress.nc4 netcdf hdf5 icompress
EOF
  RunCpptraj "Integer compress velocity/force info"
  cat > ptraj_netcdf.in <<EOF
parm ../trpzip2.ff14SB.mbondi3.parm7
set TRJ = velfrc.icompress.nc4
trajin \$TRJ
trajout trpzip2.ipos.crd
run
clear trajin
trajin \$TRJ usevelascoords
trajout trpzip2.ivel.crd
run
clear trajin
trajin \$TRJ usefrcascoords
trajout trpzip2.ifrc.crd
run
EOF
  RunCpptraj "Decompress lossy velocity/force info"
  # Max error allowed is 0.001. There are 10x more frames here compared to the
  # above lossy test, as well as velocity/force info.
  DoTest ../Test_VelFrc/trpzip2.pos.crd.save trpzip2.ipos.crd -a 0.002
  DoTest ../Test_VelFrc/trpzip2.vel.crd.save trpzip2.ivel.crd -a 0.002
  DoTest ../Test_VelFrc/trpzip2.frc.crd.save trpzip2.ifrc.crd -a 0.002
fi

EndTest

exit 0
