CXXFLAGS=-ggdb -Wall -I/usr/include/sqlplus -I/usr/include/mysql -Wno-deprecated


clean:
	rm -f *.o chatchan

chatchan: irc_user.o irc_channel.o eqirc_connection.o eqirc_bridge.o misc.o cc.o irc_prefix.o irc_line.o eq_connection.o irc_connection.o controller.o command.o
	g++ -o chatchan irc_user.o irc_channel.o eqirc_connection.o eqirc_bridge.o misc.o cc.o irc_prefix.o irc_line.o eq_connection.o irc_connection.o controller.o command.o -lsqlplus -L/usr/lib/mysql -lmysqlclient -lz -lcrypt -lnsl -lm -L/usr/lib -lssl -lcrypto

controller: controller.o irc_user.o irc_channel.o irc_connection.o misc.o irc_line.o irc_prefix.o command.o
	g++ -o controller controller.o irc_user.o irc_channel.o irc_connection.o misc.o irc_line.o irc_prefix.o command.o -lsqlplus -L/usr/lib/mysql -lmysqlclient -lz -lcrypt -lnsl -lm -L/usr/lib -lssl -lcrypto

eq_test:	eq_test.o eq_connection.o misc.o
	g++ -o eq_test eq_test.o eq_connection.o misc.o
