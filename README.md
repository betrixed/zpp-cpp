
The purpose of the zpp-cpp framework is to write standard C++ code for PHP functions, and class methods, 
which will be efficient and easier to write and read, than say C source code, wether or not it is hand-written or generated, 
and is organized in a conventional build framework such as PHP skelaton projects generate. For instance, 
all of the examples use xxx.stub.php files to generate C header files for PHP arginfo and module class registration. 

This project only exists on Linux, as this is the results of a one-man effort, 
struggled with over a number of years without any financial, team or corporate support, 
and the shared library format on windows may be sufficiently different to require extra work.

ZPP-CPP also has an integrated C++ base class, which provides the basis of zend_object instances,  
in order to implement custom PHP object classes, shown with many examples. 
Each class example has  template for static class manager instances for objects lifetime management. 
All the examples have PHP script implementations, and are mostly for a web site framework. 
Performance testing suggests that the compiled extensions, 
together result in overall execution time is 66-75% the script implementations, 
with both using the same data caching optimisations.
