(define fx (lambda (x) (lambda (y) (+ x y z))))
(define fy (fx 100))
(define z 3)
(fy 20)

; 所以说为什么python会有4种作用域！LEGB