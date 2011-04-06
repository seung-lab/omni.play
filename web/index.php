<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	<head>
		<title>Omni.Web</title>
		
		<link href="./css/general.css" rel="stylesheet" type="text/css" />
		<link href="./css/homepage.css" rel="stylesheet" type="text/css" />
		<link href="./css/page_header.css" rel="stylesheet" type="text/css" />
		<link href="./css/page_contents.css" rel="stylesheet" type="text/css" />
		<link href="./css/page_footer.css" rel="stylesheet" type="text/css" />
		
		<link href="./js/jquery_themes/css/smoothness/jquery-ui-1.8.10.custom.css" rel="stylesheet" type="text/css"/>
		
		<script src="http://code.jquery.com/jquery-1.4.4.min.js" type="text/javascript"></script>
		<script src="js/lib/util.js" type="text/javascript"></script>
		<script src="js/lib/debug.js" type="text/javascript"></script>
		<script src="js/lib/jquery-ui-1.8.9.custom.min.js" type="text/javascript"></script>
		<script src="js/lib/jquery.md5.js" type="text/javascript"></script>
		<script src="js/lib/jquery.address-1.3.1.min.js" type="text/javascript"></script>
		
		<script src="./js/homepage.js" type="text/javascript"></script>
		
	</head>
	
	<body onload="init()">
		<div class="wrapper-splash">
			<div id="pusher"></div>
			<div id="pageHeader" class="page_header">
				
				<div id="logoContainer">
					<div id="logo" class="logo-large" style="display: none;">
					</div>
					<div id="logo-subtitle" class="subtitle" style="display: none;">
						navigate the mind.
					</div>
				</div>
				
			</div>
			
			<div class="page_contents_container">
				<div class="page_contents">
					
					<div id="loginContainer" class="login_container" style="display: none;">
						<form id="login" class="login" action="./view.php">
							<input id="login-email" onfocus="toggleDefaultText(this, 'Email')" onblur="toggleDefaultText(this, 'Email')" class="input-text" type="text" value="Email" />
							<input id="login-password-fake" class="input-text" type="text" value="Password" onfocus="toggleLoginPasswordField(true)" />
							<input id="login-password" class="input-text" type="password" value="" onblur="toggleLoginPasswordField(false)" style="display: none;" />
							<input id="login-submit" class="input-submit" type="submit" value="Login" />
						</form>
					</div>
					
				</div>
			</div>
			
		</div>
	</body>
</html>