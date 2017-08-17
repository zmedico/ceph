.. _ceph-volume-lvm-activate:

``activate``
============
Once :ref:`ceph-volume-lvm-prepare` is completed, and all the various steps
that entails are done, the volume is ready to get "activated".

This activation process enables a systemd unit that persists the OSD ID and its
UUID (also called ``fsid`` in Ceph CLI tools), so that at boot time it can
understand what OSD is enabled and needs to be mounted.

.. note:: The execution of this call is fully idempotent, and there is no
          side-effects when running multiple times

New OSDs
--------
To activate newly prepared OSDs both the :term:`OSD id` and :term:`OSD uuid`
need to be supplied. For example::

    ceph-lvm activate 0 0263644D-0BF1-4D6D-BC34-28BD98AE3BC8

.. note:: The UUID is stored in the ``osd_fsid`` file in the OSD path, which is
          generated when :ref:`ceph-volume-lvm-prepare` is used.

requiring uuids
^^^^^^^^^^^^^^^
The :term:`OSD uuid` is being required as an extra step to ensure that the
right OSD is being activated. It is entirely possible that a previous OSD with
the same id exists and would end up activating the incorrect one.

Existing OSDs
-------------
For existing clusters that want to use this new system and have OSDs that are
already running there are a few things to take into account:

* OSD paths should follow this convention::

     /var/lib/ceph/osd/<cluster name>-<osd id>

* Preferably, no other mechanisms to mount the volume should exist
* There is currently no support for encrypted volumes

The one time *activation* process for an existing OSD, with an ID of 0 and
using a `"ceph"` cluster name would look like::

    ceph-lvm activate /var/lib/ceph/osd/ceph-0

The command line tool will introspect the path to fetch the pieces it needs so
that the LVM can store the metadata in its tags (for detailed metadata
description see :ref:`ceph-volume-lvm-tags`).


Discovery
---------
With either existing OSDs or new ones being activated, a *discovery* process is
performed using :term:`LVM tags` to enable the systemd units.

The systemd unit will capture the :term:`OSD id` and :term:`OSD uuid` and
persist it. Internally, the activation will enable it like::

    systemctl enable ceph-volume@$id-$uuid-lvm

For example::

    systemctl enable ceph-volume@0-8715BEB4-15C5-49DE-BA6F-401086EC7B41-lvm

Would start the discovery process for the OSD with an id of ``0`` and a UUID of
``8715BEB4-15C5-49DE-BA6F-401086EC7B41``.

.. note:: for more details on the systemd workflow see :ref:`ceph-volume-systemd`

The systemd unit will look for the matching OSD device, and by looking at its
:term:`LVM tags` will proceed to:

# mount the device in the corresponding location (by convention this is
  ``/var/lib/ceph/osd/<cluster name>-<osd id>/``)

# ensure that all required devices are ready for that OSD

# start the ``ceph-osd@0`` systemd unit


Summary
-------
To recap the ``activate`` process:

#. require both :term:`OSD id` and :term:`OSD uuid` or a mounted path for
   existing OSDs for migration
#. enable the system unit with matching id and uuid
#. the systemd unit will ensure all devices are ready and mounted (if needed)
#. the matching ``ceph-osd`` systemd unit will get started
