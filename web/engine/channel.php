<?php

include('show_dynamic_image.php');
include('../server/omni_server.php');

define('MEMCACHED_PORT', 11211);
$memcache = new Memcached;
$memcache->addServer('localhost', MEMCACHED_PORT);

// get the current channel based on the channel number in the $_GET parameters

$jsonArray = array("project" => "fixme",
                   "action" => "get_slice_channel",
                   "slice_num" => intval($_GET['slice_num'])
    );

$query = http_build_query($jsonArray);

$fc = $memcache->get($query);

if ( !$fc )
{
    $uuid = talk_omni_server($jsonArray);

    $slice_url = 'http://localhost:8585/temp_omni_imgs/channel-1/' . $uuid . '.jpg';
    $full_path = $slice_url;
    $fc = file_get_contents($full_path);
    $memcache->set($query, $fc);
}

show_dynamic_image($fc);
