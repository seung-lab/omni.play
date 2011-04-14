<?php

include('show_dynamic_image.php');

// $full_path = '../test_images/brain-' . ($slice_num % 2) . '.png';

$uuid = '';

if(isset($_GET['get_uuid'])){
    $uuid = $_GET['get_uuid'];

} else {
    $queryArray = array("project" => "fixme",
                        "action" => "get_slice_segmentation",
                        "slice_num" => $_GET['slice_num'],
                        "freshness" => "fixme"
        );
    $query = http_build_query($queryArray);
    $url = 'http://localhost:8585/omniweb/server/generate_slice_uuid.php?' . $query;
    $uuid = file_get_contents($url);
}

$slice_url = 'http://localhost:8585/temp_omni_imgs/segmentation-1/' . $uuid . '.png';
$full_path = $slice_url;

$fc = file_get_contents($full_path);

show_dynamic_image($fc);
