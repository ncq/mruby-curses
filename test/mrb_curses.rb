##  
## Curses Test
##

assert("Curses#hello") do
  t = Curses.new "hello"
  assert_equal("hello", t.hello)
end

assert("Curses#bye") do
  t = Curses.new "hello"
  assert_equal("hello bye", t.bye)
end

assert("Curses.hi") do
  assert_equal("hi!!", Curses.hi)
end
