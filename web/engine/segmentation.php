<?php

include_once('show_dynamic_image.php');
include_once('../server/omni_server.php');

// $full_path = '../test_images/brain-' . ($slice_num % 2) . '.png';

$uuid = '';

if(isset($_GET['get_uuid'])){
    $uuid = $_GET['get_uuid'];

} else {
    $jsonArray = array("project" => "fixme",
                       "action" => "get_slice_segmentation",
                       "slice_num" => intval($_GET['slice_num']),
                       "freshness" => "fixme"
        );
    $uuid = talk_omni_server($jsonArray);
}

$path = '../images/data/segmentation-1/' . $uuid . '.jpg';

$fc = file_get_contents($path);

show_dynamic_image($fc);
