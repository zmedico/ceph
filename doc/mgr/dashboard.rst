dashboard plugin
================

Dashboard plugin visualizes the statistics of the cluster using a web server
hosted by ``ceph-mgr``.

Enabling
--------

The *dashboard* module can be enabled with::

  ceph mgr module enable dashboard

Please note that the dashboard will *only* start on the manager which is active
at that moment. Query the Ceph cluster status to see which manager is active.

Configuration
-------------

Like most web applications, dashboard binds to a host name and port.
By default, the module binds to port 7000 and the address ``::``,
which corresponds to all available IPv4 or IPV6 on the host with the
active ceph-mgr daemon.  These defaults can be changed by setting the
following configuration key options::

  ceph config-key put mgr/dashboard/server_addr $IP
  ceph config-key put mgr/dashboard/server_port $PORT

Since any ceph-mgr daemon may be active and hosting the dashboard,
these settings may need to be configured sepearately.  This can be
done by adding the daemon name to the configuration key (normally the
hostname where the daemon is running).  For example, ``mgr.foo``
running on host ``foo`` can be configured to bind to a specific IP and
port with::

  ceph config-key put mgr/dashboard/foo/server_addr $IP
  ceph config-key put mgr/dashboard/foo/server_port $PORT

