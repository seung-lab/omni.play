<?php

include_once('../server/omni_server.php');

$jsonArray = array("action" => "get_segmentation_dim"
    );

print talk_omni_server($jsonArray);

