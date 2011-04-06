<?php

$slice_num = $_GET['slice_num'];

$output = array();
$command = '/home/purcaro.local/omni.dev/omni/bin/omni --client --clientCMD ' . $slice_num;
exec($command, $output);


// $url = 'http://192.168.1.71/temp_omni_imgs/chann'$output[0];
print $output[0];














?>