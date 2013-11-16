(load-extension "./oescriptimpl.so" "scm_init_onextent_lib_oescriptimpl_module")
(use-modules (onextent lib oescriptimpl))

(define signals (new-OesSigHandler "mysigfile.txt" "oescm" "info"))
(define dispatcher (new-OesDispatcher 1 "mydispatcher"))
(OesDispatcher-start dispatcher)
(define tdispatcher (new-OesThreadDispatcher 4))
(OesThreadDispatcher-start tdispatcher)
(define store (new-OesStore 3 "data" #t))

(define kernel (new-OesKernel dispatcher tdispatcher store "" '() "" ""))

(define jnet (new-OesNet dispatcher #f "" ""))
(define jsp (new-OesServer dispatcher "oejson://0.0.0.0:7775" jnet store))
(OesServer-addKernel jsp "myspace" kernel)

