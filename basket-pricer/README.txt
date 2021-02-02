Author: Kevin Connor ARPE
Email: kevinarpe@gmail.com
Mobile: +852-6098-5018

=================================================================================
Exercise 3: Basket Pricer
=================================================================================

=================================================================================
General design notes:
=================================================================================
I built a "reactive" model where an object graph is created during initialisation with callbacks.
As each last/bid/ask price update is received, the system reacts via callbacks.
Classes "react" in this order: market_data_service -> basket_manager -> baskets -> basket_monitor

To understand how all classes are "wired" together, follow from: $PROJECT_ROOT/main.cpp

=================================================================================
Build notes:
=================================================================================
Apologies if my build notes are weak.  I am not an expert on modern C++ build systems!
I used the latest version of JetBrains CLion on a Debian Linux box.
I hope you can find a version of CMake that can build on your side!  :-)
From CLion, I see these 2x build commands:
$ /home/kca/saveme/clion/latest/bin/cmake/linux/bin/cmake --build /home/kca/saveme/basket-pricer/cmake-build-debug --target clean -- -j 2
$ /home/kca/saveme/clion/latest/bin/cmake/linux/bin/cmake --build /home/kca/saveme/basket-pricer/cmake-build-debug --target all -- -j 2

I run "bleeding edge" Debian Linux (sid).
My GCC C++ compiler:
/usr/bin/c++ -> /etc/alternatives/c++ -> /usr/bin/x86_64-linux-gnu-g++-9
$ /usr/bin/c++ --version
c++ (Debian 9.2.1-1) 9.2.1 20190813

CMake is bundled with CLion for Linux:
$ /home/kca/saveme/clion/latest/bin/cmake/linux/bin/cmake --version
cmake version 3.14.5

=================================================================================
Unit & System tests:
=================================================================================
See: Google_tests/basket_config_test.cpp
See: Google_tests/system_test.cpp

=================================================================================
Throughput testing:
=================================================================================
See: throughput/main.cpp, data/test/throughput/
I wrote a small throughput testing harness that runs on my PC at about 0.6 microseconds per bid/ask price update.
I generated 1,000 baskets with 1,000 tickers where each basket has 10% overlap in tickers with next basket.
I tested with 10,000 updates per ticker, which implies a total of 10 million updates.
