clear
# watch the src and tests directories, but ignore vim swap files (*.sw*)
while true;
do
    filename=$(inotifywait -e modify,move,create,delete --exclude ^.+?\.sw.?$ ../src ../tests)
    clear
    echo $filename $(date) "\n\n"
    make
done
