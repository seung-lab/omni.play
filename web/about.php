<?php

session_start();

require_once('3rdparty/Smarty-3.0.7/libs/Smarty.class.php');

// create object
$smarty = new Smarty();
$smarty->setTemplateDir(getcwd() . '/templates');
$smarty->setCompileDir(getcwd() . '/templates_c');

$title = 'Omni.Web | About';

$top_nav = array(array('name' => 'Home', 'href' => 'index.php'),
				array('name' => 'About', 'href' => 'about.php', 'active' => true),
				array('name' => 'Data', 'href' => ''),
				array('name' => 'People', 'href' => ''),
				array('name' => 'Publications', 'href' => ''),
				array('name' => 'Contact', 'href' => '')
);


$smarty->assign('title', $title);
$smarty->assign('top_nav', $top_nav);
$smarty->assign('scripts', $scripts);
$smarty->display('about.tpl');



?>