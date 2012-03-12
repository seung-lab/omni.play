#!/bin/bash

svn co svn+ssh://buildbox@gliamac.mit.edu/gaba/svn/netgpu/omni/staging/trunk/omni omni.src.staging
svn co svn+ssh://buildbox@gliamac.mit.edu/gaba/svn/netgpu/omni/trunk/omni omni.src.trunk
svn co svn+ssh://buildbox@gliamac.mit.edu/gaba/svn/netgpu/omni/dev/omni omni.src.dev
svn co svn+ssh://buildbox@gliamac.mit.edu/gaba/svn/netgpu/omni/branches/stable-00/trunk/omni omni.src.stable
