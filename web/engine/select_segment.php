<?php

include('../server/omni_server.php');

$json = array('action' => "select_segment",
              'segmentationID' => intval(1),
              'x' => intval($_GET['x']),
              'y' => intval($_GET['y']),
              "slice_num" => intval($_GET['slice_num']),
              'w' => intval($_GET['w']),
              'h' => intval($_GET['h']),
    );

print talk_omni_server($json);
