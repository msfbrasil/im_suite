# im_suite
Test IM (instant messaging) application build on C++

# Some details

This is the result of a little practical test required for a job application which involves the creation of a server and client applications which should behave nearly like IM (Instant Messaging) cliens with every user connecting with a nickname and switching messages between each others.

There was some requirements regarding the solution like the construction of a logger class with messages being persisted on file by a parallel thread, a publish/subscriber implementation for messaging dispatch on server side, a class to handle a connection, a class to handle all the connections from the server (which was called here session_manager), a group of operations that should be supported by the client, etc.

# Warning

This is not even close of a final solution, and represents the final result presented after the allowed time to develop it.

Many improvements need to be done, like:

1- Allow the list command to return all the connected users if one single message is not enought to do it (message has limited size). Alternative would be get rid of message size limitation or establish an indication that the list is not complete so the client could perform a new call informing the last nickname received, for example.

2- Organize some constants definitions. There is one that is being used on "im_client_user_io_handler" that shouldn't be there. At least, not the way it is.

3- Some "TODO"s on the code.

All this just to have something that can be actually called of concluded.

# Building

On command prompt, just run "make clean" to clear the object files and binaries, and "make all" to build "im_server" and "im_client" applications.

# Running

To run the server "all you need is love"...

Kidding...

To run the server all you need is to inform the PORT it will listen to, like this: "./im_server 7777".

To run the client just provide the IP and PORT of the server, like this: "./im_client 127.0.0.1 7777".

When the client starts, a summary of allowed commands is presented, includind the "help" command the shows the summary again.

Have fun!!!
