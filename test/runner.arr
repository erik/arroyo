-- define some useful testing macros

macro expect(expr value) (
   e <- ~expr

   if e /= ~value (
      print "$expr failed, expected $value, but got $e"
      false)
   else
      true)

macro test(desc expr) (
   print "\ttesting: $desc"

   if not ~expr
      print "FAILED: $expr")


tests <- ["call", "case", "loop", "math", "precedence", "quote", nil]

loop _x <- 0 until tests->_x = nil (
   name <- tests->_x

   print "Running test unit '${name}'"
   require "test/${name}.arr"
   print "Finished $name \n"

   ++_x)