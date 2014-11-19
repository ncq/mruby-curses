# mruby-curses   [![Build Status](https://travis-ci.org/KotaroHibi/mruby-curses.png?branch=master)](https://travis-ci.org/KotaroHibi/mruby-curses)
use ncurses methods from mruby.

## requirement
This gem uses the ncurses library.  
If you have not installed the ncurses, please install the ncurses.

## install by mrbgems 
add conf.gem line to `build_config.rb` 

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :git => 'https://github.com/ncq/mruby-curses.git'
end
```
## example 
```ruby
Curses.initscr
=> call curses initscr
```

## License
under the MIT License:
- see LICENSE file
