# FileTransfer-Python
This program allows you to transfer files between computers with local network. This program is written in Python 2.7

## Usage
To transfer a file, you have to run the program on the first computer with the '-s' flag. It will show you the computer IP address. Run the program on the second computer with the '-c', '-ip', and '-f' flags. That's it!

## Flags
- -s                              Stands for Server. Use this flag if you want to receive a flie.
- -c                              Stands for Client. Use this flag if you want to send a flie.
- -ip <*ip address of server*>    If the '-c' flag is set, use the '-ip' flag to point to server.
- -p <*port*>                     Stand for Port. Use this flag to set the port you want to use. (Client and Server ports must be the same)      
- -d <*destination*>              Stand for Destination. If the '-c' flag is set, use this flag to set the path.
- -f <*filename*>                 Stand for File. If the '-c' flag is set, use this flag to choose the file you want to send.

## Examples
The first computer
```terminal
> python main.py -s
```
The second computer
```terminal
> python main.py -c -f "File.txt" -ip 192.168.0.1
```
