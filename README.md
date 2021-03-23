# Chat-room
In this project I try to write a client and server to build a chatroom system.

# Server
A server can serve multiple clients simultaneously.

Server will send a hello message to the client, and give the client a username ‘anonymous’ after connected.

## Hello
When a client connects to server, the server will send a hello message to the client, and broadcasts the client's coming.
* To the new client
  ```
  [Server] Hello, anonymous! From: <Client IP>/<Client Port>
  ```
* Broadcast
  ```
  [Server] Someone is coming!
  ```
## Offline
When a client disconnect, the server sends offline message to all the other online clients.
```
[Server] <USERNAME> is offline.
```

## Who
The client can ask server to list all clients.
* Recv
  ```
  who
  ```
* Send
  ```
  [Server] <USERNAME> <CLIENT IP>/<CLIENT PORT>
  [Server] <SENDER USERNAME> <CLIENT IP>/<CLIENT PORT> ->me
  ```

## Change username
For client to change his/her username.
The new name should :
1. Not anonymous
1. unique
1. 2~12 english letter

* Recv
  ```
  name <NEW USERNAME>
  ```
* Send
  * If the new name is anonymous :
    ```
    [Server] ERROR: Username cannot be anonymous.
    ```
  * If the new name is not unique :
    ```
    [Server] ERROR: <NEW USERNAME> has been used by others.
    ```
  * If the new name does not consist of 2~12 English letters. :
    ```
    [Server] ERROR: Username can only consists of 2~12 English letters.
    ```

And server will send messages to all the clients after someone change the name.
* To client who change his/her name :
  ```
  [Server] You're now known as <NEW USERNAME>.
  ```
* To other clients :
  ```
  [Server] <OLD USERNAME> is now known as <NEW USERNAME>.
  ```

## Private message
A client can send a private message to a specific client.
* Recv
  ```
  tell <USERNAME> <MESSAGE>
  ```

* Send ( successed )
  * To sender :
    ```
    [Server] SUCCESS: Your message has been sent.
    ```
  * To receiver :
    ```
    [Server] <SENDER USERNAME> tell you <MESSAGE>
    ```


* Send ( failed )
  * If the sender’s name is anonymous :
    ```
    [Server] ERROR: You are anonymous.
    ```
  * If the receiver’s name is anonymous :
    ```
    [Server] ERROR: The client to which you sent is anonymous.
    ```
  * If the receiver doesn’t exist :
    ```
    [Server] ERROR: The receiver doesn't exist.
    ```

## Broadcast
A client can send a broadcast message to all clients.
* Recv
  ```
  yell <MESSAGE>
  ```
* Send
  ```
  [Server] <SENDER USERNAME> yell <MESSAGE>
  ```

## Error command
When a server receives an error command which hasn't been declared above, it should send an error message back to the sending client.
* Send
  ```
  [Server] ERROR: Error command.
  ```

# Client
A client can't  connect to many servers at the same time.Users should give server IP_ADDRESS/DOMAIN_NAME and Port as the arguments of client program.
* Input
  ```
  ./client <SERVER_IP or DOMAIN_NAME> <SERVER_PORT>
  ```
## Exit
To terminate the process
* Input
  ```
  exit
  ```
