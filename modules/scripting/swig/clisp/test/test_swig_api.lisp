(load-extension "./oescriptimpl.so" "scm_init_onextent_lib_oescriptimpl_module")
(use-modules (onextent lib oescriptimpl))

(define signals (new-OesSigHandler "mysigfile.txt"))
(define dispatcher (new-OesDispatcher 1 "mydispatcher"))
(OesDispatcher-start dispatcher)
(define tdispatcher (new-OesThreadDispatcher 4))
(OesThreadDispatcher-start tdispatcher)
(define store (new-OesStore 3 "data" 1))

(define kernel (new-OesKernel dispatcher tdispatcher store "" '()))

(define net (new-OesNet dispatcher))
(define oesp (new-OesServer kernel dispatcher "oesp://0.0.0.0:7777" net store))
(define jnet (new-OesNet dispatcher))
(define jsp (new-OesServer kernel dispatcher "oejson://0.0.0.0:7775" jnet store))

