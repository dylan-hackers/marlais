;;;
;;; foreign.dyl
;;;
;;; Brent Benson
;;;

;;
;; Types - int: double: char*:
;;

(define *foreign-table* (make <table>))

(define-class <ff> (<object>)
  (name init-keyword: name:)
  (file-handle init-keyword: file-handle:)
  (arg-types init-keyword: arg-types:)
  (return-type init-keyword: return-type:))

(define-method load-foreign ((filename <string>)) (%load-foreign filename))
(define-method describe-foreign ((fun-name <symbol>) 
				 #key
				 file-handle
				 arg-types
				 return-type)
  
						     