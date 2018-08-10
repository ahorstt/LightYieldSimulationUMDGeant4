# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
     . /etc/bashrc
fi

# User specific aliases and functions

# gLite & CRAB
alias source_CRAB='source /cvmfs/cms.cern.ch/crab/crab.sh'

# CMSSW
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch/
. $VO_CMS_SW_DIR/cmsset_default.sh

# Kerberos
alias kinit_fnal='/usr/kerberos/bin/kinit -A -f'
alias kinit_cern='/usr/kerberos/bin/kinit -5 -A'

