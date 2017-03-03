#!/bin/sh -ex

CEPH_REF=${CEPH_REF:-master}
#wget -q https://raw.github.com/ceph/ceph/$CEPH_REF/src/test/pybind/test_rbd.py
GIT_URL="https://code.engineering.redhat.com/gerrit/p/ceph.git"
wget --no-check-certificate -O test_rbd.py "$GIT_URL;a=blob_plain;hb=$CEPH_REF;f=src/test/pybind/test_rbd.py" || \
    wget --no-check-certificate -O test_rbd.py "$GIT_URL;a=blob_plain;hb=ref/heads/$CEPH_REF;f=src/test/pybind/test_rbd.py"

if [ -n "${VALGRIND}" ]; then
  valgrind --tool=${VALGRIND} --suppressions=${TESTDIR}/valgrind.supp \
    nosetests -v test_rbd
else
  nosetests -v test_rbd
fi
