
# Socket Programming Projects

A collection of socket-based applications showcasing the fundamentals of network communication. This repository contains the following two projects:

1. **Webserver**: A lightweight web server built in C to serve static web pages.
2. **Chat Application**: A real-time chat application implemented in C using sockets for client-server communication.

> [!NOTE]
> Both the application has multithreading implemented for multi user access and handle multiple connections.

---

## Table of Contents

- [Webserver](#webserver)
  - [Features](#features)
  - [Usage](#usage)
- [Chat Application](#chat-application)
  - [Features](#features-1)
  - [Usage](#usage-1)
- [Project Structure](#project-structure)
- [Setup Instructions](#setup-instructions)
- [Contributing](#contributing)
- [License](#license)

---

## Webserver

### Features
- Serves static HTML files to clients over HTTP.
- Handles basic HTTP `GET` and `POST` requests.
- Multithreaded to handle multiple client requests simultaneously.
- Simple to configure and run.

### Usage
1. Compile the webserver:
   ```bash
   gcc webserver.c -o webserver
   ```
2. Run the server:
   ```bash
   ./webserver
   ```
3. Access the server via a browser or a tool like `postman`:
   ```
   http://localhost:<port>
   ```
> [!TIP]
> Use `-lpthread` if you get hit with a warning that `pthread is undefined`
---

## Chat Application

### Features
- Real-time, text-based messaging between multiple clients.
- Server handles concurrent clients using threads.
- Lightweight and efficient implementation using low level C sockets.

### Demo
<video src="./chatapp/screen_rec.mp4" controls="controls" style="max-width: 100%; height: auto; border-radius: 10px;">
Video tag not supported
</video>

### Usage
1. Compile both server and client:
   ```bash
   gcc server.c -o server
   ```
   ```bash
   gcc client.c -o client
   ```

2. Start up the server by just:
   ```bash
   ./server
   ```
3. Start the client with whatever `PORT` that isn't busy:
    ```bash
    ./client <port-numeber>
    ```
4. Create multiple clients in different windows to access multiple chat clients.

---

## Project Structure

```plaintext
.
├── webserver/
│   ├── webserver.c        
│   ├── index.html        
│   └── webserver           
├── chatapp/
│   ├── server.c
│   ├── server    
│   ├── client.c    
│   └── client           
└── README.md               
```

---

## Setup Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/socket-programming-projects.git
   cd socket-programming-projects
   ```

2. Follow the instructions in the [Webserver](#webserver) and [Chat Application](#chat-application) sections to run the projects.

---

## Contributing

Contributions are welcome! To contribute:
1. Fork this repository.
2. Create a feature branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. Commit your changes:
   ```bash
   git commit -m "Add your feature description"
   ```
4. Push to your branch:
   ```bash
   git push origin feature/your-feature-name
   ```
5. Open a pull request.

---


## Author

Developed by [Bhavya Gupta *(aka Gamin8ing)*](https://github.com/gamin8ing).
```
