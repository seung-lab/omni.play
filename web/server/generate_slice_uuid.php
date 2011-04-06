<?php

include('omni_server.php');

$jsonArray = array(
    "action" => $_GET['action'],
    "slice_num" => intval($_GET['slice_num'])
    );

print talk_omni_server($jsonArray);
