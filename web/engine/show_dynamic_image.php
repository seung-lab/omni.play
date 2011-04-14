<?php

function show_dynamic_image($fileContents)
{
    if ($fileContents) {
        header("Content-type: image/png"); // add here more headers for diff. extensions
        header("Content-: image/png"); // add here more headers for diff. extensions
        //header("Content-encoding: gzip");
        //$data = gzencode($fileContents);
        header("Content-length: " . strlen($fileContents) );
        header("Cache-control: private"); //use this to open files directly
        print($fileContents);
    }
}