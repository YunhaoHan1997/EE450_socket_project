# Socket Programming Project  
***
<br>

- [Socket Programming Project](#socket-programming-project)
- [Student Info](#student-info)
  - [Student Name: xxx](#student-name-xxx)
  - [StudentID: xxx](#studentid-xxx)
- [What I did?](#what-i-did)
- [How to run?](#how-to-run)
- [How does it work?](#how-does-it-work)
  - [serverM](#serverm)
  - [serverA/B/C](#serverabc)
  - [client](#client)
  - [monitor](#monitor)
- [Format of the messages](#format-of-the-messages)
- [Idiosyncrasy](#idiosyncrasy)
- [Reused Code](#reused-code)

<br>  

# Student Info
## Student Name: xxx
## StudentID: xxx
<br> 

# What I did?

All mandatory parts mentioned in instructions.  
<br>

# How to run?
Firstly, run makefile, you get exec files.  
<br>

 Move block123.txts into dictinary. Start serverM, then start serverA, serverB, serverC. Then the server side is running, unless you terminate manully, it won't be shut down, even if the input arguments are wrong.  
<br>
Then you can start client or monitor in any sequence.

 Using
> ./client Name  

to check wallet  
using
> ./client Name1 Name2 amount  

to transfer money from name1 to name2  
using
> ./monitor TXLIST   

to output transactions to txlist.txt  
<br>

# How does it work?
## serverM
In serverM, I read from serverA/B/C **only once**, then I use serverM as the core server, and store the **transactions and wallets** in it.  

When starts, it waits for response from serverA/B/C to get transactions,then uses the tranctions to build wallets. Then it begins looping to receive from **either client or monitor**.  

If receives request from client, there must be 2 situations.  

If only check, it begins to check the whether the name is exist, if it is, then send the client the name's balance.

If transfer, it checks if both of the names exist, if it is, then check the balance of sender whether sufficient or not. If sufficient, then randomly chooses serverA/B/C to restore the new transaction then sends the balance to client.  

If receives from monitor, then send transactions that stored in serverM to monitor.  

Then it ends this loop and begin next loop.

<br>

## serverA/B/C  

When serverA/B/C start, it sends the decrypt transactions to serverM **only once**, then start looping to get the response from serverM.

When it gets the new transaction from serverM, it encrypts and then will store it into block.txt. 

Then it ends this loop and begin next loop.  

<br>

## client
Client requests to serverM, then send name1, name2, amount.(name2 and amount maybe null) and waiting for response(different msg and amount) from serverM.  

Then it exit.  
<br>

## monitor
Monitor rquests to serverM, it gets all transactions and stored them into txchain.txt.  
Then it exit.  
<br>

# Format of the messages  
|  From to          |  |
|  ----  | ----  |
| client to serverM  | name1, name2, amount|
||name2 and amount can be NULL
| serverM to client  | flag, amount |
||   0 means check succ
||     1 means check but not exist
||     2 means transfer success
||     3 means name1 not exist
||     4 means name2 not exist
||     5 means neither name1 nor name2 exist
||     6 means name1 insufficient
|serverA/B/C to serverM| name1, name2, amount|
|serverM to serverA/B/C| id, name1, name2, amount|

<br>

# Idiosyncrasy

1. Input check, if there is any issue with *argv in client or monitor, it will exit, but the server will still run.
2. The transactions are stored in serverM, serverA/B/C will only send all transactions once.  
   
<br>

# Reused Code  

I referenced Beej Guide Network Programming to write some functions like sendto, accept.etc and I have them commented in my code.
