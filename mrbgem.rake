MRuby::Gem::Specification.new('mruby-curses') do |spec|
  spec.license = 'MIT'
  spec.authors = 'Kotaro Hibi'
  spec.linker.libraries << 'ncurses'
end
