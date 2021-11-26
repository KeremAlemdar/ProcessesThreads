# ProcessesThreads
Application has two parts: client and server.
Client get a request from a user about what statistics to compute and sent to the server over POSIX message queue. Server compute the requested statistics using multiple child processes that will act on data concurrently. Child processes pass data to the parent using pipes. Project also include thread version of the application where server use threads instead of child processes.

How to invoke server:
./statserver <N> '<filename1>' <filename2> ... <filename3>
How to invoke client:
./statclient
  
The followings are the requests that a user can type at the client prompt.
count
avg
max
count<start><end>
avg<start><end>
range<start><end><K>
  
Report include running time comparison of child process usage and thread usage.
