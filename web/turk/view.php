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

		<link href="./css/view.css" rel="stylesheet" type="text/css" />

		<script src="http://code.jquery.com/jquery-1.4.4.min.js" type="text/javascript"></script>
		<script src="js/lib/util.js" type="text/javascript"></script>
		<script src="js/lib/debug.js" type="text/javascript"></script>
		<script src="js/lib/jquery-ui-1.8.9.custom.min.js" type="text/javascript"></script>

		<script src="./js/lib/jquery.mousewheel.min.js" type="text/javascript"></script>
		<script src="./js/view.js" type="text/javascript"></script>

	</head>

	<body onload="init()">
		<div class="wrapper-internal">
			<div id="pageHeader" class="page_header">

				<div id="logoContainer">
				        <div id="logo" class="logo" style="float: left;" onclick="window.location = './index.php';">
                                        </div>
                                        <div class="turk">
                                          Turk
                                        </div>
					<div style="float: right;">
						<input type="button" value="Logout" style="font-size: 17px;" onclick="window.location = './index.php';">
					</div>
					<div style="clear: both;"></div>
				</div>

			</div>

			<div class="page_contents_container">
				<div class="page_contents">

					<div class="view">
						<table>
							<tbody>
								<tr>
									<td class="toolbar_column" style="">
										<div class="left_panel panel" style="">
											<div class="section">
												<div class="header" id="sliceNumberTitle">
                                                                                                       Slice Number:
												</div>
											</div>
										</div>
									</td>
									<td class="view_canvas_column">
										<div id="viewCanvasContainer" class="view_canvas_container view_canvas_container-load">
											<div id="viewCanvas" class="view_canvas" style="display: ;">
												<div id="channelCanvas" class="channel_canvas"></div>
                                                                                                      <img id="channelCanvasImg"/>
												<div id="segmentationCanvas" class="segmentation_canvas"></div>
											</div>
										</div>
									</td>
									<td style="">
										<div class="right_panel panel" style="">
										</div>
									</td>
								</tr>
							</tbody>
						</table>

					</div>

				</div>
			</div>

			<div class="page_footer">
				<div class="bottom_nav">
					<ul class="left">
						<li class="item">
							<div><a>Home</a></div>
						</li>
						<li class="item">
							<div><a>Research</a></div>
						</li>
						<li class="item">
							<div><a>About</a></div>
						</li>
					</ul>
					<ul class="right">
						<li class="item">
							<div>&#169; Omni.Web 2011</div>
						</li>
					</ul>
				</div>
			</div>
		</div>
	</body>
</html>
