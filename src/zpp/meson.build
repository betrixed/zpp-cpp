project('zppcpp', 'cpp', 
  version: '1.0.0',
  default_options : ['warning_level=2', 'cpp_std=c++23']
)

php_cfg = find_program('php-config', required: true)

php_check = run_command(php_cfg, '--include-dir', check:true)

php_include = php_check.stdout().strip()
message(php_include)

phpMain = php_include / 'main'
phpZend = php_include / 'Zend'
phpTSRM = php_include / 'TSRM'

message(phpMain)

src_files = [ 'zhm.cpp' ]

phpzpplib = shared_library( 'phpzpplib',
  sources: src_files,
  include_directories: [php_include, phpMain, phpZend, phpTSRM],
  version: '1.0.0',
  soversion : '1',
  install : true
  )
