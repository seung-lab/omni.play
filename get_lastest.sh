#set user and token in the cronjob
# user=""
# token=

lastest_ver_name=$(curl -u  $user:$token  -s https://api.github.com/repos/seung-lab/omni.play/releases/latest | grep "tag_name" | head -n 1 | cut -d '"' -f 4)
lastest_ver=$(echo $lastest_ver_name | cut -c 8-)

echo $lastest_ver
# Check if this is a newer version
for f in ~/seungmount/Omni/omni/versions/omni.linux-b* 
do   
  file_ver=$(echo $f | cut -c 59-)
  echo $file_ver
  if (( $lastest_ver >= $file_ver )) 
  then
    echo "We alread have the lastest version"
    exit 0
  fi 
done


echo "Downloading new version "+$lastest_ver_name
#Get the url to the asset ( github saves the releases in amazon )
omni=$(curl  -H "Authorization: token $token" -s https://api.github.com/repos/seung-lab/omni.play/releases/latest | python -c "import json,sys;obj=json.load(sys.stdin);print obj['assets'][0]['url'];")
aws_path=$(curl  -i  -H "Authorization: token $token" -H "Accept:application/octet-stream" "$omni" | grep location | cut -c 11- | rev | cut -c 2- | rev )


#save the new file
filename="omni."
filename+=$lastest_ver_name
curl -o ~/seungmount/Omni/omni/versions/$filename "$aws_path"
chmod +x ~/seungmount/Omni/omni/versions/$filename

#update link
rm -f ~/seungmount/Omni/omni/lastest_omni
ln ~/seungmount/Omni/omni/versions/$filename ~/seungmount/Omni/omni/lastest_omni
