<?php

// from http://stackoverflow.com/questions/2433868/how-to-create-a-tcp-client-to-send-commands-to-a-server-using-php
function socket_get($ip, $port, $data)
{
  // Create a TCP Stream Socket
  $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
  if ($socket === false)
    throw new Exception("Socket Creation Failed");

  // Connect to the server.
  $result = socket_connect($socket, $ip, $port);
  if ($result === false)
    throw new Exception("Connection Failed");

  // Write to socket!
  socket_write($socket, $data, strlen($data));

  $output = "";

  // Read from socket!
  do {
    $line = socket_read($socket, 1024);
    $output .= $line;
  } while ($line != "");

  // Close and return.
  socket_close($socket);

  $output = trim($output);

  return $output;
}