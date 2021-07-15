project('clientServerHeartBeatProgram', 'cpp')

boost = dependency('Boost', required : true, modules : ['system', 'thread'])

executable('server', 'server.cpp', dependencies: boost)
executable('client', 'client.cpp', dependencies: boost)