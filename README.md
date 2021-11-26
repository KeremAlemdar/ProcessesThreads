# ProcessesThreads
Application has two parts: client and server.
Client get a request from a user about what statistics to compute and sent to the server over POSIX message queue. Server compute the requested statistics using multiple child processes that will act on data concurrently. Child processes pass data to the parent using pipes. Project also include thread version of the application where server use threads instead of child processes.
