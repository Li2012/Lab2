Graded by: Justin Wilson and Chris Gill
Score: 92/100
Comments:

Nice lab!

The code is nicely structured and well commented.

The code compiled with no errors or warnings and ran correctly in our tests, except as noted below.

Test 1: Parsing the configuration file
======================================

Execute a single server and single client with errors in the player definition file.
The client should connect but no game will be played.
The three errors to be tested are:
a.  game w/o -g
b.  -g without game
c.  blank line

Team1
-----
a.  Ignored line with message.  Connected.
b.  Ignored line with message.  Connected.
c.  Ignored line.  Connected.

Test 2: Local game play
=======================

On the same host, start one server using the default port, wait 2 seconds, start a second server using a specified port.
"Simultaneously" start a number of clients, wait 30 seconds, then kill all processes.
Two scenarios will be attempted, one with 10 players, and one with 11 players.
The 10 player scenario should demonstrate a working system with play starting not before 5 players have joined a game.
The 11 player scenario should show at least one of the players not participating.

10 Players
==========

Team1
-----
Dealer1:
Join order: 1, 8, 6, 3, 2, 5, 4, 7, 9, 10
Dealing began with 5 players.
New players dealt hands as they joined.
Round 1 involved 9 players. (10 did not participate)
Round 2 involved 10 players.
Round 3 involved 10 players.
Round 4 involved 10 players.
Round 5 involved 10 players.
Round 6 involved 10 players.
Round 7 involved 10 players.
Appears games are started and scored on a timer.

Dealer2:
Join order: 1, 8, 6, 3, 2, 5, 4, 7, 9, 10
Round 1 involved 9 players. (10 did not participate)
Round 2 involved 10 players.
Round 3 involved 10 players.
Round 4 involved 10 players.
Round 5 involved 10 players.
Round 6 involved 10 players.
Round 7 involved 10 players.

Players:
Receive messages about other players.
Psuedo-random number generation appears to be working.

11 Players
==========

Team1
-----
Dealer1:
Join order: 7, 2, 1, 4, 5, 8, 9, 3, 6, 11, 10
Round 1 involved 9 players. (10, 11 did not participate)
Round 2 involved 10 players. (10 did not participate)
Round 3 involved 10 players. (10 did not participate)
Round 4 involved 10 players. (10 did not participate)
Round 5 involved 10 players. (10 did not participate)
Round 6 involved 10 players. (10 did not participate)
Round 7 involved 10 players. (10 did not participate)

Dealer2:
/Join order: 7, 1, 5, 4, 8, 9, 2, 3, 6, 11, 10
Round 1 involved 9 players. (11, 10 did not participate)
Round 2 involved 10 players. (10 did not participate)
Round 3 involved 10 players. (10 did not participate)
Round 4 involved 10 players. (10 did not participate)
Round 5 involved 10 players. (10 did not participate)
Round 6 involved 10 players. (10 did not participate)
Round 7 involved 10 players. (10 did not participate)

Players:
## -3 Player10 joins games but cannot participate.

Test 3: Remote game play
========================

Similar to the previous test except that the 2 servers will be started on host A, 4 clients will be started on host B, and 6/7 clients will be started on host C.
Runs are graded based on their behavior compared to the local case, i.e., the game should proceed as in Test 2.

10 Players
==========

Team1
-----
Dealer1:
Shows connections from 10 clients.
Join order: ??
## -5 Game1 does not proceed.

Dealer2:
Join order: 2, 4, 1, 3, 6, 7, 5, 8, 10, 9
Round 1 involved 9 players. (9 did not participate)
Round 2 involved 10 players.
Round 3 involved 10 players.
Round 4 involved 10 players.
Round 5 involved 10 players.
Round 6 involved 10 players.
Round 7 involved 10 players.

Players:

11 Players
==========

Team1
-----
Dealer1:
Shows connections from 11 clients.
Join order: ??
## no further deduction but Game1 does not proceed.

Dealer2:
Join order: 2, 3, 1, 4, 8, 5, 6, 10, 7, 9, 11
Round 1 involved 9 players. (11, 9 did not participate)
Round 2 involved 10 players. (11 did not participate)
Round 3 involved 10 players. (11 did not participate)
Round 4 involved 10 players. (11 did not participate)
Round 5 involved 10 players. (11 did not participate)
Round 6 involved 10 players. (11 did not participate)
Round 7 involved 10 players. (11 did not participate)

Players:

Test 4: Port conflicts
======================

Two servers are started with the default port.
One should report the error and stop.

Team1
-----
Okay.

---------------------------------------------------------------------------------

CSE 532 Fall 2011 Lab 1

