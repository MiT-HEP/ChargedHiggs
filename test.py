import os,sys
import re

import json
import requests

from optparse import OptionParser, OptionGroup
from subprocess import call,check_output
import NeroProducer.Nero.GitHubHandler as github

tokenfile=os.environ["HOME"]+"/.ssh/id_status_token"
gh=github.GitHubHandler("MiT-HEP","ChargedHiggs")
gh.read_token_fromfile(tokenfile)
comment="""PR is successfull.
    Test Comment.
    """
gh.submit_comment(198,comment)
