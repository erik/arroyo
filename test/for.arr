test->("for loop on arrays returns last value in array",
       expect->(for x in [1 2 3] (x), 3))

test->("for loop works on empty array",
       expect->(for x in [] (), nil))

test->("for doesn't pollute scope",
       expect->((for x in [1 2 3] (x), x = nil),
                true))
