<?php

session_start();

require_once('3rdparty/Smarty-3.0.7/libs/Smarty.class.php');

// create object
$smarty = new Smarty();
$smarty->setTemplateDir(getcwd() . '/templates');
$smarty->setCompileDir(getcwd() . '/templates_c');

$title = 'Omni.Web | Navigate the Mind';


$top_nav = array(array('name' => 'Home', 'href' => 'index.php'),
				array('name' => 'About', 'href' => 'about.php'),
				array('name' => 'Data', 'href' => '', 'active' => true),
				array('name' => 'People', 'href' => ''),
				array('name' => 'Publications', 'href' => ''),
				array('name' => 'Contact', 'href' => '')
);


$scripts = array(
	array('src' => './js/view.js'),
	array('src' => './js/lib/jquery.mousewheel.min.js')
);

$style_sheets = array(
	array('href' => './css/view.css')
);


$smarty->assign('title', $title);
$smarty->assign('top_nav', $top_nav);
$smarty->assign('scripts', $scripts);
$smarty->assign('style_sheets', $style_sheets);
$smarty->display('view.tpl');



?>