<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	<head>
		<title>{$title}</title>

		<link href="./css/general.css" rel="stylesheet" type="text/css" />
		<link href="./css/homepage.css" rel="stylesheet" type="text/css" />
		<link href="./css/page_header.css" rel="stylesheet" type="text/css" />
		<link href="./css/page_contents.css" rel="stylesheet" type="text/css" />
		<link href="./css/page_footer.css" rel="stylesheet" type="text/css" />

		<link href="./js/jquery_themes/css/smoothness/jquery-ui-1.8.10.custom.css" rel="stylesheet" type="text/css"/>
		
		{section name=i loop=$style_sheets}
		<link href="{$style_sheets[i].href}" rel="stylesheet" type="text/css" />
		{/section}

		<script src="http://code.jquery.com/jquery-1.4.4.min.js" type="text/javascript"></script>
		<script src="js/lib/util.js" type="text/javascript"></script>
		<script src="js/lib/debug.js" type="text/javascript"></script>
		<script src="js/lib/jquery-ui-1.8.9.custom.min.js" type="text/javascript"></script>
		
		{section name=i loop=$scripts}
		<script src="{$scripts[i].src}" type="text/javascript"></script>
		{/section}

	</head>

	<body onload="init()">
		<div class="wrapper-splash">
			<div id="pusher"></div>
			<div id="pageHeader" class="page_header">

				<div id="logoContainer" class="logo_container">
					<div id="logo" class="logo-large" >
					</div>
				</div>
				<div class="top_nav_container">
					<ul class="top_nav left">
						{section name = i loop = $top_nav}
							{$item = $top_nav[i]}
							{if $item['active']}
								{$active = 'active'}
							{else}
								{$active = ''}
							{/if}
						<li class="item {$active}">
							<div>
								<a href="{$item['href']}">{$item['name']}</a>
							</div>
						</li>
						{/section}
						<!-- <li class="item active	">
							<div>
								<a href="about.php">About</a>
							</div>
						</li>
						<li class="item">
							<div>
								<a>Data</a>
							</div>
						</li>
						<li class="item">
							<div>
								<a>People</a>
							</div>
						</li>
						<li class="item">
							<div>
								<a>Publications</a>
							<div>
						</li>
						<li class="item">
							<div>
								<a>Contact</a>
							<div>
						</li> -->
					</ul>
					<div style="clear: both"></div>
				</div>

			</div>
			
			
			<div class="page_contents_container">
				<div class="page_contents">
			
			
			
			
			
			
			