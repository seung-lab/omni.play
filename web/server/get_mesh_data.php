<?php

include('../server/omni_server.php');

$jsonArray = array(
    "action" => "get_mesh_data",
    "segmentationID" => intval(1),
    "segID" => intval(464),
    "mip" => intval(0),
    "x" => intval(0),
    "y" => intval(0),
    "z" => intval(0),
    );

print talk_omni_server($jsonArray);
