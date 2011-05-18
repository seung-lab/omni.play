<?php

include_once('show_dynamic_image.php');
include_once('../server/omni_server.php');

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

if(!$fc)
{
    $uuid = talk_omni_server($jsonArray);

    $path = '../images/data/channel-1/' . $uuid . '.jpg';
    $fc = file_get_contents($path);
    $memcache->set($query, $fc);
}

show_dynamic_image($fc);
