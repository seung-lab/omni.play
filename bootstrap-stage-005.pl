#!/usr/bin/perl

$basepath = `pwd`;
chomp $basepath;

`echo "CMAKE_BUILD_TYPE:STRING=Debug" >> $basepath/omni/CMakeCache.txt`;
`echo "CMAKE_INSTALL_PREFIX:STRING=$basepath/build" >> $basepath/omni/CMakeCache.txt`;
`echo "QT_QMAKE_EXECUTABLE:STRING=$basepath/external/libs/Qt/bin/qmake" >> $basepath/omni/CMakeCache.txt`;
`echo "DESIRED_QT_VERSION:STRING=4" >> $basepath/omni/CMakeCache.txt`;

`echo "Boost_INCLUDE_DIR:PATH=$basepath/external/libs/Boost/include/boost-1_38/" >> $basepath/omni/CMakeCache.txt`;
`echo "Boost_LIBRARY_DIRS:FILEPATH=$basepath/external/libs/Boost/" >> $basepath/omni/CMakeCache.txt`;
`echo "Boost_USE_MULTITHREADED:BOOL=ON" >> $basepath/omni/CMakeCache.txt`;