Team Members:
	Alex Benjamin - abenjamin@wustl.edu
	Yinfan Li - linfeixb27@gmail.com
	Mahesh Mahadevan - mahesh.mahadevan@gmail.com
	
	This lab contains the requested material as well as the extra credit portion
	of discussing the different necessary steps and what classes handle them. Certain
	sections of code were re-used (and modified) from publicly available code in 
	accordance with the	class policy.

---------------------------------------------------------------------------------

Lab Design and Overview:
	The purpose of this lab is to implement a client and server for playing various
	types of card games. In this lab, the card game is five card stud. The client
	and server both reactively handle messages from each other in order to conduct
	the game and various handshakes. 
	
---------------------------------------------------------------------------------

	Client:
	
	The Client plays the role of allowing a player to join one or more games at
	any point. When a player is created, the Client code will parse the command
	line information to interpret the player's name, and a definition file containing
	a list of input lines specifying games they want to join and their location via
	host and port. It reactively communicates with the server after sending an initial
	handshake upon creation. The processRead method demultiplexes all messages sent from
	the server to client by reading in a message identifier code and passing off the
	rest of the data stream to a handler method. The handler method then either prints
	out information, and/or does something and replies. All messages from the client
	are sent and received as CORBA Common Data Representation objects - this uses the
	ACE_InputCDR and ACE_OutputCDR wrappers. This was extremely convenient as you can
	read in and out of these data objects like any other stream.
	
	The command line arguments must include exactly one name flag/value and exactly
	one definition file flag/value. (ie, "Client -n Alex -f DefFile).
	
	The definition file must contain multiple lines, each with at least one
	-g flag/value. Optional parameters include the target server port (default 2000)
	and server address/hostname (default localhost).

	Design Notes:
		The Client will interpret all player names and game names to be upper case
		for the first character and lowercase for the rest. The Client will
		not throw an error if duplicate game names are specified, it will simply
		ignore later ones. 
		
		For example, all of the following definition lines are interpreted equivalently:
			1) -g Hearts
			2) -g hearts
			3) -g heaRTS

		If multiple port or address flag/value combinations are specified in
		a definition file line, the last one will be used. For example:
			1) -g Hearts -p 1050 -i localhost -p 2050
				Result -> Join game "Hearts" at port "2050" and address "localhost"
			2) -g Hearts -p 1050 -i localhost -i 0.0.0.0
				Result -> Join game "Hearts" at port "1050" and address "0.0.0.0"

	Details:
		Each time a client is initialized, the following protocol is followed:
			1)  The command line parameters are parsed and an instance of ConnectInfo
				is created to hold the information of each valid definition file line.
				Invalid definition file lines will be skipped, but will not throw an
				error. Command line parameters must include -n (name) flag and value
				and -f (definition file) flag and value.
					Syntax: Client -n Alex -f DefinitionFile
					
				Definition file must include at least one -g (game) flag/value and
				optional parameters for port (default 2000) and address (default localhost)
				
			2)	For each ConnectInfo instance, a connector requests a connection to
				the server described, and registers the connector with a reactor
				singleton.
				
			3)	It will then create a service handler for each ConnectInfo instance
				and initiate a connection on the socket address for the specified
				server. It will allow the request and further events/messages to be
				handled reactively.
				
			4)	The service handler will then send the players name to the server over
				the connection, read back the dealers name, and then send a list of
				games that the player wants to join.
				
			5)	The service handler will reactively read from the socket and respond
				to messages it receives appropriately. Most importantly, it will be sent
				5 cards, score them, and send back the score/rank.

	Syntax:
		Client -n <player name> -f <definition file>
		
		Definition file format for each line (can have multiple lines):
		-g <game name> [-p <server port>][-i <server IP address or hostname>]

		Square brackets [] denote optional parameters.

	Client Files:
		Source:
			1) Client.cpp
				This contains the main method for the client. It reads in parameters
				from the command line, does error checking and creates the appropriate
				containers needed to store game information. It then registers an event
				handler for each game the player wants to connect to, and registers
				the player with the client reactor.
				
			2) ConnectInfo.cpp
				This is a data structure that holds information about a connection to
				a game. A ConnectInfo instance is created for each (valid) line in the 
				definition file containing the game(s), port, and address that the player
				is connecting on.
				
			3) Player_Svc_Handler.cpp
				This is an implementation of an ACE_Svc_Handler which is used to reactively
				handle events from/send events to the server. The demultiplexing point for
				all events from the server is the processRead() method. This reads in a data
				stream block and checks the message identifier code. Depending on the code,
				it passes the block to the appropriate method which will read in the rest of
				the information and then print to console/send data back as needed. Information
				that is sent out to the server is passed on to a data wrapper which wraps
				the appropriate socket stream.
				
			4) PlayerConnection.cpp
				This is an implementation of an ACE_Connector which is used to manage the
				connection to the ACE Reactor. It opens a connection and manages the number
				of server connections open at any time. When a new connection is made, the
				number is incremented, and when a player disconnects from a game, the number
				is decremented. It is passed as a member variable to the Player_Svc_Handler.
				
			5) Poker.cpp
				This class handles standard poker scoring functions. Mainly, it scores 
				various	five card poker hands (Straight Flush, Four of a Kind, Full House, 
				Flush, Straight, Three of a Kind, Two Pair, Pair, High Card). It also has 
				functionality for tiebreaking between two hands of the same rank.
				
			6) Hand.cpp
				This class is a data structure implementing a regular poker hand. It 
				defines	methods for adding/removing from the hand, as well as operators 
				for hand comparison, copy construction, and printing.
				
			7) Card.cpp
				This class is a data structure for a standard playing card with rank and suit.
				
			8) Codes.cpp
				
			9) DataWrapper.cpp
				This class is a wrapper for an ACE_Sock_Stream which handles reading 
				and writing from a socket connection from client <-> server. It handles 
				writing message blocks out over the socket and determining the appropriate 
				bit order. Importantly, it also	handles partial reading over the socket. 
				When it conducts a read, it will check whether the number of bytes read 
				matches the total message length (which is contained in the	header) to 
				see if they match. If not, it will set flags and continue to read the rest
				of the message when it comes through.
			
		Header:
			1) ConnectInfo.h
			2) Player_Svc_Handler.h
			3) PlayerConnection.h
			4) Poker.h
			5) Hand.h
			6) Card.h
			7) Codes.h
			8) Const.h
			9) DataWrapper.h
			10)Lib.h
			11)Stream.h
			
