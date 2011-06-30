<?php

$sliceNum = intval($_GET['slice_num']);

$folder = "/omniWebData/jpeg512stack/";

$imageFileBase = "_affinecropped5realigned_2k_6nm_512.jpg";
$imageFile = sprintf("%04d%s", $sliceNum, $imageFileBase);

echo $folder . $imageFile;
