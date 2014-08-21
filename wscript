VERSION= '0.0.1'
APPNAME= 'ngweight'

srcdir= '.'
blddir= 'bin'

def set_options(ctx):
  ctx.tool_options('compiler_cxx')
    
def configure(ctx):
  ctx.check_tool('compiler_cxx')
  ctx.env.CXXFLAGS += ['-O2', '-Wall', '-g', '-std=c++0x']

def build(bld):
  task1= bld(features='cxx cprogram',
       source       = 'ngweight.cpp wat_array.cpp bit_array.cpp',
       name         = 'ngweight',
       target       = 'ngweight',
       includes     = '.')
