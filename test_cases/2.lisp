(define fib-rec (lambda (n) (if (< n 2) n (+ (fib-rec (- n 1)) (fib-rec (- n 2))))))
(fib-rec 3)