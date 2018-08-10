# .cshrc

# Source global definitions
if ( -f /etc/csh.cshrc ) then
        source /etc/csh.cshrc
endif

# User specific aliases and functions

# gLite & CRAB
alias source_CRAB 'source /cvmfs/cms.cern.ch/crab/crab.csh'

# CMSSW
setenv VO_CMS_SW_DIR /cvmfs/cms.cern.ch/
source $VO_CMS_SW_DIR/cmsset_default.csh

# Kerberos
alias kinit_fnal '/usr/kerberos/bin/kinit -A -f'
alias kinit_cern '/usr/kerberos/bin/kinit -5 -A'
