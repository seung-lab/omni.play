<?php

include('show_dynamic_image.php');

define('MEMCACHED_PORT', 11211);
$memcache = new Memcached;
$memcache->addServer('localhost', MEMCACHED_PORT);

// get the current channel based on the channel number in the $_GET parameters

$queryArray = array("project" => "fixme",
                    "action" => "get_slice_channel",
                    "slice_num" => $_GET['slice_num']
    );

$query = http_build_query($queryArray);

$fc = $memcache->get($query);
if ( !$fc )
{
    $url = 'http://localhost:8585/omniweb/server/generate_slice_uuid.php?' . $query;
    $uuid = file_get_contents($url);
    $slice_url = 'http://localhost:8585/temp_omni_imgs/channel-1/' . $uuid . '.png';
    $full_path = $slice_url;
    $fc = file_get_contents($full_path);
    $memcache->set($query, $fc);
}

show_dynamic_image($fc);
