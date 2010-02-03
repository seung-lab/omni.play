for file in $(grep -l $1 /Users/didier/trunk/omni/src/*/*.cpp /Users/didier/trunk/omni/src/*/*.h);

do
  echo ====================================================================================
  echo $file;
  echo ====================================================================================
  echo
  echo

  for number in $(grep -n $1 $file | awk 'BEGIN{ FS=":"}{print $1}');
      
  do
       echo ------------------------------------------------------------------------------------
       echo line $number :
       let plus=number+2;
       head -n$plus $file | tail -n5;
       echo ------------------------------------------------------------------------------------
       echo
  done;
  echo;
  echo;
  echo;
done;

        

     