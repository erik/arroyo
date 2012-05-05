-- define some useful testing macros

failed <- 0
passed <- 0

macro expect(expr value) (
   e <- ~expr

   if e /= ~value (
      print "$expr failed, expected $value, but got $e"
      false)
   else
      true)

macro test(desc expr) (
   print "\ttesting: $desc"

   if not ~expr (
      ++failed
      print "Test FAILED: $expr")
   else
      ++passed)


tests <- ["call", "case", "loop", "for", "math", "precedence", "quote"]

for t in tests (
   print "Running test unit '${t}'"
   require "test/${t}.arr"
   print "Finished $t \n"
)

print "Testing finished.\n\t$failed tests failed\n\t$passed tests passed"