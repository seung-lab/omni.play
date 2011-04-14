<?php

include('../server/omni_server.php');

$jsonArray = array("action" => "get_project_info");

print talk_omni_server($jsonArray);
