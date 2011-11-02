port=11675
logfile=../log/1000.log

# Change to area directory.
cd ../area
if [ -r shutdown.txt ]
then
         rm -f shutdown.txt
fi
#launch and keep launches
while [ 1 ]
do
    ../bin/BlinkenMUD $port >$logfile 
    # Restart, giving old connections a chance to die.
    if [ -r shutdown.txt ]
    then
        rm -f shutdown.txt
        exit 0
    fi
    sleep 10
done
