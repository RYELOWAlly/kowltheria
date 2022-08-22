(load "lib/saves.scm")

;; world_open is called at the end of World::Generate, so basically
;; when a new world is generated and any custom info is to be placed
;; within the level.

(define (world_open world)
  (etheria-log "World opened"))
