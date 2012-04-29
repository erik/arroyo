print "These should all print true"
print (fn(x)x+1)->(1) = 2
print [1, 2, 3]->(0) = 1

add <- fn (x y) x + y
print add->(1, 2) = 3

fn add (x y) x + y
print add->(1, 2) = 3

print (fn (a i) a->i)->([0, 1, 2, 3], 0) = 0