<?php

include('omni_server.php');

$jsonArray = array(
    "action" => "get_mesh_data",
    "segID" => 464,
    "mip" => 0,
    "x" => 0,
    "y" => 0,
    "z" => 0,
    );

print talk_omni_server($jsonArray);
