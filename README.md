# mysyslog
written by Gianluca Mazzini gianluca@mazzini.org
started in 2015

syslog C demon with file logging and query issue

mysyslog receives syslog file on 514 UDP port, process it, record on a file the activity and takes the last activity of a given IP for fast query

configuration file is space or \n separated fields and is supposed to be in /mysyslog

only one configuration file is necessary: mysyslog.conf with configuration action

mysyslog.conf contains in the first line: the syslog UDP receiving port, the syslog priority to be processed, the number of client allowed to send syslog; then the following lines are the ipv4 address of the allowed clients

each log file is automatic generated with path /mysyslog/log and filename in the format YYYYMMGG.log which is defined at the starting time. In order to have a different file for each day it is suggested to restart the demon every night at 00:00. In any case if any restart occurs with the same filename, and then in the same day, log file is in append mode. The log file is csv with the following fields: YYYY-MM-GGTHH:MM:SS, protocols, source ipv4, source port, destination ipv4, destination ipv4. An entry esample is 2015-04-05T23:24:57,TCP,10.42.245.36,44023,80.252.91.41,80

mysyslog caches the last time activity in which a given source ipv4 performs traffic in the range 10.32.0.0/12. A query may be sent to mysyslog to have such an information by means of a syslog fake query with special source ipv4 10.32.0.0 and destination ipv4 the ipv4 in which you are interested. Just a simple bash query whith interested ipv4 10.42.246.50 is exec 3<>/dev/udp/127.0.0.1/514; echo -n "<134>1968-03-01T00:00:00+0100 CCR1 test srcnat: in:query out:query, src-mac 00:00:00:00:00:00, proto UDP, 10.32.0.0:0->10.42.246.50:0, len 00" >&3; head -n 1 <&3; exec 3<&-; exec 3>&-

mysyslog manages the special source ipv4 with address 10.47.255.255 wguch allows to have query that just refresh mysyslog cache without create a new log file entry. Such a condition may be used for periodically check the demon status and eventually restart if something fails
