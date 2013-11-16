source("oescript.R")
signals = OesSigHandler("oeserver.pid")
io_dispatcher = OesDispatcher(0, "io_dispatcher")
template_matcher = OesDispatcher(1, "template_matcher")
OesDispatcher_start(template_matcher)
db_threader = OesThreadDispatcher(4)
OesThreadDispatcher_start(db_threader)
store = OesStore(3, "/var/oescript/data", TRUE)
kernel = OesKernel(template_matcher, db_threader, store, "", NULL)
jnet = OesNet(io_dispatcher)
json = OesServer(kernel, io_dispatcher, "oejson://0.0.0.0:7788", jnet, store)

OesDispatcher_start(io_dispatcher)

