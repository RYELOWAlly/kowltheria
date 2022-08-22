;; save_load_context is called by SaveFile when a context is loaded
;; into the game (SaveFile -> Planet). this is ran after all data is
;; initially loaded for modification; it is a good idea to use
;; save-set-modified-content and save-set-modified here if modded for
;; other versions of etheria and etheriadit to signify that the save
;; may have strange data within it

(define (wizard_mode_enabled context)
  (etheria_log "We're in Wizard mode")
  (save_set_modified context #t))

(define (save_load_context context)
  (etheria_log "Context loaded")
  (if (wizard_enabled)
      (wizard_mode_enabled context)))
  
;; save_save_context is called by SaveFile when a context is saved
;; from the game (Planet -> SaveFile). this is ran before data is
;; saved into the SaveFile for modification

(define (save_save_context context)
  (etheria_log "Context saved"))
