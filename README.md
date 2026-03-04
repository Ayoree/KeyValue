Install `boost` and `tbb` go to root folder and run
<br>
```bash
mkdir build && cd ./build && cmake -DCMAKE_BUILD_TYPE:STRING=Release .. && cmake --build .
```
There is 2 CMake targets: `client` and `server`, you can build only one of them if you want.
<br>
To run server use `./server <port> [threads]`
<br>
To run client use `./client <host> <port> [isAuto (0,1)]`
<br>
Nothing more to say.
