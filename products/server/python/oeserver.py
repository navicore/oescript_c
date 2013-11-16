#!/usr/bin/python
#  
# The contents of this file are subject to the Apache License
#  Version 2.0 (the "License"); you may not use this file except in
#  compliance with the License. You may obtain a copy of the License 
#  from the file named COPYING and from http://www.apache.org/licenses/.
#  
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#  
#  The Original Code is OeScript.
# 
#  The Initial Developer of the Original Code is OnExtent, LLC.
#  Portions created by OnExtent, LLC are Copyright (C) 2008-2009
#  OnExtent, LLC. All Rights Reserved.
#

import datetime
import oescriptimpl
from oescript import *

print "starting python oescript server: %s" % datetime.datetime.now()

signals = None
signals = OesSigHandler("oeserver.pid", "oespy", "info")
if signals == None: raise AsertionError

#todo: signals.add for each obj...

io_dispatcher = OesDispatcher(0, "io_dispatcher")
if io_dispatcher == None: raise AsertionError

template_matcher = OesDispatcher(1, "template_matcher")
if template_matcher == None: raise AsertionError
template_matcher.start()

db_threader = OesThreadDispatcher(4)
if db_threader == None: raise AsertionError
db_threader.start()

store = OesStore(3, "__replace_me_with_data_dir__", True)
if store == None: raise AsertionError

kernel = OesKernel(template_matcher, db_threader, store, False, None, None)
if kernel == None: raise AsertionError

#start ysp socket server
jnet = OesNet(io_dispatcher)
if jnet == None: raise AsertionError
json = OesServer(kernel, io_dispatcher, "oejson://0.0.0.0:7778", jnet, store)
if json == None: raise AsertionError


io_dispatcher.start() #blocking call.  server is running now

signals.shutdown() #if is isn't already


