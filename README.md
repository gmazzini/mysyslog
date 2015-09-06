# mysyslog
syslog C demon with file logging and query issue

mysyslog receives syslog file on 514 UDP port, process it, record on a file the activity and takes the last activity of a given IP for fast query

configuration file is space or \n separated fields and is supposed to be in /mysyslog

only one configuration file is necessary: mysyslog.conf with configuration action

mysyslog.conf contains in the first line: the syslog UDP receiving port, the syslog priority to be processed, the number of client allowed to send syslog; then the following lines are the ipv4 address of the allowed clients

each log file is automatic generated with path /mysyslog/log and filename in the format YYYYMMGG.log which is defined at the starting time. In order to have a different file for each day it is suggested to restart the demon every night at 00:00. In any case if any restart occurs with the same filename, and then in the same day, log file is in append mode. The log file is csv with the following fields: YYYY-MM-GGTHH:MM:SS, protocols, source ipv4, source port, destination ipv4, destination ipv4. An entry esample is 2015-04-05T23:24:57,TCP,10.42.245.36,44023,80.252.91.41,80

