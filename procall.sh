while read LINE; do
  RES=`./lshape "$LINE"`
  if [ $? == 0 ]; then
#    echo "  $LINE"
    true
  else
    echo "N $LINE"
  fi
done
