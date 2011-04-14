<?php

include('../engine/show_dynamic_image.php');
include('../server/omni_server.php');

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

$slice_url = 'http://localhost:8585/temp_omni_imgs/segmentation-1/' . $uuid . '.jpg';
$full_path = $slice_url;

$fc = file_get_contents($full_path);

show_dynamic_image($fc);
