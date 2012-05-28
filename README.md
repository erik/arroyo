# Arroyo

Simple language built to experiment with writing a custom
parser. Not even close to being complete or even usable yet.

Syntax is a mashup of some languages I enjoy, and is designed to be
relatively simple to parse and work with. That said, it will probably
be revolting to actually program with.

## Language

The main idea Arroyo is built around is that everything is an expression,
there is no concept of statements. The problem with this is that something
like `my_function(args)` cannot be parsed as a function call as it is in
other languages without introducing some ambiguity (i.e. is it a function call,
or is it the expression `my_function` followed by the block `(args)`?).

To get around this, Arroyo uses the horrible notation of `callable->(arguments)`,
which, while dreadful, is unambiguous.

Some other things to note:

* `,` is whitespace. It doesn't change meanings or precedence anywhere.
* **everything** is an expression. You can use any code in an assignment
statement for example.
* The parser hates you and is not afraid to let you know.
* `print`, `eval`, and `require` are implemented as unary operators, and as such,
do not follow the standard `function->(args)` notation. This will hopefully change
later on.
* Variable scope is very naively implemented, be careful not to overwrite values
in functions.

In summary, if you're seriously considering using Arroyo, I advise that you go
outside, take some deep breaths, and think about all the wonderful languages that
you can use instead.

Here are some simple examples of what Arroyo actually looks like:

```lua
-- map array's values to a function
fn map(a:array, f:function) (
    ret <- []
    for x in a (
        ret << f->(x)
    )
)

map->([1,2,3], fn(_)_+1)  -- => [2,3,4]
```

```lua
-- simple, inaccurate definite integral
fn integral(f:function, low:real, high:real) (
  sum <- 0.0
  loop i <- low, while i < high
    sum <- 0.01 * (func->(i <- i + 0.01)) + sum
)

integral->(fn(x) x*x, 1, 10) -- => ~334
```

## License

Copyright (C) 2012 Erik Price

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