---------------------------------------------------------------------------------

	Server:
	
	The Server plays the role of the dealer - it maintains a list of all the
	different games that have begun and all players that are currently added
	to these games. When a player (Client) wants to connect to a game, the
	server will receive a message block of information over the socket
	that contains certain required and optional parameters. Importantly,
	the server will receive the player's name and a list of games that the
	player wants to join. The server will then attempt to add the player to
	his/her desired games and then will respond with information about what 
	other players are currently playing. The server conducts various handshakes
	and communicates with the client by passing messages that all begin with
	a message identifier that lets the client know what to expect in the body
	of the message. Additionally, the server runs the game flow as far as checking
	who is playing, dealing cards, initiating scoring/ending of games, and starting
	new games. All messages from the client	are sent and received as CORBA Common 
	Data Representation objects - this uses the	ACE_InputCDR and ACE_OutputCDR wrappers. 
	This was extremely convenient as you can read in and out of these data objects 
	like any other stream.

		Syntax:
			Server -n <dealer name> [-p <server port>]

			The name is a required parameter for the Dealer, and if it is omitted
			an error will be returned. The port is an optional flag - if there
			is no port specified, the default port (2000) will be used. Ports below
			1024 will be rejected with an error, as they are reserved for standard
			protocols.
			
		Details:
			Each time a server is initialized, the following protocol is followed:
				1)  The command line parameters are parsed, and the server obtains the
					host name and port on which it will open a socket. It then opens a
					socket, creates a Dealer (which in turn creates in acceptor) and 
					registers with the reactor singleton. 
					
				2)  When a client connects, the server will accept the connection
					request and create an event handler for that client. The handler
					will be registered with the reactor with a DataWrapper for reading
					and writing to the socket stream. The reactors event loop will then
					be run and messages/events will be handled reactively as they are
					received.
					
				3)  When a client handler is created, the server will send it's own
					name to the client and read the player's name. Then, it will obtain
					a list of games from the client describing what games the player
					wants to join. 
					
				4)	A Player instance is created for the client, and if the game does
					not exist already, a Game instance as well. The game will then wait
					until the minimum number of player has joined (MIN_PLAYER) and then
					deal out cards up to the maximum number of players (restricted by the
					number of available cards in the deck). The Players communicate with
					their respective clients to obtain scores, and then the Game determines
					who will win.
					
				5)	All messages between the Server and Client are handled reactively and
					demultiplexed to the appropriate handler method.
			
		Server Files:
			Source:
				1) Server.cpp
					This file contains the main for creation of a new Server instance.
					It will first read in the parameters from the command line (a -n 
					(name) flag/value is required, and optional -p (port) flag/value).
					Then, it will obtain the host address, and open up a socket connection
					at the specified port (default 2000). Finally, it create an instance
					of the Dealer class, register with a reactor singleton, and run the
					reactor's event loop.
					
				2) ClientHandler.cpp
					This is an implementation of an ACE_Event_Handler for reading and
					writing from the server socket. It has a handle_input method which
					reads in a message from the client and passes it onto a
					handleMessageReceived method (a virtual method implemented by it's
					derived Player class) which demultiplexes the data to the appropriate
					handler method. ClientHandler also takes care of checking the bit ordering
					and uses a member DataWrapper to read/write on the socket stream.
					
				3) Dealer.cpp
					The dealer acts as the manager for the list of games and players
					currently connected to this server. When a client connects or disconnects
					from the server, the dealer will add/remove the player from its
					list of games as appropriate.
					
				4) Deck.cpp
					This is an implementation of a standard 52 card deck. Upon creation,
					it will fill a deck container (vector) with one card of each rank and suit,
					and then shuffle the deck. It maintains an iterator pointing to the next card
					in the deck which is advanced whenever a card is dealt.
					
				5) Game.cpp
					The game class handles all of the flow for players joining, playing, and
					leaving an individual game. When a player joins a game, the Game class
					will update the appropriate containers. Whenever enough players have
					joined the game (MIN_PLAYER), the Game will deal out five card hands to
					each of the players (up to MAX_PLAYER=deck size/hand size). Any other
					players beyond the max will be spectators to the game, but will not receive
					any cards. When a player is dealt cards, they will send it to their client
					to score the hand, and then report the score back to the Game. Once all
					the scores have been reported, the Game will determine who the winner is
					and reset for the next round. Subsequent rounds will start after a specified
					waiting period as long as there are enough players. If a player leaves in
					the middle of a game and already has cards, the game will have to shut down.
					
				6) Player.cpp
					This class is an instance of one player who has joined a game. Upon creation,
					the player will register with the dealer and send back the dealer's name
					to the client. The Player class also defines the handleMessageReceived
					method which is the point from which all incoming messages from the client
					are handled. This method will read in the message identifier and determine
					where to send the rest of the information. The class also contains methods
					for sending out messages to the client; however, the actual send is handled
					by it's base class via a DataWrapper object.
					
				7) ServerHandler.cpp
					The ServerHandler class is an implementation of an ACE_Event_Handler, and
					registers with the server's reactor singleton. It's other main purpose is
					to instantiate the client event handler, and use an ACE_Acceptor member 
					variable to accept a connection via an ACE_SOCK_Stream 
					
				8) Codes.cpp
				
				9) DataWrapper.cpp
					This class is a wrapper for an ACE_Sock_Stream which handles reading 
					and writing from a socket connection from client <-> server. It handles 
					writing message blocks out over the socket and determining the appropriate 
					bit order. Importantly, it also	handles partial reading over the socket. 
					When it conducts a read, it will check whether the number of bytes read 
					matches the total message length (which is contained in the	header) to 
					see if they match. If not, it will set flags and continue to read the rest
					of the message when it comes through.
					
				10)Card.cpp
					This class is a data structure for a standard playing card with rank and suit.
				
			Header:
				1) ClientHandler.h
				2) Dealer.h
				3) Deck.h
				4) Game.h
				5) Player.h
				6) ServerHandler.h
				7) Card.h
				8) Codes.h
				9) Const.h
				10)DataWrapper.h
				11)Lib.h
				12)Stream.h
				
