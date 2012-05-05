(
   -- simple helper
   fn stringify(x) "$x"

   test->("quote id",            expect->(stringify->(#some_id),
                                         "some_id"))
   test->("quote number",        expect->(stringify->(#1.234567),
                                          "1.234567"))
   test->("quote string",        expect->(stringify->(#"asd"),
                                          "asd"))
   test->("quote literal string",expect->(stringify->(#"""asd"""),
                                          "asd"))
   test->("quote true",          expect->(stringify->(#true),
                                          "true"))
   test->("quote false",         expect->(stringify->(#false),
                                          "false"))
   test->("quote nil",           expect->(stringify->(#nil),
                                          "nil"))
   test->("quote block",         expect->(stringify->(#(a, b, c)),
                                          "(a b c)"))
   test->("quote array",         expect->(stringify->(#[a, b, c]),
                                          "[a b c]"))
   test->("quote unary",         expect->(stringify->(#-1.234567),
                                          "-1.234567"))
   test->("quote if",            expect->(stringify->(#if true t else f),
                                          "if true t else f "))
   test->("quote for",           expect->(stringify->(#for x in [a b c] x),
                                          "for x in [a b c] x"))
   test->("quote loop",          expect->(stringify->(#loop until true foo),
                                          "loop until true foo"))
   test->("quote case",          expect->(stringify->(#case a of ()),
                                          "case a of ()"))
   test->("quote quote",         expect->(stringify->(##quoted),
                                          "#quoted"))
)