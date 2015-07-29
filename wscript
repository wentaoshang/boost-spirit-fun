# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def options(opt):
	opt.load('compiler_cxx')
        opt.load('boost', tooldir = ['waf-tools'])

def configure(conf):
        conf.load('compiler_cxx boost')
        conf.check_boost(lib='system')

        # Enable C++14 for all builds
        conf.env.append_value('CXXFLAGS', ['-O2',
                                           '-std=c++14',
                                           '-std=c++1y',
                                           '-g'])

def build(bld):
        bld.program(target = 'num_adder',
                    source = bld.path.ant_glob('num-adder/*.cpp'),
                    includes = 'num-adder/',
                    use = 'BOOST')
