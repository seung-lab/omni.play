<?php

include('../server/omni_server.php');

$jsonArray = array('action' => "change_threshold",
                   'segmentationID' => intval($_GET['segmentationID']),
                   'threshold' => intval($_GET['threshold']),
    );

print talk_omni_server($jsonArray);
