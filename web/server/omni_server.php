<?php

include_once('tcp_socket.php');

function talk_omni_server($jsonArray)
{
    $json = json_encode($jsonArray);

    return socket_get("127.0.0.1", 8584, $json);
}