---------------------------------------------------------------------------------

Wrapper Facades:

	For this lab, we used the following ACE wrappers:
			ACE Socket Stream
			ACE Socket Acceptor
			ACE Acceptor
			ACE INET Address
			ACE Socket Connector
			ACE Connector
			ACE Event Handler
			ACE Reactor
			ACE Svc Handler		
			ACE Input CDR
			ACE Output CDR

	This was extremely helpful as far as making the networking work easily. With
	these wrappers, we just needed to make sure that the data was being sent in a
	manner that the parsing matched up on both sides, and we didn't really need to
	worry about how to get it across the socket.

	These wrappers essentially just let you send information and encapsulate the
	details of how it is actually getting across the socket from the programmer,
	which makes it both convenient and robust. Needless to say, it would have been
	much more complicated if we had to write all the socket negotiation, connection,
	buffering, and sending ourselves.
	
	Additionally, the data objects made it convenient to construct messages for
	sending back and forth. The data in these objects can be manipulated like regular
	streams, so it was easy to pack/unpack variables.

---------------------------------------------------------------------------------

HOW TO UNPACK AND RUN THE PROGRAM:
	1) Unzip the "CSE_532_Lab_1.zip" file, and enter the "Lab1" directory

	2) Run the "make" command to compile both Client and Server
	
	3) Run command line for Client and Server programs

	Example Steps:
		1) Unzip "CSE_532_Lab_0.zip"
		2) cd Lab0
		
		(in first terminal)
		3) ./Server -n DealerTest
			
		(in second terminal)
		4) ./Client -n John -f P1Definition
			
