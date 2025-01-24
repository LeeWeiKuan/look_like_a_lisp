(define make-adder (lambda (x) (lambda (y) (+ x y))))
((make-adder 5) 2)
(define add5 (make-adder 5))
(add5 100)