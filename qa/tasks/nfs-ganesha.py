"""
Task for running and configuring nfs-ganesha
"""

import logging

from teuthology import misc
from teuthology.exceptions import ConfigError
from teuthology.task import Task
from teuthology.packaging import get_builder_project

log = logging.getLogger(__name__)


class NFSGanesha(Task):
    """
    Install and mount nfs-ganesha
    
    This will require foo.

    For example:

    tasks:
    - nfs-ganesha:
        foo:
        bar:

    Possible options for this task are:
        foo:
        bar:
        baz:
    """
    def __init__(self, ctx, config):
        super(NFSGanesha, self).__init__(ctx, config)
        self.log = log

    def setup(self):
        super(NFSGanesha, self).setup()
        config = self.config
        log.debug('config is: %r', config)
        #builder = get_builder_project()("nfs-ganesha", role_config,ctx=ctx, remote=role_remote)

    def begin(self):
        super(RBDMirror, self).begin()
        # ls /etc/ganesha

task = NFSGanesha
