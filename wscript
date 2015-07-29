# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def options(opt):
	opt.load('compiler_cxx')
        opt.load('boost', tooldir = ['waf-tools'])

def configure(conf):
        conf.load('compiler_cxx boost')
        conf.check_boost(lib='system')

def build(bld):
        bld.program(target = 'num-adder',
                    source = bld.path.ant_glob('num_adder/*.cpp'),
                    includes = 'num_adder/',
                    use = 'BOOST',
                    cxxflags = ['-O2',
                                '-std=c++14',
                                '-std=c++1y',
                                '-g'])
