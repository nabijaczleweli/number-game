#!/usr/bin/env python


def options(opts):
	opts.load('compiler_cxx')

def configure(conf):
	conf.load('compiler_cxx')
	conf.check(features='cxx cxxprogram', cxxflags=['-std=c++14', '-Wall', '-Wextra', '-O3', '-pedantic', '-pipe'], uselib_store='M')
	conf.check(features='cxx cxxprogram', lib='sqlite3', uselib_store='M')
	conf.check(features='cxx cxxprogram', header_name='sqlite3.h', uselib_store='M')

def build(buld):
	buld(features='cxx cxxprogram', source=buld.path.ant_glob('src/**/*.cpp'), target='number-game', use=['M'])
