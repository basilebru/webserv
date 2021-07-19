# webserv

HTTP server in C++
Team: bbrunet (@grim22) and julnolle (@sequoiageant)

HTTP compliant (cf RFC 7230 to 7235)

GET, POST, PUT, DELETE...
Upload of files to server (text, images, binary...)

CGI implemented (php-cgi and perl handled)

Compatible with Chrome or Firefox
Tested via Telnet and Postman
Stress tested via SIEGE
42 tester 100% OK

Server Configuration inspired from Nginx: 
"Server block(s)": one or more servers listening on a given address/port, with "server-name" defined or not
"Location block(s)": defining how to handle the request, depending on the URL  
Directives implemented: root (=Nginx alias), error-page, index, autoindex, redirect, client_max_body_size, allowed_methods...

Notes on server: 
- all I/O with server is "non blocking"
- "select" function used before each call to recv and send

Notes on parsing:
- one request can be received in several recv/read
- one read/recv can reveive multiple request ("pipelining")
- "chunked" transfer encoding handled

Notes on CGI:
- CGI is called via a fork and an execve
- The body of the request is sent to the CGI via a pipe
- The CGI writes its output onto a file, and the server then reads from this file
- TO DO: try to communicate with CGI only with pipes (so far, causes deadlocks when body is too big)
- CGI environement variables are set by server. One of them ("PATH INFO") is the name of the script that is run

Ressources:

tutorial "how to build a simple HTTP server from scratch:
https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
how the server "matches" a request to the right configuration block (server block and location block)
https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms
insights on how nginx works internally:
https://www.aosabook.org/en/nginx.html
blocking vs non blocking sockets:
https://www.scottklement.com/rpg/socktut/nonblocking.html
RFC 7230 commented:
https://docs.google.com/document/d/1jEM-PiHYoStfB8KAnTmat553gS5TmExkgDiOLFGKDk8/edit#heading=h.rdums0fppbhu
