# Tests
gtest_dep = dependency('gtest', required: true)
threads_dep = dependency('threads')

# Point tests
ringbuffer_src = ['ringbuffer.cpp',]
ringbuffer_test = executable('ringuffer_test',
                             ringbuffer_src,
                             include_directories: include_dir,
                             dependencies: [gtest_dep, threads_dep,])

test('ringbuffer_test', ringbuffer_test)
