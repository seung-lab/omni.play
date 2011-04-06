<?php


// get the current channel based on the channel number in the $_GET parameters

$slice_num = $_GET['slice_num'];

// $full_path = '../test_images/large-' . ($slice_num % 2) . '.png';
// $full_path = '../test_images/brain-' . ($slice_num % 2) . '.png';

$url = 'http://localhost:8585/omniweb/server/generate_slice_uuid.php?slice_num=' . $slice_num;
// print $url;
// exit();

$uuid = file_get_contents($url);

$slice_url = 'http://localhost:8585/temp_omni_imgs/channel-1/' . $uuid . '.png';

// print $slice_url;
// exit();

$full_path = $slice_url;

if($fd = fopen ($full_path, "r")) {
	
	// $file_info = finfo_open(FILEINFO_MIME_TYPE);
	// $mime_type = finfo_file($file_info, $full_path);
	
	// $file_size = filesize($full_path);
	// $path_parts = pathinfo($full_path);
	// $file_name = $path_parts["basename"];
	// $ext = strtolower($path_parts["extension"]);
	// switch ($ext) {
	// 	case "pdf":
	// 	header("Content-type: application/pdf"); // add here more headers for diff. extensions
	// 	header("Content-Disposition: attachment; filename=\"".$path_parts["basename"]."\""); // use 'attachment' to force a download
	// 	break;
	// 	default;
	// 	header("Content-type: application/octet-stream");
	// 	header("Content-Disposition: filename=\"".$path_parts["basename"]."\"");
	// }
	header("Content-type: image/png"); // add here more headers for diff. extensions
	// header("Content-length: $file_size");
	header("Cache-control: private"); //use this to open files directly
	while(!feof($fd)) {
		$buffer = fread($fd, 2048);
		echo $buffer;
	}
}
fclose ($fd);
exit;



?>