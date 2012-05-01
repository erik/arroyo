macro check (expr) (
      print "checking $expr ..."
      if ~expr
      	 (print "=> true")
      else
         (print "=> false"))

check->(1 + 1 = 2)
check->(1 /= 2)

check->(#(1 + 1) /= 2)
check->(~#(1 + 1) = 2)
