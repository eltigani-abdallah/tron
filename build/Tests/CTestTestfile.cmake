# CMake generated Testfile for 
# Source directory: C:/Users/Abricot/Desktop/Tron2/Tests
# Build directory: C:/Users/Abricot/Desktop/Tron2/build/Tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[TronUnitTests]=] "C:/Users/Abricot/Desktop/Tron2/build/bin/Debug/TronTests.exe")
  set_tests_properties([=[TronUnitTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Abricot/Desktop/Tron2/Tests/CMakeLists.txt;34;add_test;C:/Users/Abricot/Desktop/Tron2/Tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[TronUnitTests]=] "C:/Users/Abricot/Desktop/Tron2/build/bin/Release/TronTests.exe")
  set_tests_properties([=[TronUnitTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Abricot/Desktop/Tron2/Tests/CMakeLists.txt;34;add_test;C:/Users/Abricot/Desktop/Tron2/Tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[TronUnitTests]=] "C:/Users/Abricot/Desktop/Tron2/build/bin/MinSizeRel/TronTests.exe")
  set_tests_properties([=[TronUnitTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Abricot/Desktop/Tron2/Tests/CMakeLists.txt;34;add_test;C:/Users/Abricot/Desktop/Tron2/Tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[TronUnitTests]=] "C:/Users/Abricot/Desktop/Tron2/build/bin/RelWithDebInfo/TronTests.exe")
  set_tests_properties([=[TronUnitTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Abricot/Desktop/Tron2/Tests/CMakeLists.txt;34;add_test;C:/Users/Abricot/Desktop/Tron2/Tests/CMakeLists.txt;0;")
else()
  add_test([=[TronUnitTests]=] NOT_AVAILABLE)
endif()
