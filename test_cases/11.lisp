(define x '(a 1))
(define y '(a 2))
(if (eq (car x) (car y)) (eval (car (cdr x))) (eval (car (cdr y))))