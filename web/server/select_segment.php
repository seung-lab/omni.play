<?php

include('omni_server.php');

$json = array();
$json['action'] = "select_segment";

$json['x'] = intval($_GET['x']);
$json['y'] = intval($_GET['y']);
$json["slice_num"] = intval($_GET['slice_num']);
$json['w'] = intval($_GET['w']);
$json['h'] = intval($_GET['h']);

print talk_omni_server($json);
