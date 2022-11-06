project('SoapyRingBuffer', ['cpp'],
        version : '1.0',
        # meson_version : '>= 0.62.0',
        default_options : [
          'werror=true',
          'b_ndebug=false',
          'warning_level=3',
          'buildtype=debugoptimized',
          'cpp_std=c++17'])

cpp = meson.get_compiler('cpp')

cpp_args = [
#""  -'Wall',
  '-Wpointer-arith',
  '-Wcast-qual',
  '-Wcast-align',
  '-Wmissing-declarations',
  '-march=native',
]



include_dir = include_directories('include')

subdir('tests')
subdir('src')
