require 'rake'
require 'rake/clean'
# require 'rake/gempackagetask'
require 'rake/testtask'
require 'rake/rdoctask'

require 'rbconfig'
include Config

EXT_DIR = 'ext/zetta'
SESSION_SO = "#{EXT_DIR}/zetta.#{CONFIG['DLEXT']}"
SESSION_SRC = "#{EXT_DIR}/zetta.c"

CLEAN.include FileList["#{EXT_DIR}/*"].exclude(/^.*\.(rb|c)$/)

desc "Compile extension"
task :compile => SESSION_SO

file SESSION_SO => SESSION_SRC do
  cd EXT_DIR do
    ruby 'extconf.rb'
    sh 'make'
  end
end

desc "Run tests"
Rake::TestTask.new do |t|
  t.libs = [EXT_DIR]
  t.pattern = 'test/*_test.rb'
  t.verbose = true
  t.warning = true
end

require 'rake/rdoctask'

Rake::RDocTask.new do |t|
  t.rdoc_dir = 'rdoc'
  t.title    = "ZFS Filesystem bindings for Ruby"
  t.options << '--line-numbers' << '--inline-source' << '-A cattr_accessor=object'
  t.options << '--charset' << 'utf-8'
  t.options << '--main' << 'README.rdoc'
  t.rdoc_files.include('ext/zetta/zetta.c')
  t.rdoc_files.include('README.rdoc')
end


# desc "Compiles and tests the build"
# task :default => [ :compile, :spec ]
#
# MAJOR_VERSION = "0.1"
#
# CLEAN.include [
#   "ext/**/*.bundle", "lib/*.bundle", "ext/**/{Makefile,mkmf.log,*.o}",
#   "*.gem", 'pkg', 'doc/coverage', 'doc/rdoc'
# ]
#
# Gem::manage_gems
# SPEC = Gem::Specification.new do |s|
#   # Stuff I might want to tweak.
#   s.summary = "A Ruby interface to manage ZFS."
#
#   # Calculate version from the +MAJOR_VERSION+ constant above, and the current
#   # svn revision, if applicable.
#   revision = `svn info`[/Revision: (\d+)/, 1] rescue nil
#   s.version = MAJOR_VERSION + (revision ? ".#{revision}" : "")
#
#   # Usual constants
#   s.name = File.basename(File.dirname(File.expand_path(__FILE__)))
#   s.author = "Graeme Mathieson"
#   s.email = "mathie@rubaidh.com"
#   s.homepage = "http://rubaidh.lighthouseapp.com/projects/2308/home"
#   s.platform = Gem::Platform::RUBY # FIXME: Maybe this should be Solaris?
#   s.description = s.summary
#   s.files = (%w(CHANGELOG Rakefile README) +
#       FileList["{bin,doc,lib,spec}/**/*"].to_a +
#       FileList["ext/**/*.{h,c}"].to_a).delete_if do |f|
#     f =~ /^\._/ ||
#     f =~ /doc\/(rdoc|coverage)/ ||
#     f =~ /\.(so|bundle)$/
#   end
#   s.require_path = "lib"
#   s.extensions = FileList["ext/**/extconf.rb"].to_a
#   s.bindir = 'bin'
#   s.test_files = FileList["spec/*.rb"].to_a
#
#   # Documentation
#   s.has_rdoc = true
#   s.extra_rdoc_files = ['README', 'CHANGELOG']
# end
#
# Spec::Rake::SpecTask.new do |spec|
#   spec.spec_files = FileList['spec/**/*_spec.rb']
#   spec.rcov = true
#   spec.rcov_dir = "doc/coverage"
# end
#