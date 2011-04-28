<?php

include_once('../engine/show_dynamic_image.php');
include_once('../server/omni_server.php');

$uuid = '';

if(isset($_GET['get_uuid'])){
    $uuid = $_GET['get_uuid'];

} else {
    $jsonArray = array("project" => "fixme",
                       "action" => "get_tile_channel",
                       "slice_num" => intval($_GET['slice_num']),
                       "tileX" => intval($_GET['tileX']),
                       "tileY" => intval($_GET['tileY'])
        );
    $uuid = talk_omni_server($jsonArray);
}

$slice_url = 'http://localhost:8585/temp_omni_imgs/channel-1/' . $uuid . '.tile.jpg';
$full_path = $slice_url;

$fc = file_get_contents($full_path);

show_dynamic_image($fc);
